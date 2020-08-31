#include <sys/types.h>
#include <netdb.h>
#include <stddef.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "Address.h"
#include "logger.h"
#include "Endian.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

namespace cpan
{

/**
 * 创建一个用于生成掩码的数字
 * T:uint32_t prefix_len:24 return : (0)
 */
template <typename T>
static T CreateMask(uint32_t prefix_len)
{
	return (T) ((1 << (sizeof(T) * 8 - prefix_len)) - 1);
}

/*********************** Address  ****************************/
int Address::getFamily() const
{
	return this->getAddr()->sa_family;
}

std::string Address::toString() const
{
	std::ostringstream os;
	this->insert(os);
	return os.str();
}

bool Address::operator<(const Address& addr) const
{
	uint32_t len1 = this->getAddrLen(), len2 = addr.getAddrLen();
	if (len1 < len2)
		return true;
	else if (len1 == len2)
	{
		int res = memcmp(this->getAddr(), addr.getAddr(), len1);
		if (res < 0)
			return true;
	}
	return false;
}

bool Address::operator==(const Address& addr) const
{
	uint32_t len1 = this->getAddrLen(), len2 = addr.getAddrLen();
	if (len1 == len2)
	{
		int res = memcmp(this->getAddr(), addr.getAddr(), len1);
		if (0 == res)
			return true;
	}
	return false;
}

bool Address::operator!=(const Address& addr) const
{
	return !(this->operator==(addr));
}

Address::Ptr Address::Create(const sockaddr* address, socklen_t len)
{
	if (!address)
		return nullptr;

	switch(address->sa_family)
	{
		case AF_INET:
			return Address::Ptr(new IPv4Address(*(const sockaddr_in*)address));
			break;
		case AF_INET6:
			return Address::Ptr(new IPv6Address(*(const sockaddr_in6*)address));
			break;
		default:
			break;
	}

	return nullptr;
}

bool Address::Lookup(std::vector<Address::Ptr>& addrs, const std::string& host, int family, int type, int protocol)
{
	if (host.empty())
		return false;

	struct addrinfo hints, *results, *next;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;

	std::string node, service;
	//ipv6 address
	if (host[0] == '[')
	{
		auto pos = host.find(']');
		if (pos == std::string::npos)
		{
			CPAN_LOG_DEBUG(g_logger) << "Lookup Error, Host:" << host;
			return false;
		}
		//如果有端口号
		if (host[pos+1] == ':' && (pos + 2) < host.size())
		{
			service = host.substr(pos + 2, host.size() - pos - 2);
		}

		node = host.substr(1, pos - 1);
	}

	//ipv4 node + service
	if (node.empty())
	{
		auto pos = host.find(':');
		if (pos != std::string::npos)
		{
			node = host.substr(0, pos);
			if ((pos + 1) < host.size())
			{
				service = host.substr(pos + 1, host.size() - pos - 1);
			}
		}
		else
		{
			node = host;
		}
	}

	if (UNLIKELY(node.empty()))
	{
		return false;
	}

	int res = getaddrinfo(node.c_str(), service.c_str(), &hints, &results);
	if (0 != res)
	{
		CPAN_LOG_DEBUG(g_logger) << "GetAddrInfo Error, family:" << family << " socktype:" << type << " protocol:"
			<< protocol << " error code:" << res << " error str:" << gai_strerror(res);
		return false;
	}
	next = results;
	while (next)
	{
		addrs.push_back(Create(next->ai_addr, (socklen_t)next->ai_addrlen));
		next = next->ai_next;
	}
	freeaddrinfo(results);
	return true;
}

/*********************** Address End  ****************************/

/*********************** IPAddress  ****************************/
IPAddress::Ptr IPAddress::Create(const char* address, uint16_t port)
{

	return nullptr;
}

/*********************** IPAddress End  ****************************/

/*********************** IPv4Address  ****************************/
IPv4Address::IPv4Address(const sockaddr_in& addr)
	: addr_(addr)
{
}

IPv4Address::IPv4Address(uint32_t address, uint16_t port)
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = cpan::byteSwapOnLittleEndian(port);
	addr_.sin_addr.s_addr = cpan::byteSwapOnLittleEndian(address);
}

const sockaddr* IPv4Address::getAddr() const
{
	return (const sockaddr*)&addr_;
}

sockaddr* IPv4Address::getAddr()
{
	return (sockaddr*)&addr_;
}

socklen_t IPv4Address::getAddrLen() const
{
	return (socklen_t)sizeof(addr_);
}

std::ostream& IPv4Address::insert(std::ostream& os) const
{
	uint32_t address = cpan::byteSwapOnLittleEndian<uint32_t>(addr_.sin_addr.s_addr);
	uint16_t port = cpan::byteSwapOnLittleEndian<uint16_t>(addr_.sin_port);
	os << ((address >> 24) & 0xff) << '.'
		<< ((address >> 16) & 0xff) << '.'
		<< ((address >> 8) & 0xff) << '.'
		<< (address & 0xff);
	os << ':' << port;
	return os;
}

