#ifndef NEOVM_CONFIG_HPP
#define NEOVM_CONFIG_HPP

namespace neo
{
	namespace vm
	{
		typedef long long VMBigInteger;

		typedef long long VMLInteger; // 虚拟机中使用的整数类型

		typedef unsigned char VMByte;

		// to_json_string时最多支持引用的对象数量
#define JSON_MAX_REFERENCE_OBJECT_COUNT 10240

		// invocation最大深度
#define NEOVM_MAX_INVOCATION_DEPTH 1024

	}
}

#endif
