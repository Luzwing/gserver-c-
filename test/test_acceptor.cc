#include "../reactor/Acceptor.h"
#include "../base/logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

void NewConnectionCallback(cpan::Socket::Ptr peerAddr)
{
	CPAN_LOG_INFO(g_logger) << "World";
}

int main()
{
	CPAN_LOG_INFO(g_logger) << "Hello";
	cpan::EventLoop loop;
	cpan::IPv4Address::Ptr addrptr(cpan::IPv4Address::Create("127.0.0.1", 5050));

	cpan::Acceptor acceptor(&loop, addrptr);
	acceptor.setNewConnectionCallback(NewConnectionCallback);
	acceptor.listen();

	loop.loop();

	return 0;
}
