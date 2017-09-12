#include <neovm/types.hpp>
#include <neovm/execution_engine.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{
		std::vector<char> Boolean::TRUE = { 1 };
		std::vector<char> Boolean::FALSE = {};

		bool Array::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			Array *a = (Array*)other;
			if (a == nullptr)
				return false;
			else
				return Helper::sequence_equal(_array, a->_array);
		}

		std::vector<StackItem*>* Array::GetArray()
		{
			return &_array;
		}

		VMBigInteger Array::GetBigInteger()
		{
			throw NeoVmException("not supported operation");
		}

		bool Array::GetBoolean()
		{
			return _array.size() > 0;
		}

		std::vector<char> Array::GetByteArray()
		{
			throw NeoVmException("not supported operation");
		}

		bool Boolean::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			auto b = (Boolean*)other;
			if (b == nullptr)
				return GetByteArray() == other->GetByteArray();
			else
				return _value == b->_value;
		}

		VMBigInteger Boolean::GetBigInteger()
		{
			return _value ? 1 : 0;
		}

		bool Boolean::GetBoolean()
		{
			return _value;
		}

		std::string Boolean::GetString()
		{
			return _value ? "true" : "false";
		}

		std::vector<char> Boolean::GetByteArray()
		{
			return _value ? TRUE : FALSE;
		}

		bool ByteArray::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			return _value == other->GetByteArray();
		}

		std::vector<char> ByteArray::GetByteArray()
		{
			return _value;
		}

		bool Integer::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			Integer *i = (Integer*)other;
			if (i == nullptr)
				return GetByteArray() == other->GetByteArray();
			else
				return _value == i->_value;
		}

		VMBigInteger Integer::GetBigInteger()
		{
			return _value;
		}

		bool Integer::GetBoolean()
		{
			return _value != 0;
		}

		std::string Integer::GetString()
		{
			return std::to_string(_value);
		}

		std::vector<char> Integer::GetByteArray()
		{
			return Helper::big_integer_to_chars(_value);
		}

		bool Struct::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			Struct *a = (Struct*)other;
			if (a == nullptr)
				return false;
			else
				return Helper::sequence_equal(_array, a->_array);
		}

		StackItem * Struct::Clone(ExecutionEngine *engine)
		{
			std::vector<StackItem*> newArray;
			for (size_t i = 0; i < _array.size(); i++)
			{
				if (_array[i]->IsStruct())
				{
					newArray[i] = ((Struct*)_array[i])->Clone(engine);
				}
				else
				{
					newArray[i] = _array[i]; //array = 是引用
					//其他的由于是固定值类型，不会改内部值，所以虽然需要复制，直接= 就行
				}
			}
			return new Struct(engine, newArray);
		}

		bool Userdata::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			auto b = (Userdata*)other;
			return _value == b->_value;
		}

		VMBigInteger Userdata::GetBigInteger()
		{
			throw NeoVmException("Can't parse userdata to integer");
		}

		void* Userdata::get_userdata_address()
		{
			return _value;
		}

		bool Userdata::GetBoolean()
		{
			return 1;
		}

		std::string Userdata::GetString()
		{
			return "<userdata>";
		}

		std::vector<char> Userdata::GetByteArray()
		{
			std::string address_str("<userdata>"); // 为了使得每次计算结果一致，用统一的值
			std::vector<char> addr_chars(address_str.length() + 1);
			memcpy(addr_chars.data(), address_str.c_str(), sizeof(char) * (address_str.length() + 1));
			addr_chars[address_str.length()] = '\0';
			return addr_chars;
		}

		Array::Array(ExecutionEngine *engine, std::vector<StackItem*> value)
		{
			this->_array = value;
			_type = StackItemType::SIT_ARRAY;
			engine->add_stack_item_to_pool(this);
		}

		Boolean::Boolean(ExecutionEngine *engine, bool value)
		{
			this->_value = value;
			_type = StackItemType::SIT_BOOLEAN;
			engine->add_stack_item_to_pool(this);
		}

		Userdata::Userdata(ExecutionEngine *engine, void *value)
		{
			this->_value = value;
			_type = StackItemType::SIT_USERDATA;
			engine->add_stack_item_to_pool(this);
		}

		ByteArray::ByteArray(ExecutionEngine *engine, std::vector<char> value)
		{
			this->_value = value;
			_type = StackItemType::SIT_BYTE_ARRAY;
			engine->add_stack_item_to_pool(this);
		}

		Integer::Integer(ExecutionEngine *engine, VMBigInteger value)
		{
			this->_value = value;
			_type = StackItemType::SIT_INTEGER;
			engine->add_stack_item_to_pool(this);
		}

		Struct::Struct(ExecutionEngine *engine, std::vector<StackItem*> value) : Array(engine, value)
		{
			_type = StackItemType::SIT_STRUCT;
			engine->add_stack_item_to_pool(this);
		}

	}
}