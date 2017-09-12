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

			virtual VMBigInteger GetBigInteger();

			virtual bool GetBoolean();

			virtual std::vector<char> GetByteArray();
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

			virtual VMBigInteger GetBigInteger();

			virtual bool GetBoolean();

			virtual std::string GetString();

			virtual std::vector<char> GetByteArray();
		};

		class ByteArray : public StackItem
		{
		private:
			std::vector<char> _value;

		public:
			ByteArray(ExecutionEngine *engine, std::vector<char> value);

			virtual bool Equals(StackItem *other);

			virtual std::vector<char> GetByteArray();
		};

		class Integer : public StackItem
		{
		private:
			VMBigInteger _value;

		public:
			Integer(ExecutionEngine *engine, VMBigInteger value);

			virtual bool Equals(StackItem *other);

			virtual VMBigInteger GetBigInteger();

			virtual bool GetBoolean();

			virtual std::string GetString();

			virtual std::vector<char> GetByteArray();
		};

		class Struct : public Array
		{
		public:
			inline virtual bool IsStruct() { return true; }

			Struct(ExecutionEngine *engine, std::vector<StackItem*> value);

			virtual StackItem *Clone(ExecutionEngine *engine);

			virtual bool Equals(StackItem *other);
		};

		class Userdata : public StackItem
		{
		private:
			void *_value;
		public:
			Userdata(ExecutionEngine *engine, void *value);

			virtual bool Equals(StackItem *other);

			inline virtual bool IsUserdata() const { return true; }

			virtual VMBigInteger GetBigInteger();

			virtual bool GetBoolean();

			virtual void *get_userdata_address();

			virtual std::string GetString();

			virtual std::vector<char> GetByteArray();
		};

	}
}

#endif
