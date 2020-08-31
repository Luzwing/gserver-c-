#include <string.h>
#include "Socket.h"
#include "logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

namespace cpan
{

Socket::Socket(int family, int type, int protocol)
   : sock_(-1),
	family_(family),
	type_(type),
	protocol_(protocol),
	status_(0)
{
	sock_ = socket(family, type, protocol);
	setInited(true);
}

Socket::~Socket()
{
	close();
}

bool Socket::bind(const Address::Ptr addr)
{
	if (!isInited() || isBinded())
	{
		return false;
	}

	int ret = ::bind(sock_, addr->getAddr(), addr->getAddrLen());
	if (ret)
	{
		CPAN_LOG_ERROR(g_logger) << "Bind Error, sock:" << sock_ << " errno:" << errno << " errorstr:" << strerror(errno);
		return false;
	}
	setBinded(true);
	getLocalAddress();
	return true;
}

bool Socket::listen(int backlog)
{
	if (!isBinded())
		return false;

	int ret = ::listen(sock_, backlog);
	if (ret != 0)
	{
		CPAN_LOG_ERROR(g_logger) << "Listen Error, sock:" << sock_ << " errno:" << errno << " errorstr:" << strerror(errno);
		return false;
	}
	return true;
}

Socket::Ptr Socket::accept()
{
	if (!isBinded())
		return nullptr;

	int newsock = ::accept(sock_, NULL, NULL);
	if (-1 == newsock)
	{
		CPAN_LOG_ERROR(g_logger) << "Accept Error, sock:" << sock_ << " errono:" << errno << " errorstr:" << strerror(errno);
		return nullptr;
	}

	Socket::Ptr sock(new Socket(family_, type_, protocol_));
	if (sock->initFrom(newsock))
		return sock;

	return nullptr;
}

bool Socket::initFrom(int sock)
{
	this->sock_ = sock;
	setConnected(true);
	getLocalAddress();
	getRemoteAddress();
	return true;
}

bool Socket::connect(const Address::Ptr addr, uint64_t timeout_ms)
{
	remoteAddress_ = addr;
	if (!isValid())
	{
		return false;
	}

	if (UNLIKELY(addr->getFamily() != family_))
	{
		CPAN_LOG_ERROR(g_logger) << "Connect Error, sock:" << sock_ << " family:" << family_ << " type:" << type_ << " protocol:" << protocol_;
		return false;
	}

	if (-1 == timeout_ms)
	{
		int ret = ::connect(sock_, addr->getAddr(), addr->getAddrLen());
		if (ret)
		{
			CPAN_LOG_ERROR(g_logger) << "Connect Error, sock:" << sock_ << " family:" << family_ << " type:" << type_ << " protocol:" << protocol_ << " errno:" << errno << " errstr:" << strerror(errno);
			close();
			return false;
		}
	}
	else
	{
		close();
		return false;
	}
	
	setConnected(true);
	getRemoteAddress();
	getLocalAddress();
	return true;
}

bool Socket::close()
{
	if (!isValid())
		return false;

	setInited(false);
	setBinded(false);
	setConnected(false);
	::close(sock_);
	setClosed(true);
	return true;
}

bool Socket::reconnect(uint64_t timeout_ms)
{

}

int Socket::recv(char* buf, size_t length, int flags)
{
	if (!isConnected())
		return -1;

	return ::recv(sock_, buf, length, flags);
}

int Socket::recv(struct iovec* vec, size_t length, int flags)
{
	if (!isConnected())
		return -1;

	struct msghdr msg;
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_iovlen = length;
	msg.msg_iov = vec;

	return ::recvmsg(sock_, &msg, flags);
}

int Socket::recvFrom(char* buf, size_t length, Address::Ptr addr, int flags)
{
	if (!isConnected())
		return -1;

	socklen_t addrlen = addr->getAddrLen();
	return ::recvfrom(sock_, buf, length, flags, addr->getAddr(), &addrlen);
}

int Socket::recvFrom(struct iovec* vec, size_t length, Address::Ptr addr, int flags)
{
	if (!isConnected())
		return -1;

	struct msghdr msg;
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_iovlen = length;
	msg.msg_iov = vec;
	msg.msg_name = addr->getAddr();
	msg.msg_namelen = addr->getAddrLen();
	return ::recvmsg(sock_, &msg, flags);
}

int Socket::send(const char* buf, size_t length, int flags)
{
	if (!isConnected())
		return -1;

	return ::send(sock_, buf, length, flags);
}

int Socket::send(const struct iovec* vec, size_t length, int flags)
{
	if (!isConnected())
		return -1;

	struct msghdr msg;
	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_iovlen = length;
	msg.msg_iov = (struct iovec*)vec;

	return ::sendmsg(sock_, &msg, flags); 
}

int Socket::sendTo(const char* buf, size_t length, Address::Ptr addr, int flags)
{
	if (!isConnected())
		return -1;

	return ::sendto(sock_, buf, length, flags, addr->getAddr(), addr->getAddrLen());
}

int Socket::sendTo(const struct iovec* vec, size_t length, Address::Ptr addr, int flags)
{
	if (!isConnected() || !addr)
		return -1;

	struct msghdr msg;
	memset(&msg, 0, sizeof(msg));
	msg.msg_iovlen = length;
	msg.msg_iov = (struct iovec*)vec;
	msg.msg_name = addr->getAddr();
	msg.msg_namelen = addr->getAddrLen();

	return ::sendmsg(sock_, &msg, flags);
}

void Socket::setInited(bool status)
{
	if (status)
		status_ |= INIT_STAT;
	else
		status_ &= (~INIT_STAT);
}

void Socket::setBinded(bool status)
{
	if (status)
		status_ |= BIND_STAT;
	else
		status_ &= (~BIND_STAT);
}

void Socket::setConnected(bool status)
{
	if (status)
		status_ |= CONNECT_STAT;
	else
		status_ &= (~CONNECT_STAT);
}

void Socket::setClosed(bool status)
{
	if (status)
		status_ |= CLOSE_STAT;
	else
		status_ &= (~CLOSE_STAT);
}

Address::Ptr Socket::getRemoteAddress()
{
	if (remoteAddress_)
		return remoteAddress_;

	if (!isConnected())
		return nullptr;

	Address::Ptr result;
	switch (family_)
	{
		case AF_INET:
			result.reset(new IPv4Address());
			break;
		case AF_INET6:
			result.reset(new IPv6Address());
			break;
		case AF_UNIX:
			result.reset(new UnixAddress());
			break;
	}
	if (!result)
		return nullptr;

	socklen_t addrlen = result->getAddrLen();
	if (::getpeername(sock_, result->getAddr(), &addrlen))
	{
		CPAN_LOG_DEBUG(g_logger) << "Get Remote Address Error";
		return nullptr;
	}

	if (family_ == AF_UNIX)
	{
		UnixAddress::Ptr unixaddr = std::dynamic_pointer_cast<UnixAddress>(result);
		unixaddr->setAddrLen(addrlen);
	}
	
	remoteAddress_ = result;
	return result;
}

Address::Ptr Socket::getLocalAddress()
{
	if (localAddress_)
		return localAddress_;

	if (!isValid())
		return nullptr;

	Address::Ptr result;
	switch (family_)
	{
		case AF_INET:
			result.reset(new IPv4Address());
			break;
		case AF_INET6:
			result.reset(new IPv6Address());
			break;
		case AF_UNIX:
			result.reset(new UnixAddress());
			break;
	}
	if (!result)
		return nullptr;

	socklen_t addrlen = result->getAddrLen();
	if (::getsockname(sock_, result->getAddr(), &addrlen))
	{
		CPAN_LOG_DEBUG(g_logger) << "Get Local Address Error";
		return nullptr;
	}

	if (family_ == AF_UNIX)
	{
		UnixAddress::Ptr unixaddr = std::dynamic_pointer_cast<UnixAddress>(result);
		unixaddr->setAddrLen(addrlen);
	}
	
	localAddress_ = result;
	return result;
}

bool Socket::setOption(int level, int option, const void* optval, socklen_t optlen)
{
	int ret = ::setsockopt(sock_, level, option, optval, optlen);
	if (0 != ret)
	{
		CPAN_LOG_ERROR(g_logger) << "SetOption Error " << sock_ << " " << level << " " << errno << " " << strerror(errno); 
		return false;
	}
	return true;
}

bool Socket::getOption(int level, int option, void* optval, socklen_t* optlen)
{
	int ret = ::getsockopt(sock_, level, option, optval, optlen);
	if (0 != ret)
	{
		CPAN_LOG_ERROR(g_logger) << "GetOption Error " << sock_ << " " << level << " " << errno << " " << strerror(errno);
		return false;
	}
	return true;
}

}

