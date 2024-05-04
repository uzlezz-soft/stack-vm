#pragma once

#include "code.h"
#include <stack>

namespace svm
{

	class Machine;
	struct ExecutionContext
	{
		uint64_t call_instr_pointer;
		std::stack<Value> return_values;
		std::stack<Value> values;
		std::unordered_map<uint64_t, Value> _variables;
		Code* code;
		Machine* machine;

		void pushReturn(Value v)
		{
			return_values.push(v);
		}

		void push(Value v)
		{
			values.push(v);
		}

		Value peek() const { return values.top(); }

		Value pop()
		{
			const Value cur = values.top();
			values.pop();
			return cur;
		}

		Value getValue(Value ref)
		{
			return _variables[ref.v.ref_index];
		}

		void cleanUp()
		{
			// TODO: references cleanup
			code->setInstructionPointer(call_instr_pointer);
		}
	};

	class Machine
	{
	public:

		Machine(OpCodesTable op_codes);

        void addFunction(std::string_view name, uint64_t num_args,
                         const std::function<void(const std::vector<Value>& args, std::stack<Value>& results)>& func);

        bool run(Code& code, std::string_view function);

		void callFunctionByHash(uint64_t name_hash, int64_t num_args);
		void callFunction(std::string_view name, int64_t num_args);
		void callFunction(const Function& func, int64_t num_args);
		void returnFromFunction();

	private:

		OpCodesTable _op_codes;
		Code* _code{ nullptr };

        std::vector<Function> _external_functions;
		std::stack<ExecutionContext> _exec_ctx_stack;
		std::unordered_map<uint64_t, Value> _variables;
	};

}