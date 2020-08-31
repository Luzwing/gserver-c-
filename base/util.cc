#include "util.h"
#include "logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");

namespace cpan
{

void Assert(bool status)
{
	if (UNLIKELY(!status))
	{
		//CPAN_LOG_ERROR(g_logger) << Backtrace("\n",10);
		assert(status);
	}
}

void Backtrace(std::vector<std::string> &res, int size)
{
	int nptrs;
	void** buffer = (void**)malloc(sizeof(void*) * size);
	char** strings;

	nptrs = backtrace(buffer, size);
	if (0 == nptrs)
	{
		SAFE_FREE(buffer);
		return;
	}

	strings = backtrace_symbols(buffer, nptrs);
	if (nullptr == strings)
	{
		SAFE_FREE(buffer);
		return;
	}

	for (int i = 0; i < nptrs; ++i)
	{
		res.push_back(std::string(strings[i]));
	}
	SAFE_FREE(strings);
	SAFE_FREE(buffer);
}

std::string Backtrace(const std::string &skip, int size)
{
	std::vector<std::string> tmp;
	std::ostringstream oss;

	Backtrace(tmp, size);
	if (tmp.empty())
	{
		return std::string("");
	}

	for (auto begin = tmp.cbegin(); begin != tmp.cend(); ++begin)
	{
		oss << *begin << skip;
	}
	return oss.str();
}

pid_t getCurrentTid()
{
	pid_t tid = syscall(SYS_gettid);
	return tid;
}

}
