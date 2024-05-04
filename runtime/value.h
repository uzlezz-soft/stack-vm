#pragma once

#include <cstdint>
#include <string_view>
#include <optional>

namespace svm
{
	enum ValueType
	{
		VT_Null = 0, VT_Int = 1, VT_Real = 2, VT_Ref = 3,
		VT_Function = 4
	};

	struct Value
	{
		ValueType type;
		union
		{
			int64_t integer;
			double real;
			uint64_t ref_index;
			uint64_t function_name_hash;
		} v;

        [[nodiscard]] std::optional<int64_t> asInt() const
        {
            switch (type)
            {
                case VT_Null:
                case VT_Int:
                    return v.integer;
                case VT_Real:
                    return static_cast<int64_t>(v.real);
            }
            return {};
        }

		static Value null()
		{
			return { VT_Null, {.integer = 0} };
		}

		static Value integer(int64_t v)
		{
			return { VT_Int, {.integer = v} };
		}

		static Value real(double v)
		{
			return { VT_Real, {.real = v} };
		}

		static Value ref(uint64_t ref_index)
		{
			return { VT_Ref, {.ref_index = ref_index } };
		}

		static Value ref(std::string_view ref_name)
		{
			return { VT_Ref, {.ref_index = std::hash<std::string_view>{}(ref_name) } };
		}

		static Value function(std::string_view name)
		{
			return { VT_Function, {.function_name_hash = std::hash<std::string_view>()(name)}};
		}

		bool isTrue() const
		{
			switch (type)
			{
				case VT_Int: return v.integer > 0;
				case VT_Real: return v.real > 0;
				case VT_Ref: return v.ref_index > 0;
				case VT_Function: return v.function_name_hash > 0;
			}
			return false;
		}

		Value toTypeWith(Value other)
		{
			switch (type)
			{
				case VT_Null:
				{
					switch (other.type)
					{
						case VT_Null: return *this;
						case VT_Int: return integer(0);
						case VT_Real: return real(0.0);
						default:
							// throw
							break;
					}
					break;
				}

				case VT_Int:
				{
					switch (other.type)
					{
						case VT_Null:
						case VT_Int: return *this;
						case VT_Real: return real(static_cast<double>(v.integer));
						default:
							// throw
							break;
					}
					break;
				}

				case VT_Real:
				{
					switch (other.type)
					{
						case VT_Null:
						case VT_Int:
						case VT_Real: return *this;
						default:
							// throw
							break;
					}
					break;
				}
			}

			return *this;
		}
	};

}