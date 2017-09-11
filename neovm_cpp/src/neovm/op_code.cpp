#include <neovm/op_code.hpp>

namespace neo
{
	namespace vm
	{
#define opcode_name_pair(code) {code, #code}

		std::map<OpCode, std::string> opcode_names_map = {
			// Constants
			opcode_name_pair(PUSH0),
			opcode_name_pair(PUSHF),
			opcode_name_pair(PUSHBYTES1),
			opcode_name_pair(PUSHBYTES75),
			opcode_name_pair(PUSHDATA1),
			opcode_name_pair(PUSHDATA2),
			opcode_name_pair(PUSHDATA4),
			opcode_name_pair(PUSHM1),
			opcode_name_pair(PUSH1),
			opcode_name_pair(PUSHT),
			opcode_name_pair(PUSH2),
			opcode_name_pair(PUSH3),
			opcode_name_pair(PUSH4),
			opcode_name_pair(PUSH5),
			opcode_name_pair(PUSH6),
			opcode_name_pair(PUSH7),
			opcode_name_pair(PUSH8),
			opcode_name_pair(PUSH9),
			opcode_name_pair(PUSH10),
			opcode_name_pair(PUSH11),
			opcode_name_pair(PUSH12),
			opcode_name_pair(PUSH13),
			opcode_name_pair(PUSH14),
			opcode_name_pair(PUSH15),
			opcode_name_pair(PUSH16),


			// Flow control
			opcode_name_pair(NOP),
			opcode_name_pair(JMP),
			opcode_name_pair(JMPIF),
			opcode_name_pair(JMPIFNOT),
			opcode_name_pair(CALL),
			opcode_name_pair(RET),
			opcode_name_pair(APPCALL),
			opcode_name_pair(SYSCALL),
			opcode_name_pair(TAILCALL),


			// Stack
			opcode_name_pair(DUPFROMALTSTACK),
			opcode_name_pair(TOALTSTACK),
			opcode_name_pair(FROMALTSTACK),
			opcode_name_pair(XDROP),
			opcode_name_pair(XSWAP),
			opcode_name_pair(XTUCK),
			opcode_name_pair(DEPTH),
			opcode_name_pair(DROP),
			opcode_name_pair(DUP),
			opcode_name_pair(NIP),
			opcode_name_pair(OVER),
			opcode_name_pair(PICK),
			opcode_name_pair(ROLL),
			opcode_name_pair(ROT),
			opcode_name_pair(SWAP),
			opcode_name_pair(TUCK),


			// Splice
			opcode_name_pair(CAT),
			opcode_name_pair(SUBSTR),
			opcode_name_pair(LEFT),
			opcode_name_pair(RIGHT),
			opcode_name_pair(SIZE),


			// Bitwise logic
			opcode_name_pair(INVERT),
			opcode_name_pair(BIT_AND),
			opcode_name_pair(BIT_OR),
			opcode_name_pair(BIT_XOR),
			opcode_name_pair(EQUAL),
			//OP_EQUALVERIFY = 0x88, // Same as OP_EQUAL, but runs OP_VERIFY afterward.
			//OP_RESERVED1 = 0x89, // Transaction is invalid unless occuring in an unexecuted OP_IF branch
			//OP_RESERVED2 = 0x8A, // Transaction is invalid unless occuring in an unexecuted OP_IF branch

			// Arithmetic
			// Note: Arithmetic inputs are limited to signed 32-bit integers, but may overflow their output.
			opcode_name_pair(INC),
			opcode_name_pair(DEC),
			opcode_name_pair(SIGN),
			opcode_name_pair(NEGATE),
			opcode_name_pair(ABS),
			opcode_name_pair(OP_NOT),
			opcode_name_pair(NZ),
			opcode_name_pair(ADD),
			opcode_name_pair(SUB),
			opcode_name_pair(MUL),
			opcode_name_pair(DIV),
			opcode_name_pair(MOD),
			opcode_name_pair(SHL),
			opcode_name_pair(SHR),
			opcode_name_pair(BOOLAND),
			opcode_name_pair(BOOLOR),
			opcode_name_pair(NUMEQUAL),
			opcode_name_pair(NUMNOTEQUAL),
			opcode_name_pair(LT),
			opcode_name_pair(GT),
			opcode_name_pair(LTE),
			opcode_name_pair(GTE),
			opcode_name_pair(MIN),
			opcode_name_pair(MAX),
			opcode_name_pair(WITHIN),

			// Crypto
			//RIPEMD160 = 0xA6, // The input is hashed using RIPEMD-160.
			opcode_name_pair(SHA1),
			opcode_name_pair(SHA256),
			opcode_name_pair(HASH160),
			opcode_name_pair(HASH256),
			opcode_name_pair(CHECKSIG),
			opcode_name_pair(CHECKMULTISIG),


			// Array
			opcode_name_pair(ARRAYSIZE),
			opcode_name_pair(PACK),
			opcode_name_pair(UNPACK),
			opcode_name_pair(PICKITEM),
			opcode_name_pair(SETITEM),
			opcode_name_pair(NEWARRAY),
			opcode_name_pair(NEWSTRUCT),

			// Exceptions
			opcode_name_pair(THROW),
			opcode_name_pair(THROWIFNOT)
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