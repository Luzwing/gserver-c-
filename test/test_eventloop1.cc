#include <stdio.h>
#include "../reactor/EventLoop.h"
#include "../base/Thread.h"

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n", getpid(), cpan::Thread::getCurThreadID());

	cpan::EventLoop loop;
	loop.loop();
}

int main()
{
	cpan::EventLoop loop;

	cpan::Thread t(threadFunc, "Hello");
	t.start();

	loop.loop();
	pthread_exit(NULL);
}
