#pragma once

#include "value.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>

namespace svm
{

	struct ExecutionContext;
	struct OpCode
	{
		int64_t num;
		std::string name;
		uint8_t num_args;
		std::function<void(ExecutionContext&, const std::vector<Value>& args)> handler;
	};

	struct OpCodesTable
	{
		std::unordered_map<int64_t, OpCode> map;

		const OpCode& operator[](int64_t index) const { return map.at(index); }
		OpCode& operator[](int64_t index) { return map[index]; }
		const OpCode& operator[](std::string_view name) const
		{
			for (const auto& it : map)
			{
				if (it.second.name == name) return it.second;
			}
			throw std::runtime_error("Cannot find op code with name");
		}
	};

	struct Label
	{
		std::string name;
		uint64_t position;
		uint64_t size;
	};

	//using OpCodesTable = std::unordered_map<int64_t, OpCode>;

}
