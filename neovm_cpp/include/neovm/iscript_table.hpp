#ifndef NEOVM_ISCRIPT_TABLE_HPP
#define NEOVM_ISCRIPT_TABLE_HPP

#include <vector>
#include <memory>

namespace neo
{
	namespace vm
	{
		class IScriptTable
		{
		public:
			virtual std::vector<char> get_script(std::string script_id) = 0;
		};
	}
}

#endif
