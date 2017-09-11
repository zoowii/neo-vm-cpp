#ifndef NEOVM_VM_STATE_HPP
#define NEOVM_VM_STATE_HPP

namespace neo
{
	namespace vm
	{
		// 1 byte
		enum VMState
		{
			NONE = 0,

			HALT = 1 << 0,
			FAULT = 1 << 1,
			BREAK = 1 << 2,
		};
	}
}

#endif
