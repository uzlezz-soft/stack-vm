#include "test_code_builder.h"
#include "machine.h"
#include <iostream>

namespace svm
{

	TestCodeBuilder::TestCodeBuilder()
	{
		_table[0] = OpCode{ .num = 0, .name = "nop", .num_args = 0,
		.handler = [](ExecutionContext& _, const std::vector<Value>& _args) {} };

		_table[1] = OpCode{ .num = 1, .name = "push", .num_args = 1,
			.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
			{
				ctx.push(args[0]);
			} };

		_table[2] = OpCode{ .num = 2, .name = "copy", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
			ctx.push(ctx.peek());
		} };

		_table[5] = OpCode{ .num = 5, .name = "add", .num_args = 0,
			.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
			{
				Value b = ctx.pop();
				Value a = ctx.pop();
				Value r = Value::null();

				a = a.toTypeWith(b);
				b = b.toTypeWith(a);

				switch (a.type)
				{
					case VT_Int: r = Value::integer(a.v.integer + b.v.integer); break;
					case VT_Real: r = Value::integer(a.v.real + b.v.real); break;
					default:
						/* throw */
						break;
				}

				/*if (a.type == VT_Int && b.type == VT_Int) r = Value::integer(a.v.integer + b.v.integer);
				if (a.type == VT_Real && b.type == VT_Int) r = Value::real(a.v.real + b.v.integer);
				if (a.type == VT_Int && b.type == VT_Real) r = Value::real(a.v.integer + b.v.real);
				else if (a.type == VT_Real && b.type == VT_Real) r = Value::real(a.v.real + b.v.real);
				else /* throw error */

				ctx.push(r);
			} };

		_table[6] = OpCode{ .num = 6, .name = "sub", .num_args = 0,
			.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
			{
				Value b = ctx.pop();
				Value a = ctx.pop();
				Value r = Value::null();

				if (a.type == VT_Int && b.type == VT_Int) r = Value::integer(a.v.integer - b.v.integer);
				if (a.type == VT_Real && b.type == VT_Int) r = Value::real(a.v.real - b.v.integer);
				if (a.type == VT_Int && b.type == VT_Real) r = Value::real(a.v.integer - b.v.real);
				else if (a.type == VT_Real && b.type == VT_Real) r = Value::real(a.v.real - b.v.real);
				else /* throw error */;

				ctx.push(r);
			} };


		_table[7] = OpCode{ .num = 7, .name = "mul", .num_args = 0,
			.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
			{
				Value b = ctx.pop();
				Value a = ctx.pop();
				Value r = Value::null();

				if (a.type == VT_Int && b.type == VT_Int) r = Value::integer(a.v.integer * b.v.integer);
				if (a.type == VT_Real && b.type == VT_Int) r = Value::real(a.v.real * b.v.integer);
				if (a.type == VT_Int && b.type == VT_Real) r = Value::real(a.v.integer * b.v.real);
				else if (a.type == VT_Real && b.type == VT_Real) r = Value::real(a.v.real * b.v.real);
				else /* throw error */;

				ctx.push(r);
			} };


		_table[8] = OpCode{ .num = 8, .name = "div", .num_args = 0,
			.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
			{
				Value b = ctx.pop();
				Value a = ctx.pop();
				Value r = Value::null();

				if (a.type == VT_Int && b.type == VT_Int) r = Value::integer(a.v.integer / b.v.integer);
				if (a.type == VT_Real && b.type == VT_Int) r = Value::real(a.v.real / b.v.integer);
				if (a.type == VT_Int && b.type == VT_Real) r = Value::real(a.v.integer / b.v.real);
				else if (a.type == VT_Real && b.type == VT_Real) r = Value::real(a.v.real / b.v.real);
				else /* throw error */;

				ctx.push(r);
			} };

