#ifndef NEOVM_IINTEROP_INTERFACE_HPP
#define NEOVM_IINTEROP_INTERFACE_HPP

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

		class IInteropInterface
		{
		public:
			inline virtual bool Equals(StackItem *other)
			{
				return false;
			}
		};

		class InteropInterface : public StackItem
		{
		private:
			IInteropInterface *_object;

		public:
			InteropInterface(ExecutionEngine *engine, IInteropInterface *value);

			virtual bool Equals(StackItem *other);

			virtual  bool GetBoolean();

			virtual std::vector<char> GetByteArray();

		};

	}
}

#endif
