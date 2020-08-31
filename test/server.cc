#include <iostream>
#include "../base/Socket.h"
#include "../base/logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

int main()
{
	cpan::Socket::Ptr sockptr(new cpan::Socket(AF_INET, SOCK_STREAM, 0));
	cpan::IPv4Address::Ptr addrptr(cpan::IPv4Address::Create("127.0.0.1", 5050));
	if (!sockptr->bind(addrptr))
	{
		CPAN_LOG_ERROR(g_logger) << "Bind Error";
	}
	if (!sockptr->listen())
	{
		CPAN_LOG_ERROR(g_logger) << "Listen Error";
	}

	while (true)
	{
		cpan::Socket::Ptr clientptr = sockptr->accept();
		char buf[100];
		int ret = clientptr->recv(buf, 99);
		buf[ret] = '\0';
		std::cout << buf << std::endl;
		std::cout << ret << std::endl;
		break;
	}

	return 0;
}
