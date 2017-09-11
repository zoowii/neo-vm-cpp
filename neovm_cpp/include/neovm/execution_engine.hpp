#ifndef NEOVM_EXECUTION_ENGINE_HPP
#define NEOVM_EXECUTION_ENGINE_HPP

#include <neovm/vm_state.hpp>
#include <neovm/helper.hpp>
#include <neovm/op_code.hpp>
#include <memory>
#include <vector>
#include <string>
#include <neovm/iscript_container.hpp>
#include <neovm/iinterop_interface.hpp>
#include <neovm/interop_service.hpp>
#include <neovm/iscript_table.hpp>
#include <neovm/icrypto.hpp>
#include <neovm/share_pool.hpp>
#include <neovm/random_access_stack.hpp>
#include <neovm/exceptions.hpp>

namespace neo
{
	namespace vm
	{

#define GLOBAL_ENV_KEY_MAX_LENGTH 1024


		class ExecutionContext;

		class ExecutionEngine;

		typedef std::function<void(ExecutionEngine*)> ExecutioEngineCallback;

		class ExecutionEngine
		{
		private:
			IScriptTable *_table;
			InteropService *_service;
			IScriptContainer *_script_container;
			ICrypto *_crypto;
			RandomAccessStack<ExecutionContext*> _invocation_stack;
			RandomAccessStack<StackItem*> _evaluation_stack;
			std::vector<StackItem*> _alt_stack;
			VMState _state;
			ErrorCode _exit_code;

			bool _debug_mode;

			// memory pool
			utils::ObjectPool<StackItem> _stack_items_pool;

			std::vector<ExecutioEngineCallback> _pre_close_callbacks; // 关闭前的回调函数(比如用来销毁资源等)

			// status monitor
			int64_t _gas_limit;
			int64_t _gas_used;

			std::map<std::string, StackItem*> _global_env; // 全局变量表
			std::map<std::string, StackItem*> _container_values; // engine可以被使用者与一些值关联起来，这些值不是全局变量
		public:
			ExecutionEngine(IScriptContainer *container, ICrypto *crypto, IScriptTable *table = nullptr, InteropService *service = nullptr);

			ExecutionContext* current_context() const;

			ExecutionContext* calling_context() const;

			ExecutionContext* entry_context() const;

			ICrypto *crypto() const;

			bool has_gas_limit() const;
			int64_t gas_limit() const;
			int64_t gas_used() const;
			void set_gas_limit(int64_t gas_limit);
			void set_no_gas_limit();
			void set_gas_used(int64_t gas_used);
			void add_gas_used(int64_t delta_used);

			void stop();

			void open_debug_mode();
			void close_debug_mode();
			bool in_debug_mode() const;

			void add_stack_item_to_pool(StackItem *obj);

			ExecutionContext *pop_from_invocation_stack();

			RandomAccessStack<StackItem*> *evaluation_stack();

			IScriptContainer *script_container() const;

			void add_break_point(uint64_t position);

			void add_pre_close_callbacks(ExecutioEngineCallback callback);

			virtual ~ExecutionEngine();

			void execute();

			VMState state() const;

			ErrorCode exit_code() const;

			void register_global_variable(std::string name, StackItem *value);
			void register_string_global_variabble(std::string name, std::string value);

			StackItem *get_global_variable_value(std::string name);

			void set_container_value(std::string name, StackItem *value);
			StackItem *get_container_value(std::string name);

		private:
			void ExecuteOp(OpCode opcode, ExecutionContext *context);

			void union_change_state(VMState other);

		public:
			void load_script(std::vector<char> script, std::vector<char> script_id, bool push_only);

			bool remove_break_point(uint64_t position);

			void step_into();

			void step_out();

			void step_over();

		};

		typedef ExecutionEngine* ExecutionEngineP;
	}
}

#endif
