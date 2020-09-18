#include "Acceptor.h"
#include "../base/logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

namespace cpan
{

Acceptor::Acceptor(EventLoop* loop, const Address::Ptr addr)
	: loop_(loop),
	acceptsocket_(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP),
	acceptchannel_(loop, acceptsocket_.fd())
{
	acceptsocket_.bind(addr);
	acceptchannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
	loop_->assertInLoopThread();
	listening_ = true;
	acceptsocket_.listen();
	acceptchannel_.enableReading();
}

void Acceptor::handleRead()
{
	loop_->assertInLoopThread();
	Socket::Ptr peer = acceptsocket_.accept();
	if (peer)
	{
		if (newconnectioncb_)
			newconnectioncb_(peer);
		else
			peer->close();
	}
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback& cb)
{
	newconnectioncb_ = cb;
}

}
