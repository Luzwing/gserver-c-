#ifndef CPAN_THREAD_H
#define CPAN_THREAD_H

#include <string>
#include <memory>
#include <pthread.h>
#include "Noncopyable.h"
#include "util.h"

namespace cpan
{

class Thread : Noncopyable
{
	public:
		typedef std::shared_ptr<Thread> Thread_Ptr;
		typedef std::function<void ()> ThreadFunc;

		Thread(ThreadFunc, const std::string name = std::string());
		~Thread();

		void start();

		void join();

		std::string getName();

		static std::string getCurName();

	private:
		void setDefaultName();

		static void* runInThread(void*);

	
	private:
		pid_t          tid_;
		pthread_t      thread_;
		std::string    thread_name_;
		ThreadFunc     cb_;
		bool           started_;
		bool           joined_;

		//static AtomicInt32 thread_num_;

};




}



#endif

