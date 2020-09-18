#ifndef CPAN_POLLER_H
#define CPAN_POLLER_H

#include <vector>
#include <map>
#include <memory>

struct pollfd;

namespace cpan
{

class Channel;
class EventLoop;

class Poller
{
	public:
		typedef std::shared_ptr<Poller> Ptr;
		typedef std::vector<Channel*> ChannelList;

		Poller(EventLoop* loop);
		~Poller();

		void poll(int timeoutms, ChannelList* activeChannels);

		void updateChannel(Channel* channel);

	private:
		void fillActiveChannels(int eventsNum, ChannelList* activeChannels) const;

		typedef std::vector<struct pollfd> PollFds;
		typedef std::map<int, Channel*> ChannelMap;

		EventLoop* loop_;
		PollFds pollfds_;
		ChannelMap channels_;

};



}
#endif
