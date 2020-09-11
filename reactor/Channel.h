#ifndef CPAN_CHANNEL_H
#define CPAN_CHANNEL_H

#include <functional>
#include "base/Noncopyable.h"

namespace cpan
{

class EventLoop;


class Channel : public Noncopyable
{
	public:
		typedef std::function<void()> EventCallback;

		Channel(EventLoop* loop, int fd);
		~Channel();

		void update();

		void handleEvent();

		void setReadCallback(const EventCallback& cb)
		{ readCallback_ = cb; }

		void setWriteCallback(const EventCallback& cb)
		{ writeCallback_ = cb; }

		void setErrorCallback(const EventCallback& cb)
		{ errorCallback_ = cb; }

		void enableReading() { events_ |= ReadEventFlag; update(); }
		void enableWriting() { events_ |= WriteEventFlag; update(); }
		void disableReading() { events_ &= ~ReadEventFlag; update(); }
		void disableWriting() { events_ &= ~WriteEventFlag; update(); }
		void disableAll() { events_ = NoneEventFlag; update(); }

		int fd() const { return fd_; }
		int events() const { return events_; }
		EventLoop* ownerLoop() const { return loop_; }
		void set_revents(int revt) { revents_ = revt; }
		bool isNoneEvent() const { return events_ == NoneEventFlag; }

		int index() const { return index_; }
		void set_index(int index) { index_ = index; }

	private:
		static const int NoneEventFlag;
		static const int ReadEventFlag;
		static const int WriteEventFlag;
		static const int ErrorEventFlag;

	private:
		EventLoop*    loop_;
		const int     fd_;
		int           events_;
		int           revents_;
		int           index_;

		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback errorCallback_;

};

}
#endif
