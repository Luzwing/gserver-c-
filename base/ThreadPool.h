#ifndef CPAN_THREADPOOL_H
#define CPAN_THREADPOOL_H

#include <vector>
#include <memory>
#include <deque>
#include "Noncopyable.h"
#include "Thread.h"
#include "Cond.h"

namespace cpan
{

class ThreadPool : Noncopyable
{
	public:
		typedef std::function<void ()> Task;

		void start(size_t numthreads);

		void stop();

		void run(Task task);

		void setThreadInitCb(Task initCb)
		{
			threadInitCallback_ = initCb;
		}

		size_t queueSize();
		void setQueueSize(size_t queuesize);

		ThreadPool(const std::string& name);
		~ThreadPool();


	private:
		Task take();

		void runInThread();

	private:
		std::vector<std::unique_ptr<Thread>> threads_;
		std::deque<Task>          tasks_;
		mutable cpan::Mutex       mutex_;
		cpan::Condition           notempty_;
		cpan::Condition           notfull_;
		size_t                    maxtasksize_; 
		std::string               name_;
		Task                      threadInitCallback_;
		bool                      running_;

};

}

#endif
