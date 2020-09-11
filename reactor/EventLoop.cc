#include <poll.h>
#include <unistd.h>
#include "EventLoop.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

__thread cpan::EventLoop* t_loopInThisThread = 0;

namespace cpan
{

	EventLoop::EventLoop()
		: looping_(false),
		threadId_(Thread::getCurThreadID())
	{
		if (t_loopInThisThread)
		{
			CPAN_LOG_FATAL(g_logger) << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
		}
		else
		{
			t_loopInThisThread = this;
		}
	}

	EventLoop::~EventLoop()
	{
		Assert(!looping_);
		t_loopInThisThread = NULL;
	}

	void EventLoop::loop()
	{
		Assert(!looping_);
		looping_ = true;

		::poll(NULL, 0, 5 * 1000);

		looping_ = false;
	}

	void EventLoop::abortNotInLoopThread()
	{
		CPAN_LOG_FATAL(g_logger) << "abortNotInLoopThread";
		_exit(1);
	}



















}