IPAddress::Ptr IPv4Address::broadcastAddress(uint32_t prefix_len)
{
	return nullptr;
}

IPAddress::Ptr IPv4Address::networkAddress(uint32_t prefix_len)
{
	return nullptr;
}

IPAddress::Ptr IPv4Address::subnetMask(uint32_t prefix_len)
{
	return nullptr;
}

uint16_t IPv4Address::getPort() const
{
	return addr_.sin_port;
}

void IPv4Address::setPort(uint16_t port)
{
	addr_.sin_port = cpan::byteSwapOnLittleEndian(port);
}

IPv4Address::Ptr IPv4Address::Create(const char* address, uint16_t port)
{
	if (!address)
		return nullptr;

	IPv4Address::Ptr ptr(new IPv4Address);
	ptr->setPort(port);
	int result = inet_pton(AF_INET, address, &ptr->addr_.sin_addr.s_addr);
	if (result != 1)
	{
		CPAN_LOG_ERROR(g_logger) << "IPv4Address Create Error, address:" << address << " Return Value:" << result << " errno:" << errno << " error str:" << strerror(errno);
		return nullptr;
	}
	return ptr;
}

/*********************** IPv4Address  End  ****************************/


/*********************** IPv6Address  ****************************/

IPv6Address::IPv6Address()
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin6_family = AF_INET6;
}

IPv6Address::IPv6Address(const sockaddr_in6& addr)
	: addr_(addr)
{
}

IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port)
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin6_family = AF_INET6;
	addr_.sin6_port = cpan::byteSwapOnLittleEndian(port);
	memcpy(addr_.sin6_addr.s6_addr, address, 16);

}

const sockaddr* IPv6Address::getAddr() const
{
	return (const sockaddr*)&addr_;
}

sockaddr* IPv6Address::getAddr()
{
	return (sockaddr*)&addr_;
}

socklen_t IPv6Address::getAddrLen() const
{
	return (socklen_t)sizeof(addr_);
}

std::ostream& IPv6Address::insert(std::ostream& os) const
{
	os << '[';
	uint16_t* node = (uint16_t*)&addr_.sin6_addr.s6_addr;
	for (size_t i = 0; i < 8; ++i)
	{
		os << std::hex << byteSwapOnLittleEndian(node[i]) << std::dec;
		if (i != 7)
			os << ':';
	}
	os << ']';
	if (addr_.sin6_port)
		os << ':' << addr_.sin6_port;
	return os;
}

IPAddress::Ptr IPv6Address::broadcastAddress(uint32_t prefix_len)
{
	return nullptr;
}

IPAddress::Ptr IPv6Address::networkAddress(uint32_t prefix_len)
{
	return nullptr;
}

IPAddress::Ptr IPv6Address::subnetMask(uint32_t prefix_len)
{
	return nullptr;
}

uint16_t IPv6Address::getPort() const
{
	return addr_.sin6_port;
}

void IPv6Address::setPort(uint16_t port)
{
	addr_.sin6_port = cpan::byteSwapOnLittleEndian(port);
}

IPv6Address::Ptr IPv6Address::Create(const char* address, uint16_t port)
{
	return nullptr;
}

/*********************** IPv6Address  End ****************************/

/*********************** UnixAddress  ****************************/

UnixAddress::UnixAddress()
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sun_family = AF_UNIX;
}

UnixAddress::UnixAddress(const std::string &path)
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sun_family = AF_UNIX;
	length_ = path.size() + 1;
	if (path.empty())
		--length_;
	memcpy(addr_.sun_path, path.c_str(), length_);
	length_ += offsetof(sockaddr_un, sun_path);

}

const sockaddr* UnixAddress::getAddr() const
{
	return (const sockaddr*)&addr_;
}

sockaddr* UnixAddress::getAddr()
{
	return (sockaddr*)&addr_;
}

socklen_t UnixAddress::getAddrLen() const
{
	return length_;
}

void UnixAddress::setAddrLen(uint32_t len)
{
	length_ =  len;
}

std::ostream& UnixAddress::insert(std::ostream& os) const
{
	os << addr_.sun_path;
	return os;
}

/*********************** UnixAddress End  ****************************/
}

int main_addr()
{
	std::vector<cpan::Address::Ptr> addrs;
	cpan::Address::Lookup(addrs, "www.baidu.com", AF_INET);
	for (auto it : addrs)
	{
		std::cout <<  it->toString() << std::endl;
	}
	cpan::IPv4Address::Ptr addrptr = cpan::IPv4Address::Create("255.255.255.0", 30);
	std::cout << addrptr->toString() << std::endl;
	return 0;
}
