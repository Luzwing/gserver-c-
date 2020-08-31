#include <iostream>
#include <stdlib.h>
#include "../base/ThreadPool.h"

int main()
{
	cpan::ThreadPool pool1("pool1");
	pool1.setThreadInitCb([](){ std::cout << "Hello!" << std::endl;});
	pool1.setQueueSize(5);
	pool1.start(2);
	pool1.run([](){ for (int i = 0; i < 10; ++i) {std::cout << "Hello World!1" << std::endl; sleep(1);}}) ;
	pool1.run([](){ for (int i = 0; i < 10; ++i) {std::cout << "Hello World!2" << std::endl; sleep(1);}}) ;
	pool1.run([](){ for (int i = 0; i < 10; ++i) {std::cout << "Hello World!3" << std::endl; sleep(1);}}) ;
	pool1.run([](){ for (int i = 0; i < 10; ++i) {std::cout << "Hello World!4" << std::endl; sleep(1);}}) ;
	pool1.run([](){ for (int i = 0; i < 10; ++i) {std::cout << "Hello World!5" << std::endl; sleep(1);}}) ;
	sleep(100);
	return 0;
}
