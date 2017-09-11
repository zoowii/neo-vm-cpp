#include <neovm/helper.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{

		BinaryReader::BinaryReader(std::vector<char> data)
		{
			_data = data;
			_position = 0;
		}

		size_t BinaryReader::position()
		{
			return _position;
		}

		std::vector<char> BinaryReader::ReadBytes(size_t size)
		{
			if (size + _position > _data.size())
			{
				throw NeoVmException("not enough binary data to read");
			}
			std::vector<char> result(size);
			memcpy(result.data(), _data.data() + _position, size * sizeof(char));
			_position += size;
			return result;
		}

		std::vector<byte> BinaryReader::ReadUBytes(size_t size)
		{
			if (size + _position > _data.size())
			{
				throw NeoVmException("not enough binary data to read");
			}
			std::vector<byte> result(size);
			memcpy(result.data(), _data.data() + _position, size * sizeof(byte));
			_position += size;
			return result;
		}

		byte BinaryReader::ReadByte()
		{
			return ReadUBytes(1)[0];
		}

		LInteger BinaryReader::ReadVarInt64(LInteger max)
		{
			// little endien
			auto bytes = ReadUBytes(8);
			LInteger result = ((LInteger)bytes[7] << 56) + ((LInteger)bytes[6] << 48) + ((LInteger)bytes[5] << 40) + ((LInteger)bytes[4] << 32)
				+ ((LInteger)bytes[3] << 24) + ((LInteger)bytes[2] << 16) + ((LInteger)bytes[1] << 8) + (LInteger)bytes[0];
			return result;
		}

		uint16_t BinaryReader::ReadUInt16()
		{
			// little endien
			auto bytes = ReadUBytes(2);
			return ((uint16_t)bytes[1] << 8) + (uint16_t)bytes[10];
		}

		int16_t BinaryReader::ReadInt16()
		{
			// little endien
			auto bytes = ReadUBytes(2);
			return ((int16_t)bytes[1] << 8) + (int16_t)bytes[0];
		}

		uint32_t BinaryReader::ReadUInt32()
		{
			// little endien
			auto bytes = ReadUBytes(4);
			return ((uint32_t)bytes[3] << 24) + ((uint32_t)bytes[2] << 16) + ((uint32_t)bytes[1] << 8) + bytes[0];
		}

		int32_t BinaryReader::ReadInt32()
		{
			// little endien
			auto bytes = ReadUBytes(4);
			return ((int32_t)bytes[3] << 24) + ((int32_t)bytes[2] << 16) + ((int32_t)bytes[1] << 8) + (int32_t)bytes[0];
		}

		uint64_t BinaryReader::ReadUInt64()
		{
			// little endien
			auto bytes = ReadUBytes(8);
			uint64_t result = ((uint64_t)bytes[7] << 56) + ((uint64_t)bytes[6] << 48) + ((uint64_t)bytes[5] << 40)
				+ ((uint64_t)bytes[4] << 32) + ((uint64_t)bytes[3] << 24) + ((uint64_t)bytes[2] << 16) + ((uint64_t)bytes[1] << 8) + (uint64_t)bytes[0];
			return result;
		}

		LInteger BinaryReader::ReadVarInt(unsigned long max)
		{
			char fb = ReadByte();
			LInteger value;
			if (fb == 0xFD)
				value = ReadUInt16();
			else if (fb == 0xFE)
				value = ReadUInt32();
			else if (fb == 0xFF)
				value = ReadUInt64();
			else
				value = fb;
			if (value > max) throw NeoVmException("int format error");
			return value;
		}

		void BinaryReader::Seek(int offset, int begin)
		{
			_position = offset + begin;
		}

		std::vector<char> Helper::ReadVarBytes(BinaryReader *reader, int max)
		{
			return reader->ReadBytes((int) reader->ReadVarInt((unsigned long long)max));
		}

		unsigned long long Helper::ReadVarInt(BinaryReader *reader, unsigned long long max)
		{
			char fb = reader->ReadByte();
			unsigned long long value;
			if (fb == 0xFD)
				value = reader->ReadUInt16();
			else if (fb == 0xFE)
				value = reader->ReadUInt32();
			else if (fb == 0xFF)
				value = reader->ReadUInt64();
			else
				value = fb;
			if (value > max) throw NeoVmException("format exception");
			return value;
		}

		std::string Helper::bytes_to_string(std::vector<char> bytes)
		{
			std::vector<char> str_bytes(bytes.size() + 1);
			memcpy(str_bytes.data(), bytes.data(), bytes.size());
			str_bytes[bytes.size()] = '\0';
			return str_bytes.data();
		}

		std::vector<char> Helper::string_content_to_chars(std::string str)
		{
			std::vector<char> data(str.size());
			memcpy(data.data(), str.c_str(), str.size() * sizeof(char));
			return data;
		}

		std::vector<byte> Helper::string_content_to_bytes(std::string str)
		{
			std::vector<byte> data(str.size());
			memcpy(data.data(), str.c_str(), str.size() * sizeof(char));
			return data;
		}

		std::vector<byte> Helper::string_to_bytes(std::string str)
		{
			std::vector<byte> data(str.size());
			memcpy(data.data(), str.c_str(), str.size() * sizeof(char));
			std::vector<byte> result;
			if (data.size() < 0x100)
			{
				result.push_back((byte)data.size());
				for (const auto b : data)
				{
					result.push_back(b);
				}
			}
			else if (data.size() < 0x10000)
			{
				result.push_back(0xFD);
				auto int16_data_size_bytes = Helper::int16_to_bytes((uint16_t)data.size());
				for (const auto b : int16_data_size_bytes)
				{
					result.push_back(b);
				}
				for (const auto b : data)
				{
					result.push_back(b);
				}
			}
			else
			{
				result.push_back(0xFE);
				auto data_size_bytes = Helper::int32_to_bytes((uint32_t)data.size());
				for (const auto b : data_size_bytes)
				{
					result.push_back(b);
				}
				for (const auto b : data)
				{
					result.push_back(b);
				}
			}
			return result;
		}

		std::string Helper::ReadVarString(BinaryReader *reader)
		{
			auto bytes = ReadVarBytes(reader);
			std::vector<char> str_bytes(bytes.size() + 1);
			memcpy(str_bytes.data(), bytes.data(), bytes.size());
			str_bytes[bytes.size()] = '\0';
			return str_bytes.data();
		}

		bool Helper::enum_has_flag(int enum_value, int flag)
		{
			return (enum_value | flag) == enum_value;
		}

		std::vector<char> Helper::big_integer_to_chars(BigInteger num)
		{
			std::vector<char> array(8);
			for (int i = 0; i < 8; i++)
				array[7 - i] = (char) (num >> (i * 8));
			return array;
		}

		std::vector<byte> Helper::big_integer_to_bytes(BigInteger num)
		{
			std::vector<byte> array(8);
			for (int i = 0; i < 8; i++)
				array[7 - i] = (byte) (num >> (i * 8));
			return array;
		}

		std::vector<byte> Helper::int16_to_bytes(int16_t num)
		{
			std::vector<byte> array(2);
			for (int i = 0; i < 2; i++)
				array[1 - i] = (byte)(num >> (i * 8));
			return array;
		}

		std::vector<byte> Helper::int32_to_bytes(int32_t num)
		{
			std::vector<byte> array(4);
			for (int i = 0; i < 4; i++)
				array[3 - i] = (byte)(num >> (i * 8));
			return array;
		}


	}
}