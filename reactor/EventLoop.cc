#include <poll.h>
#include <unistd.h>
#include "EventLoop.h"
#include "Channel.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");
static const int TimeOutMs = 10000;

__thread cpan::EventLoop* t_loopInThisThread = 0;

namespace cpan
{

	EventLoop::EventLoop()
		: looping_(false),
		threadId_(Thread::getCurThreadID())
	{
		poller_.reset(new Poller(this));
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
		assertInLoopThread();
		looping_ = true;
		quit_ = false;

		while(!quit_)
		{
			activeChannels_.clear();
			poller_->poll(TimeOutMs, &activeChannels_);
			for (Channel* c : activeChannels_)
			{
				c->handleEvent();
			}
		}
		looping_ = false;
	}

	void EventLoop::abortNotInLoopThread()
	{
		CPAN_LOG_FATAL(g_logger) << "abortNotInLoopThread";
		_exit(1);
	}

	void EventLoop::updateChannel(Channel* channel)
	{
		Assert(channel->ownerLoop() == this);
		assertInLoopThread();
		poller_->updateChannel(channel);
	}



}
