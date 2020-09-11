#include <poll.h>
#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "../base/logger.h"
#include "../base/util.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

namespace cpan
{

Poller::Poller(EventLoop* loop)
	: loop_(loop)
{
}

Poller::~Poller()
{
}

void Poller::poll(int timeoutms, ChannelList* activeChannels)
{
	int eventsNum = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutms);
	if (eventsNum > 0)
	{
		fillActiveChannels(eventsNum, activeChannels);
	}
}

void Poller::updateChannel(Channel* channel)
{
	Assert(channel);
	//新的channel
	if (channel->index() < 0)
	{
		Assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = pollfds_.size() - 1;
		channel->set_index(idx);
		channels_[channel->fd()] = channel;
	}
	else
	{
		Assert(channels_.find(channel->fd()) != channels_.end());
		Assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		Assert(idx >= 0 && idx < pollfds_.size());
		auto &pfd = pollfds_[idx];
		Assert(pollfds_[idx].fd == channel->fd() || pfd.fd == -1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->isNoneEvent())
		{
			pfd.fd = -1;
		}
	}
}

void Poller::fillActiveChannels(int eventsNum, ChannelList* activeChannels) const
{
	for (auto pfd = pollfds_.begin(); pfd != pollfds_.end() && eventsNum > 0; ++pfd)
	{
		if (pfd->revents > 0)
		{
			--eventsNum;
			auto ch = channels_.find(pfd->fd);
			Assert(ch != channels_.end());
			Channel* channel = ch->second;
			Assert(channel);
			Assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activeChannels->push_back(channel);
		}
	}
}

}
