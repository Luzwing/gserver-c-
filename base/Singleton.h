#ifndef CPAN_SINGLETON_H
#define CPAN_SINGLETON_H

#include <pthread.h>
#include <stdlib.h>
#include "Noncopyable.h"
#include "util.h"

namespace cpan
{

template <typename T>
class Singleton : Noncopyable
{
	public:
		static T* getInstance()
		{
			pthread_once(&once_, &Singleton::init);
			Assert(NULL != value_);
			return value_;
		}

		static void init()
		{
			if (NULL == value_)
			{
				value_ = new T();
				::atexit(destroy);
			}
		}

		static void destroy()
		{
			if (NULL != value_)
			{
				delete value_;
				value_ = NULL;
			}
		}
	
	private:
		static T*             value_; 
		static pthread_once_t once_;
};

template <typename T>
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}


#endif
