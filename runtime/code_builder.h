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

		virtual Code build() const = 0;

		virtual const OpCodesTable& getOpCodes() const = 0;
	};

}