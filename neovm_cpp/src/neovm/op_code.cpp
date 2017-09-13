#include <neovm/op_code.hpp>

namespace neo
{
	namespace vm
	{
#define opcode_name_pair(code) {code, #code}

		std::map<OpCode, std::string> opcode_names_map = {
			// Constants
			opcode_name_pair(OP_PUSH0),
			opcode_name_pair(OP_PUSHF),
			opcode_name_pair(OP_PUSHBYTES1),
			opcode_name_pair(OP_PUSHBYTES75),
			opcode_name_pair(OP_PUSHDATA1),
			opcode_name_pair(OP_PUSHDATA2),
			opcode_name_pair(OP_PUSHDATA4),
			opcode_name_pair(OP_PUSHM1),
			opcode_name_pair(OP_PUSH1),
			opcode_name_pair(OP_PUSHT),
			opcode_name_pair(OP_PUSH2),
			opcode_name_pair(OP_PUSH3),
			opcode_name_pair(OP_PUSH4),
			opcode_name_pair(OP_PUSH5),
			opcode_name_pair(OP_PUSH6),
			opcode_name_pair(OP_PUSH7),
			opcode_name_pair(OP_PUSH8),
			opcode_name_pair(OP_PUSH9),
			opcode_name_pair(OP_PUSH10),
			opcode_name_pair(OP_PUSH11),
			opcode_name_pair(OP_PUSH12),
			opcode_name_pair(OP_PUSH13),
			opcode_name_pair(OP_PUSH14),
			opcode_name_pair(OP_PUSH15),
			opcode_name_pair(OP_PUSH16),


			// Flow control
			opcode_name_pair(OP_NOP),
			opcode_name_pair(OP_JMP),
			opcode_name_pair(OP_JMPIF),
			opcode_name_pair(OP_JMPIFNOT),
			opcode_name_pair(OP_CALL),
			opcode_name_pair(OP_RET),
			opcode_name_pair(OP_APPCALL),
			opcode_name_pair(OP_SYSCALL),
			opcode_name_pair(OP_TAILCALL),


			// Stack
			opcode_name_pair(OP_DUPFROMALTSTACK),
			opcode_name_pair(OP_TOALTSTACK),
			opcode_name_pair(OP_FROMALTSTACK),
			opcode_name_pair(OP_XDROP),
			opcode_name_pair(OP_XSWAP),
			opcode_name_pair(OP_XTUCK),
			opcode_name_pair(OP_DEPTH),
			opcode_name_pair(OP_DROP),
			opcode_name_pair(OP_DUP),
			opcode_name_pair(OP_NIP),
			opcode_name_pair(OP_OVER),
			opcode_name_pair(OP_PICK),
			opcode_name_pair(OP_ROLL),
			opcode_name_pair(OP_ROT),
			opcode_name_pair(OP_SWAP),
			opcode_name_pair(OP_TUCK),


			// Splice
			opcode_name_pair(OP_CAT),
			opcode_name_pair(OP_SUBSTR),
			opcode_name_pair(OP_LEFT),
			opcode_name_pair(OP_RIGHT),
			opcode_name_pair(OP_SIZE),


			// Bitwise logic
			opcode_name_pair(OP_INVERT),
			opcode_name_pair(BIT_AND),
			opcode_name_pair(BIT_OR),
			opcode_name_pair(BIT_XOR),
			opcode_name_pair(OP_EQUAL),
			//OP_EQUALVERIFY = 0x88, // Same as OP_EQUAL, but runs OP_VERIFY afterward.
			//OP_RESERVED1 = 0x89, // Transaction is invalid unless occuring in an unexecuted OP_IF branch
			//OP_RESERVED2 = 0x8A, // Transaction is invalid unless occuring in an unexecuted OP_IF branch

			// Arithmetic
			// Note: Arithmetic inputs are limited to signed 32-bit integers, but may overflow their output.
			opcode_name_pair(OP_INC),
			opcode_name_pair(OP_DEC),
			opcode_name_pair(OP_SIGN),
			opcode_name_pair(OP_NEGATE),
			opcode_name_pair(OP_ABS),
			opcode_name_pair(OP_NOT),
			opcode_name_pair(OP_NZ),
			opcode_name_pair(OP_ADD),
			opcode_name_pair(OP_SUB),
			opcode_name_pair(OP_MUL),
			opcode_name_pair(OP_DIV),
			opcode_name_pair(OP_MOD),
			opcode_name_pair(OP_SHL),
			opcode_name_pair(OP_SHR),
			opcode_name_pair(OP_BOOLAND),
			opcode_name_pair(OP_BOOLOR),
			opcode_name_pair(OP_NUMEQUAL),
			opcode_name_pair(OP_NUMNOTEQUAL),
			opcode_name_pair(OP_LT),
			opcode_name_pair(OP_GT),
			opcode_name_pair(OP_LTE),
			opcode_name_pair(OP_GTE),
			opcode_name_pair(OP_MIN),
			opcode_name_pair(OP_MAX),
			opcode_name_pair(OP_WITHIN),

			// Crypto
			//RIPEMD160 = 0xA6, // The input is hashed using RIPEMD-160.
			opcode_name_pair(OP_SHA1),
			opcode_name_pair(OP_SHA256),
			opcode_name_pair(OP_HASH160),
			opcode_name_pair(OP_HASH256),
			opcode_name_pair(OP_CHECKSIG),
			opcode_name_pair(OP_CHECKMULTISIG),


			// Array
			opcode_name_pair(OP_ARRAYSIZE),
			opcode_name_pair(OP_PACK),
			opcode_name_pair(OP_UNPACK),
			opcode_name_pair(OP_PICKITEM),
			opcode_name_pair(OP_SETITEM),
			opcode_name_pair(OP_NEWARRAY),
			opcode_name_pair(OP_NEWSTRUCT),

			// Exceptions
			opcode_name_pair(OP_THROW),
			opcode_name_pair(OP_THROWIFNOT)
		};

		std::string op_code_to_str(OpCode opcode)
		{
			if (opcode_names_map.find(opcode) == opcode_names_map.end())
			{
				return std::to_string(opcode);
			}
			return opcode_names_map[opcode];
		}
	}
}