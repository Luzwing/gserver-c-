#include "Mutex.h"

namespace cpan
{

Mutex::Mutex()
{
	pthread_mutex_init(&mutex_, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex_);
}

void Mutex::lock()
{
	pthread_mutex_lock(&mutex_);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t* Mutex::getPthreadMutex()
{
	return &mutex_;
}

MutexGuardLock::MutexGuardLock(Mutex &mutex) : mutex_(mutex)
{
	mutex_.lock();
	islocked_ = true;
}

MutexGuardLock::~MutexGuardLock()
{
	if (islocked_)
	{
		mutex_.unlock();
	}
}

}
