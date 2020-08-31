#include "ThreadPool.h"
#include "util.h"

namespace cpan
{
	ThreadPool::ThreadPool(const std::string& name) :
		mutex_(),
		running_(false),
		name_(name),
		notempty_(mutex_),
		notfull_(mutex_),
		maxtasksize_(0)
	{}

	ThreadPool::~ThreadPool()
	{
		if (running_)
		{
			stop();
		}
	}

	void ThreadPool::start(size_t numthreads)
	{
		cpan::Assert(!running_);
		cpan::Assert(threads_.empty());
		cpan::Assert(numthreads != 0);

		running_ = true;
		for (int i = 0; i < numthreads; ++i)
		{
			char name[20];
			sprintf(name, "thread_%i", i+1);
			threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this),
						std::string(name)));
			threads_[i]->start();
		}
	}

	size_t ThreadPool::queueSize()
	{
		MutexGuardLock guard(mutex_);
		return maxtasksize_;
	}

	void ThreadPool::setQueueSize(size_t queuesize)
	{
		MutexGuardLock guard(mutex_);
		maxtasksize_ = queuesize;
	}

	void ThreadPool::stop()
	{
		cpan::Assert(running_);
		{
			MutexGuardLock guard(mutex_);
			running_ = false;
			notempty_.notifyAll();
			notfull_.notifyAll();
		}
		for (auto &thread : threads_)
		{
			thread->join();
		}
	}

	void ThreadPool::run(Task task)
	{
		if (!task) return;
		if (threads_.empty())
		{
			task();
		}
		else
		{
			MutexGuardLock guard(mutex_);
			while (running_ && tasks_.size() >= maxtasksize_)
			{
				notfull_.wait();
			}
			if (!running_)
				return;

			if (tasks_.size() <= maxtasksize_)
			{
				tasks_.push_back(std::move(task));
				notempty_.notify();
			}
		}
	}

	ThreadPool::Task ThreadPool::take()
	{
		MutexGuardLock guard(mutex_);
		while (running_ && tasks_.empty())
		{
			notempty_.wait();
		}
		Task task;
		if (!tasks_.empty())
		{
			task = tasks_.front();
			tasks_.pop_front();
			if (maxtasksize_ > 0)
			{
				notfull_.notify();
			}
		}
		return task;
	}

	void ThreadPool::runInThread()
	{
		if (threadInitCallback_)
		{
			threadInitCallback_();
		}
		while(running_)
		{
			Task task(take());
			if (task)
			{
				task();
			}
		}
	}

}
