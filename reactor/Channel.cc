#include <poll.h>
#include "Channel.h"
#include "../base/logger.h"
#include "EventLoop.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

const int cpan::Channel::NoneEventFlag = 0;
const int cpan::Channel::ReadEventFlag = POLLIN | POLLPRI;
const int cpan::Channel::WriteEventFlag = POLLOUT;
const int cpan::Channel::ErrorEventFlag = POLLERR;

namespace cpan
{

Channel::Channel(EventLoop* loop, int fd)
	: loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0),
	index_(-1)
	{}

Channel::~Channel() {}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent()
{
	CPAN_LOG_DEBUG(g_logger) << "Channel::handleEvent()";
	if (revents_ & POLLNVAL)
	{
		CPAN_LOG_WARN(g_logger) << "Channel::handleEvent() POLLNVAL";
	}

	if (revents_ & (POLLERR | POLLNVAL))
	{
		if (errorCallback_) errorCallback_();
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if (readCallback_) readCallback_();
	}

	if (revents_ & POLLOUT)
	{
		if (writeCallback_) writeCallback_();
	}
}






















}
