#include <neovm/stack_item.hpp>
#include <neovm/iinterop_interface.hpp>
#include <neovm/types.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{
		BigInteger StackItem::GetBigInteger()
		{
			// big endien
			auto bytes = GetByteArray();
			if (bytes.size() > 8)
			{
				throw NeoVmException("too long bytes to parse to BigInteger");
			}
			BigInteger value = 0;
			for (size_t i = 0; i < bytes.size(); i++)
			{
				value += (byte)(bytes[i]) << (8 * (bytes.size() - i - 1));
			}
			return value;
		}

		bool StackItem::GetBoolean()
		{
			const auto &data = GetByteArray();
			if (data.size() < 1)
				return false;
			for (const char item : data)
			{
				if (item != 0)
					return true;
			}
			return false;
		}

		std::string StackItem::GetString()
		{
			return Helper::bytes_to_string(GetByteArray());
		}

		IInteropInterface* StackItem::GetInterface()
		{
			throw NeoVmException("not supported operation");
		}

		StackItem *GetStackItemFromInterface(ExecutionEngine *engine, IInteropInterface *value)
		{
			return new InteropInterface(engine, value);
		}

		StackItem *StackItem::to_stack_item(ExecutionEngine *engine, std::vector<char> bytes)
		{
			return new ByteArray(engine, bytes);
		}

		StackItem *StackItem::to_stack_item(ExecutionEngine *engine, std::string str)
		{
			std::vector<char> bytes(str.size());
			memcpy(bytes.data(), str.c_str(), sizeof(char) * str.size());
			return to_stack_item(engine, bytes);
		}

		StackItem* StackItem::to_stack_item_from_bool(ExecutionEngine *engine, bool value)
		{
			return new Boolean(engine, value);
		}

		StackItem *StackItem::to_stack_item(ExecutionEngine *engine, BigInteger num)
		{
			return new Integer(engine, num);
		}

		StackItem *StackItem::to_stack_item(ExecutionEngine *engine, std::vector<StackItem*> &items)
		{
			return new Array(engine, items);
		}

		StackItem *StackItem::to_stack_struct_item(ExecutionEngine *engine, std::vector<StackItem*> &items)
		{
			return new Struct(engine, items);
		}

		StackItem *StackItem::to_stack_userdata_item(ExecutionEngine *engine, void *userdata)
		{
			return new Userdata(engine, userdata);
		}

	}
}