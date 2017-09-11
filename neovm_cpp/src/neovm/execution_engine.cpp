#include <neovm/execution_engine.hpp>
#include <neovm/execution_context.hpp>
#include <neovm/types.hpp>
#include <neovm/stack_item.hpp>
#include <neovm/exceptions.hpp>

#include <math.h>

namespace neo
{
	namespace vm
	{
		ExecutionEngine::ExecutionEngine(IScriptContainer *container, ICrypto *crypto, IScriptTable *table, InteropService *service)
		{
			_script_container = container;
			_crypto = crypto;
			_table = table;
			_service = service ? service : new InteropService();
			_state = VMState::BREAK;
			_exit_code = ErrorCode::OK;
			_debug_mode = false;
			_gas_limit = -1;
			_gas_used = 0;
		}

		void ExecutionEngine::add_break_point(uint64_t position)
		{
			current_context()->break_points()->insert(position);
		}

		void ExecutionEngine::add_pre_close_callbacks(ExecutioEngineCallback callback)
		{
			_pre_close_callbacks.push_back(callback);
		}

		VMState ExecutionEngine::state() const
		{
			return _state;
		}

		ErrorCode ExecutionEngine::exit_code() const
		{
			return _exit_code;
		}

		void ExecutionEngine::open_debug_mode()
		{
			_debug_mode = true;
		}
		void ExecutionEngine::close_debug_mode()
		{
			_debug_mode = false;
		}
		bool ExecutionEngine::in_debug_mode() const
		{
			return _debug_mode;
		}

		bool ExecutionEngine::has_gas_limit() const
		{
			return _gas_limit >= 0;
		}
		int64_t ExecutionEngine::gas_limit() const
		{
			return _gas_limit;
		}
		int64_t ExecutionEngine::gas_used() const
		{
			return _gas_used;
		}
		void ExecutionEngine::set_gas_limit(int64_t gas_limit)
		{
			_gas_limit = gas_limit;
		}
		void ExecutionEngine::set_no_gas_limit()
		{
			_gas_limit = -1;
		}
		void ExecutionEngine::set_gas_used(int64_t gas_used)
		{
			_gas_used = gas_used;
		}
		void ExecutionEngine::add_gas_used(int64_t delta_used)
		{
			_gas_used += delta_used;
		}

		void ExecutionEngine::stop()
		{
			set_gas_limit(0); // 0 is don't exeucte any script op
		}

		ExecutionEngine::~ExecutionEngine()
		{
			while (_invocation_stack.size() > 0)
			{
				auto last = _invocation_stack.peek();
				delete last;
				_invocation_stack.pop();
			}

			for (const auto &cb : _pre_close_callbacks)
			{
				cb(this);
			}
			_pre_close_callbacks.clear();

			auto &pool = _stack_items_pool.pool();
			for (auto &item : pool)
			{
				delete item;
			}
			pool.clear();
		}

		void ExecutionEngine::execute()
		{
			_state = (VMState) (_state & ~VMState::BREAK);
			auto i = 0;
			while (!Helper::enum_has_flag(_state, VMState::HALT) && !Helper::enum_has_flag(_state, VMState::FAULT) && !Helper::enum_has_flag(_state, VMState::BREAK))
			{
				if (in_debug_mode())
				{
					std::cout << (i++) << ":";
				}
				step_into();
			}
		}

		void ExecutionEngine::load_script(std::vector<char> script, std::vector<char> script_id, bool push_only = false)
		{
			_invocation_stack.push(new ExecutionContext(this, script, script_id, push_only, std::set<uint64_t>()));
		}

		bool ExecutionEngine::remove_break_point(uint64_t position)
		{
			if (_invocation_stack.size() == 0) 
				return false;
			if (current_context()->break_points()->find(position) == current_context()->break_points()->end())
				return false;
			current_context()->break_points()->erase(position);
			return true;
		}

		void ExecutionEngine::step_into()
		{
			if (_invocation_stack.size() == 0) _state = (VMState)(_state | VMState::HALT);
			if (Helper::enum_has_flag(_state, VMState::HALT) || Helper::enum_has_flag(_state, VMState::FAULT)) return;
			OpCode opcode = current_context()->get_instruction_pointer() >= current_context()->script()->size() ? OpCode::RET : (OpCode)((byte)current_context()->op_reader()->ReadByte());
			try
			{
				ExecuteOp(opcode, current_context());
			}
			catch (NeoVmException &e)
			{
				_exit_code = e.code();
				union_change_state(VMState::FAULT);
				throw e;
			}
			catch (std::exception &e)
			{
				_exit_code = ErrorCode::SIMPLE_ERROR;
				union_change_state(VMState::FAULT);
				throw e;
			}
		}

