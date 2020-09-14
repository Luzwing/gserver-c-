#include "Thread.h"

namespace cpan
{
	static thread_local Thread* thread_t = nullptr; 

	Thread::Thread(ThreadFunc func, const std::string name) :
		thread_(0),
		thread_name_(name),
		cb_(func),
		started_(false),
		joined_(false)
	{

		this->setDefaultName();
	}

	Thread::~Thread()
	{
		if (started_ && !joined_)
		{
			pthread_detach(thread_);
		}
	}

	void Thread::setDefaultName()
	{
		//thread_num_,increment();
		if (thread_name_.empty())
		{
			thread_name_ = "default name";
		}
	}

	void Thread::start()
	{
		if (!started_)
		{
			started_ = true;
			if (pthread_create(&thread_, NULL, &Thread::runInThread, this))
			{
				started_ = false;
				fprintf(stderr, "pthread_create error");
			}
			tid_ = cpan::getCurrentTid();
		}
	}

	void* Thread::runInThread(void* para)
	{
		Thread* thread = static_cast<Thread*>(para);
		thread_t = thread;
		ThreadFunc cb;
		cb.swap(thread->cb_);
		cb();
	}

	void Thread::join()
	{
		if (started_ && !joined_)
		{
			joined_ = true;
			pthread_join(thread_, NULL);
		}
	}

	std::string Thread::getName()
	{
		return thread_name_;
	}


	std::string Thread::getCurName()
	{
		if (thread_t)
		{
			return thread_t->getName();
		}
		return "UNKNOWN";
	}

	pid_t Thread::getCurThreadID()
	{
		if (thread_t)
			return thread_t->getThreadID();
		return 0;
	}

	pid_t Thread::getThreadID()
	{
		return this->tid_;
	}

}
