#include <iostream>
#include "../base/Thread.h"

using namespace std;

int main()
{
	cpan::Thread thread([](){cout << "Hello World!";} , "chenpan");
	thread.start();
	thread.join();
	return 0;
}

