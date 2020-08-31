#include "Cond.h"

namespace cpan
{
	Condition::Condition(Mutex &mutex) : mutex_(mutex)
	{
		pthread_cond_init(&cond_, NULL);
	}


	Condition::~Condition()
	{
		pthread_cond_destroy(&cond_);
	}
	
	void Condition::wait()
	{
		mutex_.lock();
		pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
	}

	void Condition::notify()
	{
		pthread_cond_signal(&cond_);
	}

	void Condition::notifyAll()
	{
		pthread_cond_broadcast(&cond_);
	}

}
