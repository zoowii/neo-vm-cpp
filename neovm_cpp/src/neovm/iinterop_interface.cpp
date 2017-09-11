#include <neovm/iinterop_interface.hpp>
#include <neovm/execution_engine.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{
		InteropInterface::InteropInterface(ExecutionEngine *engine, IInteropInterface *value)
		{
			this->_object = value;
			engine->add_stack_item_to_pool(this);
		}

		bool InteropInterface::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			auto i = (InteropInterface*)other;
			if (i == nullptr) return false;
			return _object->Equals((StackItem*)(i->_object));
		}

		bool InteropInterface::GetBoolean()
		{
			return _object != nullptr;
		}

		std::vector<char> InteropInterface::GetByteArray()
		{
			throw NeoVmException("not supported operation");
		}
	}
}