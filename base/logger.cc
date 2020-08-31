#include <algorithm>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "logger.h"


namespace cpan
{

class StringFormatItem : public LogFormatter::FormatItem
{
	public:
		StringFormatItem(const std::string& str) : str_(str) {}
		void format(std::ostream& oss, LogLevel level, const LogEvent& event)
		{
			oss << str_;
		}
	private:
		std::string str_;
};

class MessageFormatItem : public LogFormatter::FormatItem
{
	public:
		MessageFormatItem(const std::string& str) {}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			oss << event.getContents();
		}
};

class DateTimeFormatItem : public LogFormatter::FormatItem
{
	public:
		DateTimeFormatItem(const std::string& fmt = "%Y-%m-%d %H:%M:%S") : fmt_(fmt) 
		{
			if (fmt_.empty())
			{
				fmt_ = "%Y-%m-%d %H:%M:%S";
			}
		}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			struct tm tm;
			time_t timestamp = event.getTimestamp();
			localtime_r(&timestamp, &tm);
			char timestr[32];
			strftime(timestr, sizeof(timestr), fmt_.c_str(), &tm);
			oss << timestr;
		}
	private:
		std::string fmt_;
};

class ThreadIdFormatItem : public LogFormatter::FormatItem
{
	public:
		ThreadIdFormatItem(const std::string& str) {}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			oss << event.getThreadId();
		}

};

class ThreadNameFormatItem : public LogFormatter::FormatItem
{
	public:
		ThreadNameFormatItem(const std::string& str) {}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			oss << event.getThreadName();
		}
};

class LineFormatItem : public LogFormatter::FormatItem
{
	public:
		LineFormatItem(const std::string& str) {}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			oss << event.getLine();
		}
};

class FileFormatItem : public LogFormatter::FormatItem
{
	public:
		FileFormatItem(const std::string& str) {}
		void format(std::ostream &oss, LogLevel level, const LogEvent& event)
		{
			oss << event.getFile();
		}
};

class TableFomatItem : public LogFormatter::FormatItem
{
	public:
		TableFomatItem(const std::string& str) {}
		void format(std::ostream& oss, LogLevel level, const LogEvent& event)
		{
			oss << "\t";
		}
};

class LinefeedFormatItem : public LogFormatter::FormatItem
{
	public:
		LinefeedFormatItem(const std::string& str) {}
		void format(std::ostream& oss, LogLevel level, const LogEvent& event)
		{
			oss << "\n";
		}
};

class LevelFormatItem : public LogFormatter::FormatItem
{
	public:
		LevelFormatItem(const std::string& str) {}
		void format(std::ostream& oss, LogLevel level, const LogEvent& event)
		{
			oss << cpan::ToString(level);
		}
};

LogFormatter::LogFormatter(const std::string& pattern) : pattern_(pattern)
{
	init();
}

