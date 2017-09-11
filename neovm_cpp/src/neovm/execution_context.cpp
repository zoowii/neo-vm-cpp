#include <neovm/execution_context.hpp>

namespace neo
{
	namespace vm
	{
		ExecutionContext::ExecutionContext(ExecutionEngineP engine, std::vector<char> script, std::vector<char> script_id, bool push_only, std::set<uint64_t> break_points)
		{
			this->_engine = engine;
			this->_script = script;
			this->_script_id = script_id;
			this->_push_only = push_only;
			this->_op_reader = new BinaryReader(script);
			this->_break_points = break_points;
		}

		void ExecutionContext::set_instruction_pointer(int value)
		{
			_op_reader->Seek(value, BinaryReader::BEGIN);
		}

		int ExecutionContext::get_instruction_pointer()
		{
			return (int)_op_reader->position();
		}

		std::set<uint64_t>* ExecutionContext::break_points()
		{
			return &_break_points;
		}

		std::vector<char> ExecutionContext::script_id() const
		{
			return _script_id;
		}

		bool ExecutionContext::push_only() const
		{
			return _push_only;
		}

		BinaryReader* ExecutionContext::op_reader() const
		{
			return _op_reader;
		}

		std::vector<char>* ExecutionContext::script()
		{
			return &_script;
		}

		OpCode ExecutionContext::next_instruction() 
		{ 
			return (OpCode)((byte)_script[_op_reader->position()]); 
		}

		ExecutionContext* ExecutionContext::clone()
		{
			auto other = new ExecutionContext(_engine, _script, _script_id, _push_only, _break_points);
			other->set_instruction_pointer(get_instruction_pointer());
			return other;
		}

		ExecutionContext::~ExecutionContext()
		{
			// _op_reader->Dispose();
		}

	}
}