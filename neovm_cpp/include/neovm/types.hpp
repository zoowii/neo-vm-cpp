#ifndef NEOVM_TYPES_HPP
#define NEOVM_TYPES_HPP

#include <neovm/stack_item.hpp>
#include <neovm/helper.hpp>
#include <vector>

namespace neo
{
	namespace vm
	{
		class ExecutionEngine;

		class Array : public StackItem
		{
		protected:
			std::vector<StackItem*> _array;

		public:
			inline virtual bool IsArray() const { return true; }

			Array(ExecutionEngine *engine, std::vector<StackItem*> value);

			virtual bool Equals(StackItem *other);

			virtual std::vector<StackItem*> *GetArray();

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual std::vector<char> GetByteArray() const;
		};



		class Map : public StackItem
		{
		protected:
			std::vector<std::pair<StackItem*, StackItem*>> _items;
		public:
			inline virtual bool is_map() const { return true; }
			
			Map(ExecutionEngine *engine, std::vector<std::pair<StackItem*, StackItem*>> items);

			virtual bool Equals(StackItem *other);

			virtual void put(StackItem *key, StackItem *value);

			virtual StackItem* get(StackItem *key);

			virtual std::vector<StackItem*> keys() const;

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual std::vector<char> GetByteArray() const;
		};

		class Boolean : public StackItem
		{
		private:
			static std::vector<char> TRUE;
			static std::vector<char> FALSE;

			bool _value;

		public:
			Boolean(ExecutionEngine *engine, bool value);

			virtual bool Equals(StackItem *other);

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual std::vector<char> GetByteArray() const;
		};

		class ByteArray : public StackItem
		{
		private:
			std::vector<char> _value;

		public:
			ByteArray(ExecutionEngine *engine, std::vector<char> value);

			virtual bool Equals(StackItem *other);

			virtual std::vector<char> GetByteArray() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;
		};

		class Integer : public StackItem
		{
		private:
			VMBigInteger _value;

		public:
			Integer(ExecutionEngine *engine, VMBigInteger value);

			virtual bool Equals(StackItem *other);

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual std::vector<char> GetByteArray() const;
		};

		class Struct : public Array
		{
		public:
			inline virtual bool IsStruct() { return true; }

			Struct(ExecutionEngine *engine, std::vector<StackItem*> value);

			virtual StackItem *Clone(ExecutionEngine *engine);

			virtual bool Equals(StackItem *other);

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;
		};

		class Userdata : public StackItem
		{
		private:
			void *_value;
		public:
			Userdata(ExecutionEngine *engine, void *value);

			virtual bool Equals(StackItem *other);

			inline virtual bool IsUserdata() const { return true; }

			virtual VMBigInteger GetBigInteger() const;

			virtual bool GetBoolean() const;

			virtual void *get_userdata_address() const;

			virtual std::string GetString() const;

			virtual std::string to_json_string(std::set<void*> referenced_objects) const;

			virtual std::vector<char> GetByteArray() const;
		};

	}
}

#endif
