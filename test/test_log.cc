#include "../base/logger.h"

static cpan::Logger::Ptr g_logger = cpan::LoggerManager::getInstance()->getLogger("root");



int main()
{
	CPAN_LOG_DEBUG(g_logger) << "Hello";		
//	cpan::CPAN_LOG_INFO(g_logger) << "Hello";		
//	cpan::CPAN_LOG_WARN(g_logger) << "Hello";		
//	cpan::CPAN_LOG_ERROR(g_logger) << "Hello";		
//	cpan::CPAN_LOG_FATAL(g_logger) << "Hello";		
	return 0;
	
}
