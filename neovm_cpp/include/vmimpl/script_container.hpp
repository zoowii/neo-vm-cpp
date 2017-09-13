#ifndef NEOVM_SCRIPT_CONTAINER_HPP
#define NEOVM_SCRIPT_CONTAINER_HPP

#include <neovm/iscript_container.hpp>
#include <neovm/op_code.hpp>

namespace neo
{
	namespace vm
	{
		namespace impl
		{
			class DemoScriptContainer : public IScriptContainer
			{
			public:
				inline virtual std::vector<char> get_message()
				{
					std::vector<char> data;
					data.push_back(OpCode::OP_PUSH0);
					data.push_back(OpCode::OP_PUSH1);
					
					// FIXME: 用生成的demo字节码填充

					return data;
				}
			};
		}
	}
}

#endif
