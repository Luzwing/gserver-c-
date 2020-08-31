#ifndef CPAN_MUTEX_H
#define CPAN_MUTEX_H

#include <pthread.h>
#include "Noncopyable.h"

namespace cpan
{

class Mutex :Noncopyable
{
	public:
		Mutex();
		~Mutex();

		void lock();
		void unlock();
	
		pthread_mutex_t* getPthreadMutex();
	private:
		pthread_mutex_t mutex_;
};

class MutexGuardLock
{
	public:
		MutexGuardLock(Mutex &);
		~MutexGuardLock();
	private:
		Mutex& mutex_;
		bool islocked_;
};

}
#endif
