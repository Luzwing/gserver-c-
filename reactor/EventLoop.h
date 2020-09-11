#ifndef CPAN_EVENTLOOP_H
#define CPAN_EVENTLOOP_H

#include "base/Noncopyable.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/Socket.h"

namespace cpan
{

class EventLoop : public Noncopyable
{
	public:
		EventLoop();
		~EventLoop();

		void loop();

		void assertInLoopThread()
		{
			if (!isInLoopThread())
			{
				abortNotInLoopThread();
			}
		}

		bool isInLoopThread() const
		{
			return threadId_ == Thread::getCurThreadID();
		}

	private:
		void abortNotInLoopThread();

		bool        looping_;
		const pid_t threadId_;
};

}
#endif
