#pragma once

#include "opcode.h"
#include <stack>

namespace svm
{

	struct Function
	{
		std::string name;
		uint64_t name_hash;
        bool external = false;

        struct
        {
            std::vector<std::string> args;
            uint64_t start_instr_pointer;
        } script;

        struct
        {
            uint64_t num_args;
            std::function<void(const std::vector<Value> &args, std::stack<Value> &results)> handler;
        } native;
	};

	class Code
	{
	public:

		Code(const std::vector<Value>& data, const std::vector<Function>& funcs)
			: _data(data), _funcs(funcs), _instr_pointer(0) {}

		const std::vector<Value>& getData() const { return _data; }
		const std::vector<Function>& getFunctions() const { return _funcs; }
		uint64_t getSize() const { return _data.size(); }
		uint64_t getInstructionPointer() const { return _instr_pointer; }

		Value next() { return _data[_instr_pointer++]; }
		void setInstructionPointer(uint64_t p) { _instr_pointer = p; }

		bool end() const { return _instr_pointer >= getSize(); }

	private:

		uint64_t _instr_pointer;
		std::vector<Value> _data;
		std::vector<Function> _funcs;
	};

}