		void ExecutionEngine::step_out()
		{
			_state = (VMState)(_state & ~VMState::BREAK);
			int c = _invocation_stack.size();
			while (!Helper::enum_has_flag(_state, VMState::HALT) && !Helper::enum_has_flag(_state, VMState::FAULT) && !Helper::enum_has_flag(_state, VMState::BREAK) && _invocation_stack.size() >= c)
			{
				step_into();
			}
		}

		void ExecutionEngine::step_over()
		{
			if (Helper::enum_has_flag(_state, VMState::HALT) || Helper::enum_has_flag(_state, VMState::FAULT)) return;
			_state = (VMState) (_state & ~VMState::BREAK);
			int c = _invocation_stack.size();
			do
			{
				step_into();
			} while (!Helper::enum_has_flag(_state, VMState::HALT) && !Helper::enum_has_flag(_state, VMState::FAULT) && !Helper::enum_has_flag(_state, VMState::BREAK) && _invocation_stack.size() > c);
		}

		ExecutionContext* ExecutionEngine::pop_from_invocation_stack()
		{
			if (_invocation_stack.size()<1)
			{
				throw NeoVmException("empty invocation stack can't pop");
			}
			auto last = _invocation_stack.peek();
			_invocation_stack.pop();
			return last;
		}

		ExecutionContext* ExecutionEngine::current_context() const
		{
			return _invocation_stack.peek();
		}

		ExecutionContext* ExecutionEngine::calling_context() const
		{
			return (_invocation_stack.size() > 1) ? _invocation_stack.peek(1) : nullptr;
		}

		ExecutionContext* ExecutionEngine::entry_context() const
		{
			return _invocation_stack.peek(_invocation_stack.size() - 1);
		}

		ICrypto* ExecutionEngine::crypto() const
		{
			return _crypto;
		}

		RandomAccessStack<StackItem*>* ExecutionEngine::evaluation_stack()
		{
			return &_evaluation_stack;
		}

		IScriptContainer* ExecutionEngine::script_container() const
		{
			return _script_container;
		}

		void ExecutionEngine::add_stack_item_to_pool(StackItem *obj)
		{
			_stack_items_pool.add(obj);
		}

		void ExecutionEngine::union_change_state(VMState other)
		{
			_state = (VMState)(_state | other);
		}

		void ExecutionEngine::register_global_variable(std::string name, StackItem *value)
		{
			_global_env[name] = value;
		}

		void ExecutionEngine::register_string_global_variabble(std::string name, std::string value)
		{
			_global_env[name] = StackItem::to_stack_item(this, value);
		}

		StackItem* ExecutionEngine::get_global_variable_value(std::string name)
		{
			if (_global_env.find(name) != _global_env.end())
				return _global_env[name];
			else
				return nullptr;
		}

		void ExecutionEngine::set_container_value(std::string name, StackItem *value)
		{
			if (value)
				_container_values.erase(name);
			else
				_container_values[name] = value;
		}
		StackItem* ExecutionEngine::get_container_value(std::string name)
		{
			if (_container_values.find(name) != _container_values.end())
				return _container_values[name];
			else
				return nullptr;
		}

