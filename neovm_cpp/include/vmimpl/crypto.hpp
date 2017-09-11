#ifndef NEOVM_CRYPTO_HPP
#define NEOVM_CRYPTO_HPP
#include "neovm/icrypto.hpp"

namespace neo
{
	namespace vm
	{
		namespace impl
		{
			class DemoCrypto : public ICrypto
			{
			public:
				virtual std::vector<char> Hash160(std::vector<char> message)
				{
					std::vector<char> data(10);
					memcpy(data.data(), "hashed160", 10);
					return data;
				}

				virtual std::vector<char> Hash256(std::vector<char> message)
				{
					std::vector<char> data(10);
					memcpy(data.data(), "hashed256", 10);
					return data;
				}

				virtual bool VerifySignature(std::vector<char> message, std::vector<char> signature, std::vector<char> pubkey)
				{
					return true;
				}
			};
		}
	}
}

#endif
