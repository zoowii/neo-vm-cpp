#ifndef NEOVM_ICRYPTO_HPP
#define NEOVM_ICRYPTO_HPP
#include <vector>

namespace neo
{
	namespace vm
	{
		class ICrypto
		{
		public:
			virtual std::vector<char> Hash160(std::vector<char> message) = 0;

			virtual std::vector<char> Hash256(std::vector<char> message) = 0;

			virtual bool VerifySignature(std::vector<char> message, std::vector<char> signature, std::vector<char> pubkey) = 0;
		};
	}
}

#endif
