#ifndef NEOVM_CONFIG_HPP
#define NEOVM_CONFIG_HPP

namespace neo
{
	namespace vm
	{
		typedef long long VMBigInteger;

		typedef long long VMLInteger; // �������ʹ�õ���������

		typedef unsigned char VMByte;

		// to_json_stringʱ���֧�����õĶ�������
#define JSON_MAX_REFERENCE_OBJECT_COUNT 10240
	}
}

#endif