		_table[9] = OpCode{ .num = 9, .name = "toint", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
				Value a = ctx.pop();

				switch (a.type)
				{
				case VT_Null: ctx.push(Value::integer(0)); break;
				case VT_Int: ctx.push(a); break;
				case VT_Real: ctx.push(Value::integer(static_cast<int64_t>(a.v.real))); break;
				default: /* throw error */ break;
				}
		} };

		_table[10] = OpCode{ .num = 10, .name = "toreal", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
				Value a = ctx.pop();

				switch (a.type)
				{
				case VT_Null: ctx.push(Value::real(0.0)); break;
				case VT_Int: ctx.push(Value::real(static_cast<double>(a.v.integer))); break;
				case VT_Real: ctx.push(a); break;
				default: /* throw error */ break;
				}
		} };



		_table[11] = OpCode{ .num = 11, .name = "cmple", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
			Value b = ctx.pop();
			Value a = ctx.pop();

			a = a.toTypeWith(b);
			b = b.toTypeWith(a);

			Value r = Value::integer(0);

			switch (a.type)
			{
				case VT_Int: r = Value::integer(a.v.integer < b.v.integer); break;
				case VT_Real: r = Value::integer(a.v.real < b.v.real); break;
			}

			ctx.push(r);
		} };



		_table[50] = OpCode{ .num = 50, .name = "jump", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
		{
			ctx.code->setInstructionPointer(args[0].v.ref_index);
		} };

		_table[51] = OpCode{ .num = 51, .name = "br", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
		{
			Value a = ctx.pop();

			if (a.isTrue())
				ctx.code->setInstructionPointer(args[0].v.ref_index);
		} };

		_table[52] = OpCode{ .num = 52, .name = "ret", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
			ctx.machine->returnFromFunction();
		} };

		_table[53] = OpCode{ .num = 53, .name = "retn", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value> args)
		{
			int64_t num_values = args[0].v.integer;
			for (int64_t i = 0; i < num_values; ++i)
			{
				ctx.pushReturn(ctx.pop());
			}
			ctx.machine->returnFromFunction();
		} };

		_table[54] = OpCode{ .num = 54, .name = "call", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
		{
			Value a = ctx.pop();
			if (a.type != VT_Function)
				throw std::runtime_error("Cannot call function from top of the stack: not a function");
			ctx.machine->callFunctionByHash(a.v.function_name_hash, args[0].v.integer);
		} };

		_table[60] = OpCode{ .num = 60, .name = "store", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
		{
			Value v = ctx.pop();
			ctx._variables[args[0].v.ref_index] = v;
		} };

		_table[61] = OpCode{ .num = 61, .name = "load", .num_args = 1,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& args)
		{
			ctx.push(ctx._variables[args[0].v.ref_index]);
		} };

		_table[100] = OpCode{ .num = 100, .name = "print", .num_args = 0,
		.handler = [](ExecutionContext& ctx, const std::vector<Value>& _args)
		{
			Value a = ctx.pop();
			switch (a.type)
			{
				case VT_Null: std::cout << "Value of type null\n"; break;
				case VT_Int: std::cout << "Value of type int: " << a.v.integer << "\n"; break;
				case VT_Real: std::cout << "Value of type real: " << a.v.real << "\n"; break;
				case VT_Ref: std::cout << "Value of type ref: " << a.v.ref_index << "\n"; break;
				case VT_Function: std::cout << "Value of type function: " << a.v.function_name_hash << "\n"; break;
				default:
				{
					std::cout << "Unknown value type\n";
				}
			}
		} };
	}

	void TestCodeBuilder::addFunction(std::string_view name, const std::vector<std::string>& args)
	{
		_functions[name.data()] = {name.data(), args, {}};
	}

	void TestCodeBuilder::setCurrentFunction(std::string_view name)
	{
		auto it = std::find_if(_functions.begin(), _functions.end(),
			[&name](const auto& f) { return f.first == name; });
		if (it == _functions.end()) return;

		_current_function = &it->second;
	}

	Label TestCodeBuilder::insertLabel(std::string_view n)
	{
		Label l(n.data(), 0, 0);
		_current_function->labels.push_back({ l, {} });
		return l;
	}

	void TestCodeBuilder::setInsertionLabel(std::string_view name)
	{
		auto it = std::find_if(_current_function->labels.begin(), _current_function->labels.end(),
			[name](const auto& pair) { return pair.first.name == name; });
		if (it == _current_function->labels.end()) return;

		_current_label = &it->first;
		_code = &it->second;
	}

	Code TestCodeBuilder::build() const
	{
		std::vector<Value> values;
		std::vector<svm::Function> funcs;

		for (auto fn_pair : _functions)
		{
			svm::Function func;
			func.name = fn_pair.first;
			func.args = fn_pair.second.args;
			func.start_instr_pointer = values.size();
			funcs.push_back(func);

			std::vector<uint64_t> jump_instructions;

			for (auto& label : fn_pair.second.labels)
			{
				label.first.position = values.size();
				label.first.size = label.second.size();
				values.insert(values.end(), label.second.begin(), label.second.end());\

				for (const auto& it : fn_pair.second.jump_instructions)
				{
					if (it.first != label.first.name) continue;

					for (uint64_t instr : it.second)
					{
						jump_instructions.push_back(label.first.position + instr);
					}
				}
			}

			for (uint64_t instr : jump_instructions)
			{
				uint64_t hash = values[instr].v.ref_index;

				for (auto& l : fn_pair.second.labels)
				{
					if (std::hash<std::string>()(l.first.name) == hash)
					{
						values[instr].v.ref_index = l.first.position;
						break;
					}
				}
			}
		}

		return Code{ values, funcs };
	}
}
