#ifndef CPAN_LOGGER_H
#define CPAN_LOGGER_H

#include <memory>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "Singleton.h"
#include "util.h"
#include "Thread.h"
#include "Mutex.h"

namespace cpan
{

#define CPAN_LOG(logger, level) \
		cpan::LogEventWrap(cpan::LogEvent::Ptr(new cpan::LogEvent( \
					logger, level, __FILE__, __LINE__, cpan::getCurrentTid(), cpan::Thread::getCurName(), time(NULL)))).getSS()

#define CPAN_LOG_DEBUG(logger) CPAN_LOG(logger, cpan::LogLevel::DEBUG)
#define CPAN_LOG_INFO(logger) CPAN_LOG(logger, cpan::LogLevel::INFO)
#define CPAN_LOG_WARN(logger) CPAN_LOG(logger, cpan::LogLevel::WARN)
#define CPAN_LOG_ERROR(logger) CPAN_LOG(logger, cpan::LogLevel::ERROR)
#define CPAN_LOG_FATAL(logger) CPAN_LOG(logger, cpan::LogLevel::FATAL)

enum LogLevel
{
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERROR = 4,
	FATAL = 5

};

static const char* ToString(LogLevel level)
{
	switch (level)
	{
		#define XX(level) \
		case LogLevel::level : \
			return #level;

		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);

		#undef XX
		default:
			return "Default";
			break;
	}
	return "Unknown";
}

class Logger;

class LogEvent
{
	public:
		typedef std::shared_ptr<LogEvent> Ptr;

		LogEvent(std::shared_ptr<Logger> logger, LogLevel level, const char* file, uint32_t line, uint32_t threadId, const std::string& threadName, uint64_t timestamp);

		const char* getFile() const { return file_; }
		uint32_t getLine() const { return line_; }
		uint32_t getThreadId() const { return threadId_; }
		uint64_t getTimestamp() const { return timestamp_; }
		const std::string& getThreadName() const { return threadName_; }
		const std::string getContents() const { return ss_.str(); }
		std::shared_ptr<Logger> getLogger() { return logger_; }
		const LogLevel getLevel() const { return level_; }

		std::stringstream& getSS() { return ss_; }

	private:
		std::shared_ptr<Logger>	logger_;
		const char*				file_ = nullptr;
		uint32_t				line_ = 0;
		uint32_t				threadId_ = 0;
		std::string				threadName_;
		uint64_t				timestamp_ = 0;
		LogLevel				level_;
		std::stringstream		ss_;
};

class LogEventWrap
{
	public:
		LogEventWrap(LogEvent::Ptr event) : event_(event) {}
		~LogEventWrap();

		std::stringstream& getSS() { return event_->getSS(); }

	private:
		LogEvent::Ptr event_;
};

class LogFormatter
{
	public:
		class FormatItem
		{
			public:
				typedef std::shared_ptr<FormatItem> Ptr;

				~FormatItem() {} 

				virtual void format(std::ostream &oss, LogLevel level, const LogEvent& event) = 0;
			
		};

		typedef std::shared_ptr<LogFormatter> Ptr;

		LogFormatter(const std::string &pattern);

		const std::string& getPattern() { return pattern_; }

		std::string format(LogLevel level, const LogEvent &event);
		void format(std::ostream &oss, LogLevel level, const LogEvent& event);

		void init();

	private:
		std::vector<FormatItem::Ptr> format_items_;  //
		std::string pattern_;                        //日志格式
		bool error_;                                 //是否有错误
		
};

class LogAppender
{
	public:
		typedef std::shared_ptr<LogAppender> Ptr;

		LogAppender();

		virtual void log(LogLevel level, const LogEvent& event) = 0;

		virtual ~LogAppender() {};

	protected:
		LogLevel          level_ = LogLevel::DEBUG;
		LogFormatter::Ptr formatter_;

};

class Logger
{
	public:
		typedef std::shared_ptr<Logger> Ptr;
		Logger(const std::string &name = "root");

		void log(LogLevel level, const LogEvent& event);

		void debug(const LogEvent &event);
		void info(const LogEvent &event);
		void warn(const LogEvent &event);
		void error(const LogEvent &event);
		void fatal(const LogEvent &event);

		void addAppender(LogAppender::Ptr appender);
		void delAppender(LogAppender::Ptr appender);

		LogLevel getLevel();
		void setLevel(LogLevel level);

		const std::string& getName() const { return name_; }

	private:
		LogLevel     level_;               //日志最小级别
		std::string  name_;                //日志名称
		std::list<LogAppender::Ptr> appenders_; //日志输出地集合
};


class FileAppender : public LogAppender
{
	public:
		typedef std::shared_ptr<FileAppender> Ptr;

		void log(LogLevel level, const LogEvent& event) override;

		FileAppender(const std::string &file_name);

	private:
		std::string    file_name_;
		std::ofstream  file_stream_;

};


class StdOutAppender : public LogAppender
{
	public:
		typedef std::shared_ptr<StdOutAppender> Ptr;

		void log(LogLevel level, const LogEvent& event) override;
};

class LoggerManager : public cpan::Singleton<LoggerManager>
{
	public:
		LoggerManager();

		Logger::Ptr getLogger(const std::string &name);

		Logger::Ptr getRoot();

	private:
		std::map<std::string, Logger::Ptr> loggers_;

		cpan::Mutex mutex_;

		Logger::Ptr root_logger_;
};

}

#endif
