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
				callFunction(it, num_args);
				return;
			}
		}

        for (const auto& it : _external_functions)
        {
            if (std::hash<std::string>()(it.name) == name_hash)
            {
                callFunction(it, num_args);
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
            const auto& funcs = _external_functions;
            const auto it = std::find_if(funcs.begin(), funcs.end(),
                                         [name](const auto& f) { return f.name == name; });
            if (it == funcs.end())
            {
                throw std::runtime_error("No such function");
            }
            callFunction(*it, num_args);
		}
		else
            callFunction(*it, num_args);
	}

	void Machine::callFunction(const Function& func, int64_t num_args)
	{
        if (!func.external)
        {
            ExecutionContext ctx;
            ctx.machine = this;
            ctx.code = _code;
            ctx.call_instr_pointer = _code->getInstructionPointer();

            if (num_args == func.script.args.size()) {
                for (int64_t i = num_args - 1; i >= 0; --i) {
                    ctx._variables[std::hash<std::string>()(func.script.args[i])] = _exec_ctx_stack.top().pop();
                }
            } else if (num_args < func.script.args.size()) {
                for (int64_t i = func.script.args.size() - 1; i >= num_args; --i) {
                    ctx._variables[std::hash<std::string>()(func.script.args[i])] = Value::null();
                }
                for (int64_t i = num_args - 1; i >= 0; --i) {
                    ctx._variables[std::hash<std::string>()(func.script.args[i])] = _exec_ctx_stack.top().pop();
                }
            } else {
                throw std::runtime_error("shit");
            }

            _exec_ctx_stack.push(ctx);
            _code->setInstructionPointer(func.script.start_instr_pointer);
        }
        else
        {
            std::vector<Value> args;
            args.resize(func.native.num_args);
            if (num_args == func.native.num_args) {
                for (int64_t i = num_args - 1; i >= 0; --i) {
                    args[i] = _exec_ctx_stack.top().pop();
                }
            } else if (num_args < func.native.num_args) {
                for (int64_t i = func.native.num_args - 1; i >= num_args; --i) {
                    args[i] = Value::null();
                }
                for (int64_t i = num_args - 1; i >= 0; --i) {
                    args[i] = _exec_ctx_stack.top().pop();
                }
            } else {
                throw std::runtime_error("shit");
            }

            func.native.handler(args, _exec_ctx_stack.top().values);
        }
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

    void Machine::addFunction(std::string_view name, uint64_t num_args,
                              const std::function<void(const std::vector<Value>&, std::stack<Value>&)>& func)
    {
        Function f;
        f.name = name;
        f.external = true;
        f.native.num_args = num_args;
        f.native.handler = func;
        _external_functions.push_back(f);
    }
}
