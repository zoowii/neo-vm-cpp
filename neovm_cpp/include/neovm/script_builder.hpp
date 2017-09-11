#ifndef NEOVM_SCRIPT_BUILDER_HPP
#define NEOVM_SCRIPT_BUILDER_HPP

#include <neovm/config.hpp>
#include <neovm/op_code.hpp>
#include <neovm/helper.hpp>
#include <vector>

namespace neo
{
	namespace vm
	{
		class ScriptBuilder
		{
		private:
			std::vector<byte> _ms;
			size_t _position;
		public:
			size_t Offset() const;

			ScriptBuilder();

			virtual ~ScriptBuilder();

			ScriptBuilder *emit(OpCode op, std::vector<byte> arg = {});

			ScriptBuilder *emit_app_call(std::vector<byte> scriptHash, bool useTailCall = false);

			ScriptBuilder *emit_jump(OpCode op, int16_t offset);

			ScriptBuilder *emit_push(BigInteger number);

			ScriptBuilder *emit_push(bool data);

			ScriptBuilder *emit_push(const char *str);

			ScriptBuilder *emit_push_string(std::string str);

			ScriptBuilder *emit_push(std::vector<byte> data);

			ScriptBuilder *emit_sys_call(std::string api);

			std::vector<byte> to_array();

			std::vector<char> to_char_array();
		};
	}
}

#endif