void LogFormatter::init()
{
	std::vector<std::tuple<std::string, std::string, int> > vec;
	std::string nstr;
	for (size_t i = 0; i < pattern_.size(); ++i)
	{
		if (pattern_[i] != '%')
		{
			nstr.append(1, pattern_[i]);
			continue;
		}
		if (i+1 < pattern_.size() && pattern_[i+1] == '%')
		{
			nstr.append(1, '%');
			continue;
		}

		size_t next = i+1;
		int fmt_status = 0;
		size_t fmt_begin = i+1;

		std::string str, fmt;
		while (next < pattern_.size())
		{
			if (!fmt_status && (!isalpha(pattern_[next]) && pattern_[next] != '{' && pattern_[next] != '}'))
			{
				str = pattern_.substr(i+1, next-i-1);
				break;
			}

			if (fmt_status == 0)
			{
				if (pattern_[next] == '{')
				{
					str = pattern_.substr(i+1, next-i-1);
					fmt_status = 1;
					fmt_begin = next;
					++next;
					continue;
				}
			}
			else if (fmt_status == 1)
			{
				if (pattern_[next] == '}')
				{
					fmt_status = 0;
					fmt = pattern_.substr(fmt_begin+1, next - fmt_begin-1);
					++next;
					break;
				}
			}
			++next;

			if (next >= pattern_.size())
			{
				if (str.empty())
				{
					str = pattern_.substr(i+1);
				}
			}
		}

		if (fmt_status == 0)
		{
			if (!nstr.empty())
			{
				vec.push_back(std::make_tuple(nstr, std::string(), 0));
				nstr.clear();
			}
			if (!str.empty())
			{
				vec.push_back(std::make_tuple(str, fmt, 1));
			}
			i = next - 1;
		}
		else if (fmt_status == 1)
		{
			error_ = true;
			vec.push_back(std::make_tuple(std::string("<<error parse>>"), fmt, 0));
		}
	}
	if (!nstr.empty())
	{
		vec.push_back(std::make_tuple(nstr, std::string(), 0));
	}

	std::map<std::string, std::function<FormatItem::Ptr(const std::string&)>> s_format_items = {
#define ITEMDEFINE(str, item) \
		{#str, [](const std::string& fmt){ return FormatItem::Ptr(new item(fmt)); }}	

		ITEMDEFINE(m, MessageFormatItem),
		ITEMDEFINE(d, DateTimeFormatItem),
		ITEMDEFINE(p, LevelFormatItem),
		ITEMDEFINE(T, TableFomatItem),
		ITEMDEFINE(n, LinefeedFormatItem),
		ITEMDEFINE(t, ThreadIdFormatItem),
		ITEMDEFINE(f, FileFormatItem),
		ITEMDEFINE(l, LineFormatItem),
		ITEMDEFINE(N, ThreadNameFormatItem)

#undef ITEMDEFINE
	};
	
	for (auto &it : vec)
	{
		if (std::get<2>(it) == 0)
		{
			std::string fmt_str = std::get<0>(it);
			format_items_.push_back(FormatItem::Ptr(new StringFormatItem(fmt_str)));
		}
		else
		{
			std::string fmt_str = std::get<0>(it),
				fmt_fmt = std::get<1>(it);

			auto iter = s_format_items.find(fmt_str);
			if (iter == s_format_items.end())
			{
				error_ = true;
				format_items_.push_back(FormatItem::Ptr(new StringFormatItem(fmt_str)));
			}
			else
			{
				auto func = iter->second;
				format_items_.push_back(func(fmt_fmt));
			}
		}
	}
}

void LogFormatter::format(std::ostream& oss, LogLevel level, const LogEvent& event)
{
	for (auto &item : format_items_)
	{
		item->format(oss, level, event);
	}
}

std::string LogFormatter::format(LogLevel level, const LogEvent& event)
{
	std::ostringstream os;
	this->format(os, level, event);
	return os.str();
}

LogAppender::LogAppender()
{
	formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%p%T%f%T%l%T%t%T%m%n"));
}

Logger::Logger(const std::string &name) : level_(LogLevel::DEBUG), name_(name)
{
}

void Logger::setLevel(LogLevel level)
{
	if (level < LogLevel::DEBUG || level > LogLevel::FATAL)
	{
		std::cout << "Set Level Error, Error Level: " << level <<  std::endl;
		exit(0);
	}
	level_ = level;
}

LogLevel Logger::getLevel()
{
	return level_;
}

void Logger::log(LogLevel level, const LogEvent& event)
{
	for (auto it : appenders_)
	{
		if (it)
		{
			it->log(level, event);
		}
	}
}

void Logger::debug(const LogEvent &event)
{
	this->log(LogLevel::DEBUG, event);
}

void Logger::info(const LogEvent &event)
{
	this->log(LogLevel::INFO, event);
}

void Logger::warn(const LogEvent &event)
{
	this->log(LogLevel::WARN, event);
}

void Logger::error(const LogEvent &event)
{
	this->log(LogLevel::ERROR, event);
}

void Logger::fatal(const LogEvent &event)
{
	this->log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::Ptr appender)
{
	appenders_.push_back(std::move(appender));
}

void Logger::delAppender(LogAppender::Ptr appender)
{
	std::remove(appenders_.begin(), appenders_.end(), appender);
}

FileAppender::FileAppender(const std::string &file_name) :
	file_name_(file_name)
{
	file_stream_.open(file_name, std::ios::out | std::ios::app);
}

void FileAppender::log(LogLevel level, const LogEvent& event)
{
	if (level >= level_)
	{
		//file_stream_ << formatter_->format(level, event);	
		formatter_->format(file_stream_, level, event);
	}
}

void StdOutAppender::log(LogLevel level, const LogEvent& event)
{
	if (level >= level_)
	{
		//std::cout << formatter_->format(level, event);	
		formatter_->format(std::cout, level, event);
	}
}

LogEvent::LogEvent(Logger::Ptr logger, LogLevel level, const char* file, uint32_t line, uint32_t threadId, const std::string& threadName, uint64_t timestamp) : logger_(logger), file_(file), line_(line), threadId_(threadId), threadName_(threadName), timestamp_(timestamp), level_(level) {}


LogEventWrap::~LogEventWrap()
{
	event_->getLogger()->log(event_->getLevel(), *event_);
}

LoggerManager::LoggerManager()
{
	root_logger_.reset(new Logger);
	root_logger_->addAppender(LogAppender::Ptr(new StdOutAppender));
	root_logger_->addAppender(LogAppender::Ptr(new FileAppender("~/log/root.xml")));

	loggers_[root_logger_->getName()] = root_logger_;
}

Logger::Ptr LoggerManager::getLogger(const std::string &name)
{
	auto iter = loggers_.find(name);
	if (iter != loggers_.end())
		return iter->second;

	Logger::Ptr logger(new Logger(name));
	loggers_[name] = logger;
	return logger;
}

Logger::Ptr LoggerManager::getRoot()
{
	return root_logger_;
}


struct LogInit
{
	LogInit()
	{
		
	}

};

}
