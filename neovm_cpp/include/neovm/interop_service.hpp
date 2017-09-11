#ifndef NEOVM_INTEROP_SERVICE
#define NEOVM_INTEROP_SERVICE

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <neovm/stack_item.hpp>

namespace neo
{
	namespace vm
	{
		class ExecutionEngine;

		class InteropService
		{
		private:
			std::map<std::string, std::function<bool(ExecutionEngine*)>> _dictionary;

		public:
			InteropService();

			void register_service(std::string method, std::function<bool(ExecutionEngine*)> handler);

			bool invoke(std::string method, ExecutionEngine *engine);

		private:
			static bool GetScriptContainer(ExecutionEngine *engine);

			static bool GetExecutingScriptHash(ExecutionEngine *engine);

			static bool GetCallingScriptHash(ExecutionEngine *engine);

			static bool GetEntryScriptHash(ExecutionEngine *engine);

			static bool SetGlobalVariable(ExecutionEngine *engine);
			
			static bool GetGlobalVariable(ExecutionEngine *engine);

		};
	}
}

#endif
