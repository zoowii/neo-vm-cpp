#ifndef NEOVM_EXECUTION_CONTEXT_HAPP
#define NEOVM_EXECUTION_CONTEXT_HAPP

#include <neovm/execution_engine.hpp>
#include <vector>
#include <set>

namespace neo
{
	namespace vm
	{
		class ExecutionContext
		{
		private:
			ExecutionEngineP _engine;
			std::vector<char> _script;
			bool _push_only;
			BinaryReader *_op_reader;
			std::set<uint64_t> _break_points;

			std::vector<char> _script_id;
		public:
			void set_instruction_pointer(int value);

			int get_instruction_pointer();

			std::set<uint64_t> *break_points();

			std::vector<char> script_id() const;

			bool push_only() const;

			BinaryReader *op_reader() const;

			std::vector<char> *script();

			OpCode next_instruction();

			ExecutionContext(ExecutionEngineP engine, std::vector<char> script, std::vector<char> script_id, bool push_only, std::set<uint64_t> break_points);

			ExecutionContext *clone();

			virtual ~ExecutionContext();
		};
	}
}

#endif
