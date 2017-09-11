#include <iostream>
#include <fstream>
#include <neovm/execution_context.hpp>
#include <neovm/execution_engine.hpp>
#include <vmimpl/script_container.hpp>
#include <vmimpl/crypto.hpp>
#include <vmimpl/script_table.hpp>
#include <neovm/types.hpp>
#include <neovm/script_builder.hpp>

using namespace neo::vm;
using namespace neo::utils;


bool GetStorageContext(neo::vm::ExecutionEngine *engine)
{
	// TODO: ��storage context��userdata����ѹջ�������ͷź�������pre_close_callback
	auto storage_context = new std::string("TODO");
	engine->add_pre_close_callbacks([storage_context](neo::vm::ExecutionEngine *engine) {
		std::cout << "freeed storage context" << std::endl;
		delete storage_context;
	});
	engine->evaluation_stack()->push_back(neo::vm::StackItem::to_stack_userdata_item(engine, (void*)storage_context));
	return true;
}

bool GetTrigger(neo::vm::ExecutionEngine *engine)
{
	// TODO: neo Trigger����
	std::cout << "GetTrigger fuction here" << std::endl;
	engine->evaluation_stack()->push_back(neo::vm::StackItem::to_stack_item(engine, 0x10));
	return true;
}

bool SetStorage(neo::vm::ExecutionEngine *engine)
{
	auto cur_script_id = engine->current_context()->script_id();
	auto &eval_stack = *(engine->evaluation_stack());
	auto storage_context_item = neo::vm::Helper::pop(eval_stack);
	if (!storage_context_item->IsUserdata())
	{
		throw NeoVmException("need storage context argument");
	}
	auto storage_context_addr = ((neo::vm::Userdata*)storage_context_item)->get_userdata_address();
	auto prop_name = neo::vm::Helper::pop(eval_stack)->GetString();
	auto prop_value = neo::vm::Helper::pop(eval_stack);

	// TODO: �� storage�浽����
	std::cout << "storage[" << prop_name << "]=" << prop_value->GetString() << std::endl;
	return true;
}

bool GetStorage(ExecutionEngine *engine)
{
	auto cur_script_id = engine->current_context()->script_id();
	auto &eval_stack = *(engine->evaluation_stack());
	auto storage_context_item = neo::vm::Helper::pop(eval_stack);
	if (!storage_context_item->IsUserdata())
	{
		throw NeoVmException("need storage context argument");
	}
	auto storage_context_addr = ((neo::vm::Userdata*)storage_context_item)->get_userdata_address();
	auto prop_name = neo::vm::Helper::pop(eval_stack)->GetString();
	// TODO: �����϶�ȡstorage
	std::cout << "storage[" << prop_name << "] loaded from chain" << std::endl;
	engine->evaluation_stack()->push_back(neo::vm::StackItem::to_stack_item(engine, 1234));
	return true;
}

bool Dummy(neo::vm::ExecutionEngine *engine)
{
	std::cout << "dummy interop service doing" << std::endl;
	return true;
}

bool CorePrint(neo::vm::ExecutionEngine *engine)
{
	std::cout << "core_print api doing" << std::endl;
	auto &eval_stack = *(engine->evaluation_stack());
	auto item = neo::vm::Helper::pop(eval_stack);
	if (item->type() == neo::vm::StackItemType::BYTE_ARRAY)
	{
		auto str = item->GetString();
		std::cout << str << std::endl;
	}
	else if (item->type() == StackItemType::INTEGER)
	{
		auto num = item->GetBigInteger();
		std::cout << num << std::endl;
	}
	return true;
}

std::vector<char> load_bytecode_file(std::string filepath)
{
	std::ifstream bytecode_file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
	if (!bytecode_file.is_open())
	{
		std::cerr << "can't open file " << filepath << std::endl;
		throw NeoVmException((std::string("Can't open file ") + filepath).c_str());
	}
	bytecode_file.seekg(0, std::ios::end);
	auto file_size = bytecode_file.tellg();
	std::vector<char> bytecode_data((unsigned int)file_size);
	bytecode_file.seekg(0, std::ios::beg);
	bytecode_file.read(bytecode_data.data(), bytecode_data.size());
	bytecode_file.close();
	return bytecode_data;
}

std::vector<char> string_to_bytes(std::string str)
{
	std::vector<char> data(str.size());
	memcpy(data.data(), str.c_str(), sizeof(char) * str.size());
	return data;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "please pass avm file as first argument" << std::endl;
		return 1;
	}

	neo::vm::impl::DemoScriptContainer script_container;
	neo::vm::impl::DemoCrypto crypto;
	neo::vm::impl::DemoScriptTable script_table;

	neo::vm::InteropService interop_service;

	// �����⼸��Ӧ����ʹ����(������)��ע��
	interop_service.register_service("Neo.Storage.GetContext", GetStorageContext);
	interop_service.register_service("Neo.Storage.Put", SetStorage);
	interop_service.register_service("Neo.Storage.Get", GetStorage);
	interop_service.register_service("Neo.Runtime.GetTrigger", GetTrigger);

	interop_service.register_service("dummy", Dummy);

	interop_service.register_service("core_print", CorePrint);

	auto engine = std::make_shared<neo::vm::ExecutionEngine>(&script_container, &crypto, &script_table, &interop_service);
	engine->open_debug_mode();

	for (auto i = 1; i < argc; i++)
	{
		std::string filepath(argv[i]);
		auto bytecode_data = load_bytecode_file(filepath);

		// engine->LoadScript(script_container->get_message(), false);
		engine->evaluation_stack()->clear();

		engine->load_script(bytecode_data, Helper::string_content_to_chars("demo_script"), false);

		// load args script
		neo::vm::ScriptBuilder sb;

		std::vector<StackItem*> script_args;
		script_args.push_back(StackItem::to_stack_item(engine.get(), string_to_bytes("demoAddress")));

		engine->evaluation_stack()->push(StackItem::to_stack_item(engine.get(), script_args)); // �ڶ������� args
		sb.emit_push("balanceOf"); // ��Ӧ�β�1, balanceOf, name
		auto sbData = sb.to_char_array();
		
		engine->load_script(sbData, Helper::string_content_to_chars("args_address"), false);
	}
	try 
	{
		engine->execute();
	}
	catch (std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
	}
	std::cout << "vm execute end with status " << engine->state() << std::endl;
	if (engine->evaluation_stack()->size() > 0)
	{
		auto result_item = engine->evaluation_stack()->peek();
		if (result_item->type() == neo::vm::StackItemType::BYTE_ARRAY)
		{
			auto result_str = result_item->GetString();
			std::cout << "result: " << result_str << std::endl;
		}
		else if (result_item->type() == StackItemType::INTEGER)
		{
			auto result_num = result_item->GetBigInteger();
			std::cout << "result: " << result_num << std::endl;
		}
	}
	return 0;
}