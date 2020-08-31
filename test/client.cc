#include "../base/Socket.h"
#include "../base/logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

int main()
{
	cpan::Socket::Ptr client(new cpan::Socket(AF_INET, SOCK_STREAM, 0));
	cpan::IPv4Address::Ptr addrptr(cpan::IPv4Address::Create("127.0.0.1", 5050));
	if (!client->connect(addrptr, -1))
	{
		CPAN_LOG_ERROR(g_logger) << "Connect Error";
		return 0;
	}

	std::string str("Hello World!");
	client->send(str.c_str(), str.size(), 0);
	client->close();
	return 0;
}
