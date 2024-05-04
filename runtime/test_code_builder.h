#pragma once

#include <map>

#include "code_builder.h"

namespace svm
{

	class TestCodeBuilder : public CodeBuilder
	{
	public:

		struct Function
		{
			std::string name;
			std::vector<std::string> args;
			std::vector<std::pair<Label, std::vector<Value>>> labels;
			std::unordered_map<std::string /* label name */, std::vector<uint64_t> /* jump instructions */> jump_instructions;
		};

		TestCodeBuilder();

		void addFunction(std::string_view name,
			const std::vector<std::string>& args = {}) override;
		void setCurrentFunction(std::string_view name) override;

		Label insertLabel(std::string_view name) override;
		void setInsertionLabel(std::string_view label) override;

		Code build() const override;

		const OpCodesTable& getOpCodes() const override { return _table; }

		void nop() { emit("nop", {}); }
		void push(Value v) { emit<1>("push", { v }); }
		void copy() { emit("copy", {}); }
		void add() { emit("add", {}); }
		void sub() { emit("sub", {}); }
		void mul() { emit("mul", {}); }
		void div() { emit("div", {}); }

		void toInt() { emit("toint", {}); }
		void toReal() { emit("toreal", {}); }

        void cmpLE() { emit("cmple", {}); }
        void cmpGE() { emit("cmpge", {}); }
        void cmpE() { emit("cmpe", {}); }
        void cmpNE() { emit("cmpne", {}); }
        void cmpL() { emit("cmpl", {}); }
        void cmpG() { emit("cmpg", {}); }
        void inv() { emit("inv", {}); }

		void ret() { emit("ret", {}); }
		void ret(int64_t num_values) { emit<1>("retn", { Value::integer(num_values) }); }
		void call(int64_t num_args) { emit<1>("call", { Value::integer(num_args) }); }

		void store(std::string_view var) { emit<1>("store", { Value::ref(var) }); }
		void load(std::string_view var) { emit<1>("load", { Value::ref(var) }); }

		// Unconditional jump to label
		void jump(const Label& label)
		{
			emit<1>("jump", { Value::ref(label.name) });
			_current_function->jump_instructions[_current_label->name].push_back(_current_label->size - 1);
		}
		// Jump if true
		void br(const Label& label)
		{
			emit<1>("br", { Value::ref(label.name) });
			_current_function->jump_instructions[_current_label->name].push_back(_current_label->size - 1);
		}

	private:

		OpCodesTable _table;
		//std::unordered_map<std::string, Label> _labels;

		std::unordered_map<std::string, Function> _functions;

		Function* _current_function{ nullptr };
		Label* _current_label{ nullptr };
		std::vector<Value>* _code{ nullptr };

		template <uint64_t num = 0>
		void emit(std::string_view name, const std::array<Value, num>& args)
		{
			const OpCode& op_code = getOpCodes()[name];
			assert(op_code.num_args == args.max_size());

			auto& code = *_code;

			code.push_back(Value::integer(op_code.num));
			code.push_back(Value::integer(op_code.num_args));
			for (uint64_t i = 0; i < args.size(); ++i)
			{
				code.push_back(args[i]);
			}
			_current_label->size = code.size();
		}
	};

}