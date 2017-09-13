#include <neovm/types.hpp>
#include <neovm/execution_engine.hpp>
#include <neovm/exceptions.hpp>
#include <neovm/helper.hpp>
#include <sstream>

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
			{
				return false;
			}
			else
			{
				return Helper::sequence_equal(_array, a->_array);
			}
		}

		std::vector<StackItem*>* Array::GetArray()
		{
			return &_array;
		}

		VMBigInteger Array::GetBigInteger() const
		{
			throw NeoVmException("not supported operation");
		}

		bool Array::GetBoolean() const
		{
			return _array.size() > 0;
		}

		std::string Array::GetString() const
		{
			return to_json_string({});
		}

		std::string Array::to_json_string(std::set<void*> referenced_objects) const
		{
			referenced_objects.insert((void*)this);
			if (referenced_objects.size() > JSON_MAX_REFERENCE_OBJECT_COUNT)
				throw NeoVmException("too many objects referenced in json");
			std::stringstream ss;
			ss << "[";
			for (size_t i = 0; i < _array.size(); i++)
			{
				if (i > 0)
					ss << ",";
				ss << _array[i]->to_json_string(referenced_objects);
			}
			ss << "]";
			return ss.str();
		}

		std::vector<char> Array::GetByteArray() const
		{
			throw NeoVmException("not supported operation");
		}

		bool Map::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			Map *a = (Map*)other;
			if (a == nullptr)
			{
				return false;
			}
			else
			{
				if (this->_items.size() != a->_items.size())
					return false;
				for (size_t i = 0; i < this->_items.size(); i++)
				{
					auto &pair1 = this->_items[i];
					auto found = false;
					for (size_t j = 0; j < a->_items.size(); j++)
					{
						auto &pair2 = a->_items[j];
						if (pair1.first->Equals(pair2.first))
						{
							found = true;
							if (!pair1.second->Equals(pair2.second))
								return false;
						}
					}
					if (!found)
						return false;
				}
				return true;
			}
		}

		void Map::put(StackItem *key, StackItem *value)
		{
			if (!key || key->type() != StackItemType::SIT_BYTE_ARRAY)
				throw NeoVmException("Map key must be string");
			if (!value)
				throw NeoVmException("Map value can't be null");
			for (size_t i = 0; i < _items.size(); i++)
			{
				auto &pair = _items[i];
				if (pair.first->Equals(key))
				{
					pair.second = value;
					return;
				}
			}
			_items.push_back(std::make_pair(key, value));
		}

		StackItem* Map::get(StackItem *key)
		{
			if (!key || key->type() != StackItemType::SIT_BYTE_ARRAY)
				throw NeoVmException("Map key must be string");
			for (size_t i = 0; i < _items.size(); i++)
			{
				auto &pair = _items[i];
				if (pair.first->Equals(key))
				{
					return pair.second;
				}
			}
			return nullptr;
		}

		std::vector<StackItem*> Map::keys() const
		{
			std::vector<StackItem*> key_items;
			for (size_t i = 0; i < _items.size(); i++)
			{
				auto &pair = _items[i];
				key_items.push_back(pair.first);
			}
			return key_items;
		}

		VMBigInteger Map::GetBigInteger() const
		{
			throw NeoVmException("not supported operation");
		}

		bool Map::GetBoolean() const
		{
			return true;
		}

		std::string Map::GetString() const
		{
			return to_json_string({});
		}

		std::string Map::to_json_string(std::set<void*> referenced_objects) const
		{
			referenced_objects.insert((void*)this);
			if (referenced_objects.size() > JSON_MAX_REFERENCE_OBJECT_COUNT)
				throw NeoVmException("too many objects referenced in json");
			std::stringstream ss;
			ss << "{";
			for (size_t i = 0; i < _items.size(); i++)
			{
				const auto &pair = _items[i];
				if (i > 0)
					ss << ",";
				ss << pair.first->to_json_string(referenced_objects) << ":" << pair.second->to_json_string(referenced_objects);
			}
			ss << "}";
			return ss.str();
		}

		std::vector<char> Map::GetByteArray() const
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

		VMBigInteger Boolean::GetBigInteger() const
		{
			return _value ? 1 : 0;
		}

		bool Boolean::GetBoolean() const
		{
			return _value;
		}

		std::string Boolean::GetString() const
		{
			return _value ? "true" : "false";
		}

		std::string Boolean::to_json_string(std::set<void*> referenced_objects) const
		{
			return GetString();
		}

		std::vector<char> Boolean::GetByteArray() const
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

		std::vector<char> ByteArray::GetByteArray() const
		{
			return _value;
		}

		std::string ByteArray::GetString() const
		{
			std::vector<char> str_content(_value.size() + 1);
			memcpy(str_content.data(), _value.data(), sizeof(char) * _value.size());
			str_content[_value.size()] = '\0';
			std::string str(str_content.data());
			return str;
		}

		std::string ByteArray::to_json_string(std::set<void*> referenced_objects) const
		{
			return Helper::quote_string(GetString());
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

		VMBigInteger Integer::GetBigInteger() const
		{
			return _value;
		}

		bool Integer::GetBoolean() const
		{
			return _value != 0;
		}

		std::string Integer::GetString() const
		{
			return std::to_string(_value);
		}

		std::string Integer::to_json_string(std::set<void*> referenced_objects) const
		{
			return GetString();
		}

		std::vector<char> Integer::GetByteArray() const
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

		std::string Struct::GetString() const
		{
			return to_json_string({});
		}

		std::string Struct::to_json_string(std::set<void*> referenced_objects) const
		{
			referenced_objects.insert((void*)this);
			if (referenced_objects.size() > JSON_MAX_REFERENCE_OBJECT_COUNT)
				throw NeoVmException("too many referenced object in json");
			std::stringstream ss;
			ss << "[";
			for (size_t i = 0; i < _array.size(); i++)
			{
				if (i > 0)
					ss << ",";
				ss << _array[i]->to_json_string(referenced_objects);
			}
			ss << "]";
			return ss.str();
		}

		bool Userdata::Equals(StackItem *other)
		{
			if (this == other) return true;
			if (nullptr == other) return false;
			if (other->type() != this->type()) return false;
			auto b = (Userdata*)other;
			return _value == b->_value;
		}

		VMBigInteger Userdata::GetBigInteger() const
		{
			throw NeoVmException("Can't parse userdata to integer");
		}

		void* Userdata::get_userdata_address() const
		{
			return _value;
		}

		bool Userdata::GetBoolean() const
		{
			return 1;
		}

		std::string Userdata::GetString() const
		{
			return "<userdata>";
		}

		std::string Userdata::to_json_string(std::set<void*> referenced_objects) const
		{
			return GetString();
		}

		std::vector<char> Userdata::GetByteArray() const
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

		Map::Map(ExecutionEngine *engine, std::vector<std::pair<StackItem*, StackItem*>> items)
		{
			this->_type = StackItemType::SIT_MAP;
			for (const auto &pair : items)
			{
				put(pair.first, pair.second);
			}
			engine->add_stack_item_to_pool(this);
		}

	}
}