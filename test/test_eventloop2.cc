#include <sys/timerfd.h>
#include <iostream>
#include "../reactor/EventLoop.h"
#include "../reactor/Poller.h"
#include "../reactor/Channel.h"

static cpan::EventLoop* g_loop = nullptr;

void timeout()
{
	std::cout << "Time Out" << std::endl;
	g_loop->quit();
}

int main()
{
	cpan::EventLoop loop;
	g_loop = &loop;

	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	cpan::Channel channel(&loop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();
	::close(timerfd);
}
