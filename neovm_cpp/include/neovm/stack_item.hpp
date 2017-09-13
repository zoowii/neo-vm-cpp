#ifndef NEOVM_STACK_ITEM_HPP
#define NEOVM_STACK_ITEM_HPP

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <neovm/config.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{
		class IInteropInterface;
		class ExecutionEngine;

		enum StackItemType
		{
			SIT_INTEGER = 0,
			SIT_BOOLEAN = 1,
			SIT_BYTE_ARRAY = 2,
			SIT_ARRAY = 3,
			SIT_STRUCT = 4,
			SIT_MAP = 5,

			SIT_USERDATA = 8,

			SIT_INTEROP_INTERFACE = 10
		};

		class StackItem
		{
		protected:
			StackItemType _type;
		public:

			inline virtual bool IsArray() const { return false; }
			inline virtual bool IsStruct() const { return false; }
			inline virtual bool IsUserdata() const { return false; }
			inline virtual bool is_map() const { return false; }

			virtual bool Equals(StackItem *other) = 0;

			inline virtual std::vector<StackItem*> *GetArray()
			{
				throw NeoVmException("not supported operation");
			}

			inline virtual StackItemType type() const
			{
				return _type;
			}

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual std::vector<char> GetByteArray() const = 0;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual IInteropInterface *GetInterface();
			

			static StackItem *to_stack_item(ExecutionEngine *engine, std::vector<char> bytes);

			static StackItem *to_stack_item(ExecutionEngine *engine, std::string str);

			static StackItem *to_stack_item_from_bool(ExecutionEngine *engine, bool value);

			static StackItem *to_stack_item(ExecutionEngine *engine, VMBigInteger num);

			static StackItem *to_stack_item(ExecutionEngine *engine, std::vector<StackItem*> &items);

			static StackItem *to_stack_struct_item(ExecutionEngine *engine, std::vector<StackItem*> &items);

			static StackItem *to_stack_userdata_item(ExecutionEngine *engine, void *userdata);

		};

		StackItem *GetStackItemFromInterface(ExecutionEngine *engine, IInteropInterface *value);

	}
}

#endif
