#pragma once

#include "code.h"
#include <array>
#include <cassert>

namespace svm
{

	class CodeBuilder
	{
	public:

		CodeBuilder() = default;
		virtual ~CodeBuilder() = default;

		virtual void addFunction(std::string_view name = "",
			const std::vector<std::string>& args = {}) = 0;
		virtual void setCurrentFunction(std::string_view name = "") = 0;

		virtual Label insertLabel(std::string_view name = "") = 0;
		virtual void setInsertionLabel(std::string_view label) = 0;

		//Code build() const { return Code{ _code }; }
		virtual Code build() const = 0;

		virtual const OpCodesTable& getOpCodes() const = 0;


	/*protected:

		template <uint64_t num = 0>
		void emit(std::string_view name, const std::array<Value, num>& args)
		{
			const OpCode& op_code = getOpCodes()[name];
			assert(op_code.num_args == args.max_size());

			_code.insert(_code.begin() + _insertion_point++, Value::integer(op_code.num));
			_code.insert(_code.begin() + _insertion_point++, Value::integer(op_code.num_args));
			for (uint64_t i = 0; i < args.size(); ++i)
			{
				_code.insert(_code.begin() + _insertion_point++, args[i]);
			}
		}

	private:

		uint64_t _insertion_point{ 0 };
		std::vector<Value> _code;*/
	};

}