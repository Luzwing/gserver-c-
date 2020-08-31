#include <iostream>

int i = 1;
namespace cpan
{
	void func()
	{
		i = 2;
	}
}

namespace cpan
{
	void func2()
	{
		func();
	}
}

int main()
{
	cpan::func2();
	std::cout << i << std::endl;
	return 0;

}
