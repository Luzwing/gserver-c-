#ifndef CPAN_UTIL_H
#define CPAN_UTIL_H

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/syscall.h>


#define SAFE_DELETE(ptr) \
	if (nullptr != ptr) \
		delete(ptr);

#define SAFE_FREE(ptr) \
	if (nullptr != ptr) \
		free(ptr);

#if defined __GNUC__
#define LIKELY(x)     __builtin_expect(x, 1)
#define UNLIKELY(x)   __builtin_expect(x, 0)
#else
#define LIKELY(x)   x
#define UNLIKELY(x) x
#endif


namespace cpan
{
void Assert(bool status);

void Backtrace(std::vector<std::string> &res, int size);

std::string Backtrace(const std::string &skip, int size);

pid_t getCurrentTid();

}
#endif
