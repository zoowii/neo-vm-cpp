#include <neovm/interop_service.hpp>
#include <neovm/execution_engine.hpp>
#include <neovm/execution_context.hpp>
#include <neovm/types.hpp>
#include <neovm/exceptions.hpp>

#include <iostream>

namespace neo
{
	namespace vm
	{
		InteropService::InteropService()
		{
			register_service("System.ExecutionEngine.GetScriptContainer", GetScriptContainer);
			register_service("System.ExecutionEngine.GetExecutingScriptHash", GetExecutingScriptHash);
			register_service("System.ExecutionEngine.GetCallingScriptHash", GetCallingScriptHash);
			register_service("System.ExecutionEngine.GetEntryScriptHash", GetEntryScriptHash);

			register_service("System.ExecutionEngine.SetGlobal", SetGlobalVariable);
			register_service("System.ExecutionEngine.GetGlobal", GetGlobalVariable);

			// TODO: json_loads service
		}

		void InteropService::register_service(std::string method, std::function<bool(ExecutionEngine*)> handler)
		{
			_dictionary[method] = handler;
		}

		void InteropService::clear_services()
		{
			_dictionary.clear();
		}

		bool InteropService::invoke(std::string method, ExecutionEngine *engine)
		{
			if (_dictionary.find(method) == _dictionary.end()) return false;
			return _dictionary[method](engine);
		}

		bool InteropService::GetScriptContainer(ExecutionEngine *engine)
		{
			engine->evaluation_stack()->push_back(GetStackItemFromInterface(engine, (IInteropInterface*)engine->script_container()));
			return true;
		}

		bool InteropService::GetExecutingScriptHash(ExecutionEngine *engine)
		{
			engine->evaluation_stack()->push_back(StackItem::to_stack_item(engine, engine->current_context()->script_id()));
			return true;
		}

		bool InteropService::GetCallingScriptHash(ExecutionEngine *engine)
		{
			engine->evaluation_stack()->push_back(StackItem::to_stack_item(engine, engine->calling_context()->script_id()));
			return true;
		}

		bool InteropService::GetEntryScriptHash(ExecutionEngine *engine)
		{
			engine->evaluation_stack()->push_back(StackItem::to_stack_item(engine, engine->entry_context()->script_id()));
			return true;
		}

		bool InteropService::SetGlobalVariable(ExecutionEngine *engine)
		{
			auto value = engine->evaluation_stack()->pop();
			auto name = engine->evaluation_stack()->pop();
			if (name->type() != StackItemType::SIT_BYTE_ARRAY)
			{
				throw NeoVmException("global variable name must be string");
			}
			auto name_str = name->GetString();
			if (name_str.empty())
			{
				throw NeoVmException("global env key can't be empty string");
			}
			if (name_str.size() > GLOBAL_ENV_KEY_MAX_LENGTH) {
				throw NeoVmException("too long string for global env key");
			}
			engine->register_global_variable(name_str, value);
			return true;
		}

		bool InteropService::GetGlobalVariable(ExecutionEngine *engine)
		{
			auto name = engine->evaluation_stack()->pop();
			auto name_str = name->GetString();
			if (name_str.empty())
			{
				throw NeoVmException("global env key can't be empty string");
			}
			if (name_str.size() > GLOBAL_ENV_KEY_MAX_LENGTH) {
				throw NeoVmException("too long string for global env key");
			}
			auto value = engine->get_global_variable_value(name_str);
			if (value != nullptr)
			{
				engine->evaluation_stack()->push(value);
			}
			else
			{
				// push false value as nil value
				auto value = StackItem::to_stack_item_from_bool(engine, false);
				engine->evaluation_stack()->push(value);
			}
			return true;
		}

	}
}