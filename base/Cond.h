#ifndef CPAN_COND_H
#define CPAN_COND_H

#include <pthread.h>
#include "Mutex.h"

namespace cpan
{

class Condition : Noncopyable
{

	public:
		Condition(cpan::Mutex &mutex);
		~Condition();

		void notify();

		void notifyAll();
		
		void wait();

	private:
		pthread_cond_t   cond_;
		cpan::Mutex&     mutex_;
};

}
#endif
