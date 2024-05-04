#include "runtime.h"

using namespace std;

int main()
{
	using namespace svm;

	TestCodeBuilder builder;
	builder.addFunction("main");
	builder.setCurrentFunction("main");

	Label entry = builder.insertLabel("entry");
	builder.setInsertionLabel(entry.name);

	builder.push(Value::real(2));
	builder.push(Value::integer(32));
	builder.push(Value::function("pow"));
    builder.call(2);
    builder.push(Value::integer(4294967296));
    builder.cmpE();
    builder.store("result");
    builder.push(Value::real(69.0));
    builder.push(Value::real(420.0));
    builder.load("result");
    builder.push(Value::function("select"));
    builder.call(3);
	builder.push(Value::function("print"));
    builder.call(1);
	builder.ret();



	Machine m(builder.getOpCodes());

    m.addFunction("select", 3, [](const std::vector<Value>& args,
            std::stack<Value>& result)
    {
        Value a = args[0];
        Value b = args[1];
        result.push(args[2].isTrue() ? a : b);
    });

    m.addFunction("print", 1, [](const std::vector<Value>& args,
            std::stack<Value>& result)
    {
        Value a = args[0];
        switch (a.type)
        {
            case VT_Null: std::cout << "Value of type null\n"; break;
            case VT_Int: std::cout << "Value of type int = " << a.v.integer << "\n"; break;
            case VT_Real: std::cout << "Value of type real = " << a.v.real << "\n"; break;
            case VT_Ref: std::cout << "Value of type ref = " << a.v.ref_index << "\n"; break;
            case VT_Function: std::cout << "Value of type function = " << a.v.function_name_hash << "\n"; break;
            default:
            {
                std::cout << "Unknown value type\n";
            }
        }
    });

    m.addFunction("pow", 2, [](const std::vector<Value>& args,
            std::stack<Value>& result)
    {
        auto p = args[1].asInt();
        if (!p)
        {
            result.push(Value::null());
            throw std::runtime_error("Cannot convert arg[1] to int");
        }

        if (args[0].type == VT_Int)
        {
            int64_t res = args[0].v.integer;
            for (int64_t i = 1; i < p.value(); ++i)
                res *= args[0].v.integer;
            result.push(Value::integer(res));
        }
        else if (args[0].type == VT_Real)
        {
            double res = args[0].v.real;
            for (int64_t i = 1; i < p.value(); ++i)
                res *= args[0].v.real;
            result.push(Value::real(res));
        }
        else
        {
            result.push(Value::null());
            throw std::runtime_error("Unsupported value `a`");
        }
    });

	Code code = builder.build();
	m.run(code, "main");

	return 0;
}