		void ExecutionEngine::ExecuteOp(OpCode opcode, ExecutionContext *context)
		{
			if (opcode > OpCode::PUSH16 && opcode != OpCode::RET && context->push_only())
			{
				_state = (VMState) (_state | VMState::FAULT);
				return;
			}
			if (in_debug_mode())
			{
				std::cout << "op: " << op_code_to_str(opcode) << " before eval stack size is: " << std::to_string(evaluation_stack()->size()) << std::endl;
			}
			if (has_gas_limit() && _gas_used >= _gas_limit)
			{
				throw NeoVmException("gas used out of limit");
			}
			++_gas_used;

			if (opcode >= OpCode::PUSHBYTES1 && opcode <= OpCode::PUSHBYTES75)
				_evaluation_stack.push(StackItem::to_stack_item(this, context->op_reader()->ReadBytes((char)opcode)));
			else
			{ 
				switch (opcode)
				{
					// Push value
				case OpCode::PUSH0:
					_evaluation_stack.push(StackItem::to_stack_item(this, std::vector<char>()));
					break;
				case OpCode::PUSHDATA1:
					_evaluation_stack.push(StackItem::to_stack_item(this, context->op_reader()->ReadBytes(context->op_reader()->ReadByte())));
					break;
				case OpCode::PUSHDATA2:
					_evaluation_stack.push(StackItem::to_stack_item(this, context->op_reader()->ReadBytes(context->op_reader()->ReadUInt16())));
					break;
				case OpCode::PUSHDATA4:
					_evaluation_stack.push(StackItem::to_stack_item(this, context->op_reader()->ReadBytes(context->op_reader()->ReadInt32())));
					break;
				case OpCode::PUSHM1:
				case OpCode::PUSH1:
				case OpCode::PUSH2:
				case OpCode::PUSH3:
				case OpCode::PUSH4:
				case OpCode::PUSH5:
				case OpCode::PUSH6:
				case OpCode::PUSH7:
				case OpCode::PUSH8:
				case OpCode::PUSH9:
				case OpCode::PUSH10:
				case OpCode::PUSH11:
				case OpCode::PUSH12:
				case OpCode::PUSH13:
				case OpCode::PUSH14:
				case OpCode::PUSH15:
				case OpCode::PUSH16:
					_evaluation_stack.push(StackItem::to_stack_item(this, (int)opcode - (int)OpCode::PUSH1 + 1));
					break;

					// Control
				case OpCode::NOP:
					break;
				case OpCode::JMP:
				case OpCode::JMPIF:
				case OpCode::JMPIFNOT:
				{
					int offset = context->op_reader()->ReadInt16();
					offset = context->get_instruction_pointer() + offset - 3;
					if (offset < 0 || offset > context->script()->size())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					bool fValue = true;
					if (opcode > OpCode::JMP)
					{
						fValue = _evaluation_stack.pop()->GetBoolean();
						if (opcode == OpCode::JMPIFNOT)
							fValue = !fValue;
					}
					if (fValue)
						context->set_instruction_pointer(offset);
				}
				break;

				case OpCode::CALL:
					_invocation_stack.push(context->clone());
					context->set_instruction_pointer(context->get_instruction_pointer() + 2);
					ExecuteOp(OpCode::JMP, current_context());
					break;
				case OpCode::RET:
				{
					if (_invocation_stack.size() < 1)
					{
						throw NeoVmException("empty invocation stack to pop");
					}
					delete _invocation_stack.pop();
					if (_invocation_stack.size() == 0)
						union_change_state(VMState::HALT);
				}
					break;
				case OpCode::APPCALL:
				case OpCode::TAILCALL:
				{
					if (_table == nullptr)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto script_id = Helper::bytes_to_string(context->op_reader()->ReadBytes(20)); // TODO: maybe need change to read string directly as script_id
					auto script = _table->get_script(script_id);
					if (script.size() < 1)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					if (opcode == OpCode::TAILCALL)
						delete _invocation_stack.pop();
					load_script(script, Helper::string_content_to_chars(script_id));
				}
				break;
				case OpCode::SYSCALL:
				{
					auto bytes = Helper::ReadVarBytes(context->op_reader(), 252);
					auto func_name = Helper::bytes_to_string(bytes);
					if (in_debug_mode())
					{
						std::cout << "syscall " << func_name << std::endl;
					}
					if (!_service->invoke(func_name, this))
					{
						if (in_debug_mode())
						{
							std::cout << "Can't find syscall " + func_name << std::endl;
						}
						union_change_state(VMState::FAULT);
					}
					break;
				}

					// Stack ops
				case OpCode::DUPFROMALTSTACK:
					_evaluation_stack.push_back(Helper::peek(_alt_stack));
					break;
				case OpCode::TOALTSTACK:
					_alt_stack.push_back(_evaluation_stack.pop());
					break;
				case OpCode::FROMALTSTACK:
					_evaluation_stack.push(Helper::pop(_alt_stack));
					break;
				case OpCode::XDROP:
				{
					int n = (int)(_evaluation_stack.pop()->GetBigInteger());
					if (n < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					_evaluation_stack.remove(n);
				}
				break;
				case OpCode::XSWAP:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					if (n < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					if (n == 0) break;
					auto xn = _evaluation_stack.peek(n);
					_evaluation_stack.set(n, _evaluation_stack.peek());
					_evaluation_stack.set(0, xn);
				}
				break;
				case OpCode::XTUCK:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					if (n <= 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					_evaluation_stack.insert(n, _evaluation_stack.peek());
				}
				break;
				case OpCode::DEPTH:
					_evaluation_stack.push_back(StackItem::to_stack_item(this, _evaluation_stack.size()));
					break;
				case OpCode::DROP:
					_evaluation_stack.pop();
					break;
				case OpCode::DUP:
					_evaluation_stack.push_back(_evaluation_stack.peek());
					break;
				case OpCode::NIP:
				{
					auto x2 = _evaluation_stack.pop();
					_evaluation_stack.pop();
					_evaluation_stack.push_back(x2);
				}
				break;
				case OpCode::OVER:
				{
					auto x2 = _evaluation_stack.pop();
					auto x1 = _evaluation_stack.peek();
					_evaluation_stack.push_back(x2);
					_evaluation_stack.push_back(x1);
				}
				break;
				case OpCode::PICK:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					if (n < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					_evaluation_stack.push(_evaluation_stack.peek(n));
				}
				break;
				case OpCode::ROLL:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					if (n < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					if (n == 0) break;
					_evaluation_stack.push_back(_evaluation_stack.remove(n));
				}
				break;
				case OpCode::ROT:
				{
					auto x3 =_evaluation_stack.pop();
					auto x2 = _evaluation_stack.pop();
					auto x1 = _evaluation_stack.pop();
					_evaluation_stack.push_back(x2);
					_evaluation_stack.push_back(x3);
					_evaluation_stack.push_back(x1);
				}
				break;
				case OpCode::SWAP:
				{
					auto x2 = _evaluation_stack.pop();
					auto x1 = _evaluation_stack.pop();
					_evaluation_stack.push_back(x2);
					_evaluation_stack.push_back(x1);
				}
				break;
				case OpCode::TUCK:
				{
					auto x2 = _evaluation_stack.pop();
					auto x1 = _evaluation_stack.pop();
					_evaluation_stack.push_back(x2);
					_evaluation_stack.push_back(x1);
					_evaluation_stack.push_back(x2);
				}
				break;
				case OpCode::CAT:
				{
					auto x2 = _evaluation_stack.pop()->GetByteArray();
					auto x1 = _evaluation_stack.pop()->GetByteArray();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, Helper::concat_vector(x1, x2)));
				}
				break;
				case OpCode::SUBSTR:
				{
					int count = (int)_evaluation_stack.pop()->GetBigInteger();
					if (count < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					int index = (int)_evaluation_stack.pop()->GetBigInteger();
					if (index < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto x = _evaluation_stack.pop()->GetByteArray();
					std::vector<char> result(count);
					memcpy(result.data(), x.data() + index, sizeof(char) * count);
					_evaluation_stack.push_back(StackItem::to_stack_item(this, result));
				}
				break;
				case OpCode::LEFT:
				{
					int count = (int)_evaluation_stack.pop()->GetBigInteger();
					if (count < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto x = _evaluation_stack.pop()->GetByteArray();
					std::vector<char> result(count);
					memcpy(result.data(), x.data(), sizeof(char) * count);
					_evaluation_stack.push_back(StackItem::to_stack_item(this, result));
				}
				break;
				case OpCode::RIGHT:
				{
					int count = (int)_evaluation_stack.pop()->GetBigInteger();
					if (count < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto x = _evaluation_stack.pop()->GetByteArray();
					if (x.size() < count)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					std::vector<char> result(count);
					memcpy(result.data(), x.data() + x.size() - count, sizeof(char) * count);
					_evaluation_stack.push_back(StackItem::to_stack_item(this, result));
				}
				break;
				case OpCode::SIZE:
				{
					auto x = _evaluation_stack.pop()->GetByteArray();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x.size()));
				}
				break;

				// Bitwise logic
				case OpCode::INVERT:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, ~x));
				}
				break;
				case OpCode::BIT_AND:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 & x2));
				}
				break;
				case OpCode::BIT_OR:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 | x2));
				}
				break;
				case OpCode::BIT_XOR:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 ^ x2));
				}
				break;
				case OpCode::EQUAL:
				{
					auto x2 = _evaluation_stack.pop();
					auto x1 = _evaluation_stack.pop();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1->Equals(x2)));
				}
				break;

				// Numeric
				case OpCode::INC:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x + 1));
				}
				break;
				case OpCode::DEC:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x - 1));
				}
				break;
				case OpCode::SIGN:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					// FIXME: �ĳ� _evaluation_stack.push_back(x.Sign);
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x));
				}
				break;
				case OpCode::NEGATE:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, -x));
				}
				break;
				case OpCode::ABS:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, std::abs(x)));
				}
				break;
				case OpCode::OP_NOT:
				{
					bool x = _evaluation_stack.pop()->GetBoolean();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, !x));
				}
				break;
				case OpCode::NZ:
				{
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x != 0));
				}
				break;
				case OpCode::ADD:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 + x2));
				}
				break;
				case OpCode::SUB:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 - x2));
				}
				break;
				case OpCode::MUL:
				{
					BigInteger x2 =_evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 * x2));
				}
				break;
				case OpCode::DIV:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 / x2));
				}
				break;
				case OpCode::MOD:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 % x2));
				}
				break;
				case OpCode::SHL:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x << n));
				}
				break;
				case OpCode::SHR:
				{
					int n = (int)_evaluation_stack.pop()->GetBigInteger();
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x >> n));
				}
				break;
				case OpCode::BOOLAND:
				{
					bool x2 = _evaluation_stack.pop()->GetBoolean();
					bool x1 = _evaluation_stack.pop()->GetBoolean();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 && x2));
				}
				break;
				case OpCode::BOOLOR:
				{
					bool x2 = _evaluation_stack.pop()->GetBoolean();
					bool x1 = _evaluation_stack.pop()->GetBoolean();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 || x2));
				}
				break;
				case OpCode::NUMEQUAL:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 == x2));
				}
				break;
				case OpCode::NUMNOTEQUAL:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 != x2));
				}
				break;
				case OpCode::LT:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 < x2));
				}
				break;
				case OpCode::GT:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 > x2));
				}
				break;
				case OpCode::LTE:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 <= x2));
				}
				break;
				case OpCode::GTE:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, x1 >= x2));
				}
				break;
				case OpCode::MIN:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 < x2 ? x1 : x2));
				}
				break;
				case OpCode::MAX:
				{
					BigInteger x2 = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x1 = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, x1 < x2 ? x2 : x1));
				}
				break;
				case OpCode::WITHIN:
				{
					BigInteger b = _evaluation_stack.pop()->GetBigInteger();
					BigInteger a = _evaluation_stack.pop()->GetBigInteger();
					BigInteger x = _evaluation_stack.pop()->GetBigInteger();
					_evaluation_stack.push_back(StackItem::to_stack_item_from_bool(this, a <= x && x < b));
				}
				break;

				/* TODO
				// Crypto
				case OpCode::SHA1:
				using (SHA1 sha = SHA1.Create())
				{
				byte[] x = Helper::pop(_evaluation_stack)->GetByteArray();
				_evaluation_stack.push_back(sha.ComputeHash(x));
				}
				break;
				case OpCode::SHA256:
				using (SHA256 sha = SHA256.Create())
				{
				byte[] x = Helper::pop(_evaluation_stack)->GetByteArray();
				_evaluation_stack.push_back(sha.ComputeHash(x));
				}
				break;
				case OpCode::HASH160:
				{
				byte[] x = Helper::pop(_evaluation_stack)->GetByteArray();
				_evaluation_stack.push_back(Crypto.Hash160(x));
				}
				break;
				case OpCode::HASH256:
				{
				byte[] x = Helper::pop(_evaluation_stack)->GetByteArray();
				_evaluation_stack.push_back(Crypto.Hash256(x));
				}
				break;
				case OpCode::CHECKSIG:
				{
				byte[] pubkey = Helper::pop(_evaluation_stack)->GetByteArray();
				byte[] signature = Helper::pop(_evaluation_stack)->GetByteArray();
				try
				{
				_evaluation_stack.push_back(Crypto.VerifySignature(ScriptContainer.GetMessage(), signature, pubkey));
				}
				catch (ArgumentException)
				{
				_evaluation_stack.push_back(false);
				}
				}
				break;
				case OpCode::CHECKMULTISIG:
				{
				int n = (int)Helper::pop(_evaluation_stack)->GetBigInteger();
				if (n < 1)
				{
				union_change_state(VMState::FAULT);
				return;
				}
				byte[][] pubkeys = new byte[n][];
				for (int i = 0; i < n; i++)
				pubkeys[i] = Helper::pop(_evaluation_stack)->GetByteArray();
				int m = (int)Helper::pop(_evaluation_stack)->GetBigInteger();
				if (m < 1 || m > n)
				{
				union_change_state(VMState::FAULT);
				return;
				}
				byte[][] signatures = new byte[m][];
				for (int i = 0; i < m; i++)
				signatures[i] = Helper::pop(_evaluation_stack)->GetByteArray();
				byte[] message = ScriptContainer.GetMessage();
				bool fSuccess = true;
				try
				{
				for (int i = 0, j = 0; fSuccess && i < m && j < n;)
				{
				if (Crypto.VerifySignature(message, signatures[i], pubkeys[j]))
				i++;
				j++;
				if (m - i > n - j)
				fSuccess = false;
				}
				}
				catch (ArgumentException)
				{
				fSuccess = false;
				}
				_evaluation_stack.push_back(fSuccess);
				}
				break;
				*/

				// Array
				case OpCode::ARRAYSIZE:
				{
					auto item = _evaluation_stack.pop();
					if (!item->IsArray())
						_evaluation_stack.push_back(StackItem::to_stack_item(this, item->GetByteArray().size()));
					else
						_evaluation_stack.push_back(StackItem::to_stack_item(this, item->GetArray()->size()));
				}
				break;
				case OpCode::PACK:
				{
					int size = (int)_evaluation_stack.pop()->GetBigInteger();
					if (size < 0 || size > _evaluation_stack.size())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					std::vector<StackItem*> items(size);
					for (int i = 0; i < size; i++)
						items[i] = _evaluation_stack.pop();
					_evaluation_stack.push_back(StackItem::to_stack_item(this, items));
				}
				break;
				case OpCode::UNPACK:
				{
					auto item = _evaluation_stack.pop();
					if (!item->IsArray())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto items = item->GetArray();
					for (int i = items->size() - 1; i >= 0; i--)
						_evaluation_stack.push_back((*items)[i]);
					_evaluation_stack.push_back(StackItem::to_stack_item(this, items->size()));
				}
				break;
				case OpCode::PICKITEM:
				{
					int index = (int)_evaluation_stack.pop()->GetBigInteger();
					if (index < 0)
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto item = _evaluation_stack.pop();
					if (!item->IsArray())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto items = item->GetArray();
					if (index >= items->size())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					_evaluation_stack.push_back((*items)[index]);
				}
				break;
				case OpCode::SETITEM:
				{
					auto newItem = _evaluation_stack.pop();
					if (newItem->IsStruct())
					{
						newItem = ((Struct*)newItem)->Clone(this);
					}
					int index = (int)_evaluation_stack.pop()->GetBigInteger();
					auto arrItem = _evaluation_stack.pop();
					if (!arrItem->IsArray())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					auto items = arrItem->GetArray();
					if (index < 0 || index >= items->size())
					{
						union_change_state(VMState::FAULT);
						return;
					}
					(*items)[index] = newItem;
				}
				break;
				case OpCode::NEWARRAY:
				{
					int count = (int)_evaluation_stack.pop()->GetBigInteger();
					std::vector<StackItem*> items(count);
					for (auto i = 0; i < count; i++)
					{
						items[i] = StackItem::to_stack_item_from_bool(this, false);
					}
					_evaluation_stack.push_back(StackItem::to_stack_item(this, items));
				}
				break;
				case OpCode::NEWSTRUCT:
				{
					int count = (int)_evaluation_stack.pop()->GetBigInteger();
					std::vector<StackItem*> items(count);
					for (auto i = 0; i < count; i++)
					{
						items[i] = false;
					}
					_evaluation_stack.push_back(StackItem::to_stack_struct_item(this, items));
				}
				break;
				// Exceptions
				case OpCode::THROW:
				{
					union_change_state(VMState::FAULT);
					return;
				}
				case OpCode::THROWIFNOT:
				{
					if (!_evaluation_stack.pop()->GetBoolean())
					{
						union_change_state(VMState::FAULT);
						return;
					}
				}
					break;

				default:
					union_change_state(VMState::FAULT);
					return;
				}
			}
			if (!Helper::enum_has_flag(_state, VMState::FAULT) && _invocation_stack.size() > 0)
			{
				if (current_context()->break_points()->find((uint64_t)current_context()->get_instruction_pointer()) != current_context()->break_points()->end())
					union_change_state(VMState::BREAK);
			}
		}

	}
}