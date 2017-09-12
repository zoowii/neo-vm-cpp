#ifndef NEOVM_HELPER_HPP
#define NEOVM_HELPER_HPP

#include "config.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>
#include <type_traits>
#include <neovm/stack_item.hpp>
#include <neovm/random_access_stack.hpp>

namespace neo
{
	namespace vm
	{

		class BinaryReader
		{
		private:
			std::vector<char> _data;
			size_t _position;
		public:
			static const size_t BEGIN = 0;

			BinaryReader(std::vector<char> data);

			size_t position();

			std::vector<char> ReadBytes(size_t size);

			std::vector<byte> ReadUBytes(size_t size);

			byte ReadByte();

			VMLInteger ReadVarInt64(VMLInteger max);

			uint16_t ReadUInt16();

			int16_t ReadInt16();

			uint32_t ReadUInt32();

			int32_t ReadInt32();

			uint64_t ReadUInt64();

			VMLInteger ReadVarInt(unsigned long max = -1);

			void Seek(int offset, int begin);

		};

		class Helper
		{
		public:
			static std::vector<char> ReadVarBytes(BinaryReader *reader, int max = 0X7fffffc7);

			static unsigned long long ReadVarInt(BinaryReader *reader, unsigned long long max = -1);

			static std::string bytes_to_string(std::vector<char> bytes);

			static std::vector<char> string_content_to_chars(std::string str);

			static std::vector<byte> string_content_to_bytes(std::string str);

			static std::vector<byte> string_to_bytes(std::string str);

			static std::vector<byte> chars_to_bytes(std::vector<char> data);

			static std::string ReadVarString(BinaryReader *reader);

			template <char>
			static bool sequence_equal(std::vector<char> &a, std::vector<char> &b)
			{
				if (a.size() != b.size())
					return false;
				for (size_t i = 0; i < a.size(); i++)
				{
					const auto &item_a = a[i];
					const auto &item_b = b[i];
					if (item_a != item_b)
						return false;
				}
				return true;
			}

			template <typename T>
			static bool sequence_equal(std::vector<T> &a, std::vector<T> &b)
			{
				if (a.size() != b.size())
					return false;
				for (size_t i = 0; i < a.size();i++)
				{
					const auto item_a = a[i];
					const auto item_b = b[i];
					if (item_a == item_b)
						continue;
					if (!item_b)
						return false;
					if (!item_a->Equals(item_b))
						return false;
				}
				return true;
			}

			static bool enum_has_flag(int enum_value, int flag);

			static std::vector<char> big_integer_to_chars(VMBigInteger num);

			static std::vector<byte> big_integer_to_bytes(VMBigInteger num);

			static std::vector<byte> int16_to_bytes(int16_t num);

			static std::vector<byte> int32_to_bytes(int32_t num);

			template <typename T>
			static T peek(std::vector<T> &col, size_t index=0)
			{
				if (col.size() < 1)
					throw NeoVmException("peek from empty collection error");
				if (index >= col.size())
					throw NeoVmException("peek out of range error");
				return col[col.size() -index - 1];
			}

			template <typename T>
			static T peek(RandomAccessStack<T> &col, size_t index = 0)
			{
				return col.peek(index);
			}

			template <typename T>
			static T pop(std::vector<T> &col)
			{
				if (col.size() < 1)
					throw NeoVmException("pop from empty collection error");
				auto item = col[col.size() - 1];
				col.pop_back();
				return item;
			}

			template <typename T>
			static T pop(RandomAccessStack<T> &col)
			{
				if (col.size() < 1)
					throw NeoVmException("pop from empty collection error");
				auto item = col.peek();
				col.pop();
				return item;
			}
			
			template <typename T>
			static void insert(std::vector<T> &col, size_t index, T &value)
			{
				if (index > col.size())
					throw NeoVmException("too large position to insert");
				col.push_back(value);
				if (index == col.size() - 1)
					return;
				for (size_t i = col.size() - 1; i >= index + 1;i--)
				{
					col[i] = col[i - 1];
				}
				col[index] = value;
			}

			template <typename T>
			static std::vector<T>concat_vector(std::vector<T> &a, std::vector<T> &b)
			{
				std::vector<T> result(a.size() + b.size());
				memcpy(result.data(), a.data(), sizeof(T) * a.size());
				memcpy(result.data() + a.size(), b.data(), sizeof(T) * b.size());
				return result;
			}

		};
	}
}

#endif
