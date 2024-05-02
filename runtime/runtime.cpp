#include "runtime.h"

using namespace std;

int main()
{
	using namespace svm;

	TestCodeBuilder builder;

R"(

main()
entry:
	push 2.0
	push pow5times
	call
	print
	ret

pow5times(a)
entry:
	push 1
	store i
	load a
	store r
loop_start:
	load i
	push 5
	cmple
	br loop
	jump loop_end

loop:
	load r
	load a
	mul
	store r
	load i
	push 1
	add
	store i
	jump loop_start

loop_end:
	load r
	ret 1

)";

	builder.addFunction("main");
	builder.setCurrentFunction("main");

	Label entry = builder.insertLabel("entry");
	builder.setInsertionLabel(entry.name);

	builder.push(Value::real(2));
	builder.push(Value::integer(16));
	builder.push(Value::function("pow"));
	builder.call(2);
	builder.print();
	builder.ret();



	builder.addFunction("pow", { "a", "n" });
	builder.setCurrentFunction("pow");

	entry = builder.insertLabel("entry");
	Label loop_start = builder.insertLabel("loop_start");
	Label loop = builder.insertLabel("loop");
	Label loop_end = builder.insertLabel("loop_end");
	builder.setInsertionLabel(entry.name);
	// i = 1
	builder.push(Value::integer(1));
	builder.store("i");
	// r = a
	builder.load("a");
	builder.store("r");

	builder.setInsertionLabel(loop_start.name);
	// i < n
	builder.load("i");
	builder.load("n");
	builder.cmpLE();
	builder.br(loop);
	builder.jump(loop_end);

	builder.setInsertionLabel(loop.name);
	// r = r * a
	builder.load("r");
	builder.load("a");
	builder.mul();
	builder.store("r");
	// i = i + 1
	builder.load("i");
	builder.push(Value::integer(1));
	builder.add();
	builder.store("i");
	builder.jump(loop_start);

	builder.setInsertionLabel(loop_end.name);
	builder.load("r");
	builder.ret(1);

	/*builder.push(Value::integer(1));
	builder.push(Value::real(68.0));
	builder.add();
	builder.push(Value::integer(2));
	builder.push(Value::function("pow5times"));
	builder.call(1);
	builder.div();
	builder.print();
	builder.ret();

	builder.addFunction("pow5times");
	builder.setCurrentFunction("pow5times");

	entry = builder.insertLabel("entry");
	builder.setInsertionLabel(entry.name);
	builder.copy();
	builder.mul();
	builder.ret(1);*/

	Machine m(builder.getOpCodes());

	Code code = builder.build();
	m.run(code, "main");

	return 0;
}
