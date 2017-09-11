#ifndef NEOVM_ISCRIPT_CONTAINER_HPP
#define NEOVM_ISCRIPT_CONTAINER_HPP

#include <vector>
#include <neovm/iinterop_interface.hpp>

namespace neo
{
	namespace vm
	{
		class IScriptContainer : IInteropInterface
		{
		public:
			virtual std::vector<char> get_message() = 0;
		};
	}
}

#endif
