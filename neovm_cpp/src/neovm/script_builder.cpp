#include <neovm/script_builder.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{
		size_t ScriptBuilder::Offset() const
		{
			return _position;
		}
		ScriptBuilder::ScriptBuilder()
			: _position(0)
		{}

		ScriptBuilder::~ScriptBuilder()
		{

		}

		ScriptBuilder* ScriptBuilder::emit(OpCode op, std::vector<byte> arg)
		{
			_ms.push_back((byte)op);
			if (arg.size() > 0)
			{
				for (const auto b : arg)
				{
					_ms.push_back(b);
				}
			}
			return this;
		}

		ScriptBuilder* ScriptBuilder::emit_app_call(std::vector<char> script_id, bool useTailCall)
		{
			if (script_id.size() < 1)
				throw NeoVmException("argument exception");
			return emit(useTailCall ? OpCode::TAILCALL : OpCode::APPCALL, Helper::chars_to_bytes(script_id));
		}

		ScriptBuilder* ScriptBuilder::emit_jump(OpCode op, int16_t offset)
		{
			if (op != OpCode::JMP && op != OpCode::JMPIF && op != OpCode::JMPIFNOT && op != OpCode::CALL)
				throw NeoVmException("argument exception");
			return emit(op, Helper::int16_to_bytes(offset));
		}

		ScriptBuilder* ScriptBuilder::emit_push(VMBigInteger number)
		{
			if (number == -1) return emit(OpCode::PUSHM1);
			if (number == 0) return emit(OpCode::PUSH0);
			if (number > 0 && number <= 16) return emit((OpCode)(OpCode::PUSH1 - 1 + (byte)number));
			return emit_push(Helper::big_integer_to_bytes(number));
		}

		ScriptBuilder* ScriptBuilder::emit_push(bool data)
		{
			return emit(data ? OpCode::PUSHT : OpCode::PUSHF);
		}

		ScriptBuilder* ScriptBuilder::emit_push(const char *str)
		{
			return emit_push_string(str);
		}

		ScriptBuilder* ScriptBuilder::emit_push_string(std::string str)
		{
			std::vector<byte> data(str.size());
			memcpy(data.data(), str.c_str(), sizeof(char) * str.size());
			return emit_push(data);
		}

		ScriptBuilder* ScriptBuilder::emit_push(std::vector<byte> data)
		{
			if (data.size() <= (int)OpCode::PUSHBYTES75)
			{
				_ms.push_back((byte)data.size());
				for (const auto b : data)
				{
					_ms.push_back(b);
				}
			}
			else if (data.size() < 0x100)
			{
				emit(OpCode::PUSHDATA1);
				_ms.push_back((byte)data.size());
				for (const auto b : data)
				{
					_ms.push_back(b);
				}
			}
			else if (data.size() < 0x10000)
			{
				emit(OpCode::PUSHDATA2);
				auto int16_data_size_bytes = Helper::int16_to_bytes((uint16_t)data.size());
				for (const auto b : int16_data_size_bytes)
				{
					_ms.push_back(b);
				}
				for (const auto b : data)
				{
					_ms.push_back(b);
				}
			}
			else// if (data.Length < 0x100000000L)
			{
				emit(OpCode::PUSHDATA4);
				auto data_size_bytes = Helper::int32_to_bytes((uint32_t)data.size());
				for (const auto b : data_size_bytes)
				{
					_ms.push_back(b);
				}
				for (const auto b : data)
				{
					_ms.push_back(b);
				}
			}
			return this;
		}

		ScriptBuilder* ScriptBuilder::emit_sys_call(std::string api)
		{
			if (api.empty())
				throw NeoVmException("argument exception");
			auto api_bytes = Helper::string_to_bytes(api);
			if (api_bytes.empty() || api_bytes.size() > 252)
				throw NeoVmException("argument exception");
			return emit(OpCode::SYSCALL, api_bytes);
		}

		std::vector<byte> ScriptBuilder::to_array()
		{
			return _ms;
		}

		std::vector<char> ScriptBuilder::to_char_array()
		{
			std::vector<char> result(_ms.size());
			memcpy(result.data(), _ms.data(), _ms.size() * sizeof(byte));
			return result;
		}
	}
}
