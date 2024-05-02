#include "machine.h"

#include <iostream>

namespace svm
{
	Machine::Machine(OpCodesTable op_codes)
		: _op_codes(std::move(op_codes))
	{}

	bool Machine::run(Code& code, std::string_view function)
	{
		_code = &code;

		/*ExecutionContext ctx;
		ctx.call_instr_pointer = 0;
		ctx.code = &code;
		ctx.machine = this;
		_exec_ctx_stack.push(ctx);
		std::vector<Value> args;*/

		callFunction(function, 0);
		std::vector<Value> args;

		while (!_exec_ctx_stack.empty() && !_code->end())
		{
			auto& ctx = _exec_ctx_stack.top();

			Value instr = code.next();
			auto& op_code = _op_codes[instr.v.integer];

			Value num_args = code.next();

			if (num_args.v.integer != op_code.num_args)
			{
				std::cerr << "Error at position " << code.getInstructionPointer()
					<< ": op code has " << op_code.num_args << " args, but "
					<< num_args.v.integer << " were provided.";
				return false;
			}

			args.resize(num_args.v.integer);
			for (int64_t i = 0; i < num_args.v.integer; ++i)
				args[i] = code.next();

			op_code.handler(ctx, args);
		}

		if (_code->end() && !_exec_ctx_stack.empty())
		{
			throw std::runtime_error("Reached end of code but execution stack is not empty");
		}

		return true;
	}

	void Machine::callFunctionByHash(uint64_t name_hash, int64_t num_args)
	{
		for (const auto& it : _code->getFunctions())
		{
			if (std::hash<std::string>()(it.name) == name_hash)
			{
				callFunction(/*it.start_instr_pointer, num_args*/it, num_args);
				return;
			}
		}

		throw std::runtime_error("No such function");
	}

	void Machine::callFunction(std::string_view name, int64_t num_args)
	{
		const auto& funcs = _code->getFunctions();
		const auto it = std::find_if(funcs.begin(), funcs.end(),
			[name](const auto& f) { return f.name == name; });
		if (it == funcs.end())
		{
			throw std::runtime_error("No such function");
		}
		callFunction(/*it->start_instr_pointer, num_args*/*it, num_args);
	}

	void Machine::callFunction(/*uint64_t start_instr_pointer, int64_t num_args*/const Function& func, int64_t num_args)
	{
		ExecutionContext ctx;
		ctx.machine = this;
		ctx.code = _code;
		ctx.call_instr_pointer = _code->getInstructionPointer();

		if (num_args == func.args.size())
		{
			for (int64_t i = num_args - 1; i >= 0; --i)
			{
				ctx._variables[std::hash<std::string>()(func.args[i])] = _exec_ctx_stack.top().pop();
			}
		}
		else if (num_args < func.args.size())
		{
			for (int64_t i = func.args.size() - 1; i >= num_args; --i)
			{
				ctx._variables[std::hash<std::string>()(func.args[i])] = Value::null();
			}
			for (int64_t i = num_args - 1; i >= 0; --i)
			{
				ctx._variables[std::hash<std::string>()(func.args[i])] = _exec_ctx_stack.top().pop();
			}
		}
		else
		{
			throw std::runtime_error("shit");
		}

		/*for (int64_t i = 0; i < num_args; ++i)
		{
			auto v = _exec_ctx_stack.top().pop();

			//ctx.push(_exec_ctx_stack.top().pop());
		}*/

		_exec_ctx_stack.push(ctx);
		_code->setInstructionPointer(func.start_instr_pointer);
	}

	void Machine::returnFromFunction()
	{
		auto& ctx = _exec_ctx_stack.top();
		ctx.cleanUp();

		{
			auto ctx = _exec_ctx_stack.top();
			_exec_ctx_stack.pop();
			while (!ctx.return_values.empty())
			{
				_exec_ctx_stack.top().push(ctx.return_values.top());
				ctx.return_values.pop();
			}
		}
	}

	/*void Machine::runFunction(ExecutionContext& ctx, uint64_t levels_deep)
	{
		std::vector<Value> args;
		while (!ctx.ended)
		{
			Value instr = ctx.code->next();
			auto& op_code = _op_codes[instr.v.integer];

			Value num_args = ctx.code->next();

			if (num_args.v.integer != op_code.num_args)
			{
				std::cerr << "Error at position " << ctx.code->getInstructionPointer()
					<< ": op code has " << op_code.num_args << " args, but "
					<< num_args.v.integer << " were provided.";
				return;
			}

			args.resize(num_args.v.integer);
			for (int64_t i = 0; i < num_args.v.integer; ++i)
				args[i] = code.next();

			op_code.handler(ctx, args);
		}
	}*/
}
