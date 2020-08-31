#include <iostream>
#include "../base/util.h"

void func()
{
	std::string s = cpan::Backtrace("\n", 10);
	std::cout << s;
	return;
}

void func2()
{
	func();
}

int main()
{
	func2();
	return 0;
}
