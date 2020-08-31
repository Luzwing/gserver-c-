#ifndef CPAN_FDMANAGER_H
#define CPAN_FDMANAGER_H

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <memory>
#include <map>
#include "Singleton.h"
#include "Mutex.h"

namespace cpan
{

class FdCtl : public std::enable_shared_from_this<FdCtl>
{
	public:
		typedef std::shared_ptr<FdCtl> Ptr;
		FdCtl(int fd);
		~FdCtl();

	public:
		bool isInit() { return is_init_; }
		bool isSocket() { return is_socket_; }
		bool isNonblock() { return is_nonblock_; }
		bool isClosed() { return is_closed_; }
		uint64_t getRecvTimeout() { return recvTimeout_; }
		uint64_t getSendTimeout() { return sendTimeout_; }

		void setRecvTimeout(uint64_t timeout);
		void setSendTimeout(uint64_t timeout);

	private:
		// 是否初始化
		bool is_init_;
		// 是否是socket
		bool is_socket_;
		// 是否是非阻塞
		bool is_nonblock_;
		// 是否已经关闭
		bool is_closed_;
		// 文件描述符
		int  fd_;
		// 读超时时间 / 毫秒
		uint64_t recvTimeout_;
		// 写超时时间 / 毫秒
		uint64_t sendTimeout_;
};


class FdManager : public Singleton<FdManager>
{
	public:
		FdCtl::Ptr get(int fd, bool auto_create = false);

		void del(int fd);

	private:
		Mutex mutex_;
		std::map<int, FdCtl::Ptr> fds_;

};



}
#endif
