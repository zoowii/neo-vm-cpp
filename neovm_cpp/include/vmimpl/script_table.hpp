#ifndef NEOVM_SCRIPT_TABLE_HPP
#define NEOVM_SCRIPT_TABLE_HPP

#include <neovm/iscript_table.hpp>
#include <neovm/op_code.hpp>
#include <neovm/exceptions.hpp>
#include <map>

namespace neo
{
	namespace vm
	{
		namespace impl
		{
			class DemoScriptTable : public IScriptTable
			{
			private:
				std::map<std::string, std::vector<char>> _cached_scripts;
			public:
				virtual std::vector<char> get_script(std::string script_id)
				{
					if (_cached_scripts.find(script_id) == _cached_scripts.end())
						throw NeoVmException(("can't find script " + script_id).c_str());
					return _cached_scripts[script_id];
				}

				void put_script(std::string script_id, std::vector<char> &script)
				{
					_cached_scripts[script_id] = script;
				}
			};
		}
	}
}

#endif
