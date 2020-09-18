#ifndef CPAN_EVENTLOOP_H
#define CPAN_EVENTLOOP_H

#include <memory>
#include "base/Noncopyable.h"
#include "base/util.h"
#include "base/logger.h"
#include "base/Socket.h"
#include "Poller.h"

namespace cpan
{
class Channel;

class EventLoop : public Noncopyable
{
	public:
		typedef std::shared_ptr<EventLoop> Ptr;

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

		void quit() { quit_ = true; }

		void updateChannel(Channel* channel);

	private:
		void abortNotInLoopThread();

		typedef std::vector<Channel*> ChannelList;

		bool        looping_;
		bool        quit_;
		const pid_t threadId_;
		std::unique_ptr<Poller> poller_;
		ChannelList activeChannels_;
};

}
#endif
