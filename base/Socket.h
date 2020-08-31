#ifndef CPAN_SOCKET_H
#define CPAN_SOCKET_H

#include "Address.h"
#include "Noncopyable.h"

#define INIT_STAT    1
#define BIND_STAT    2
#define CONNECT_STAT 4
#define CLOSE_STAT   8

namespace cpan
{

class Socket : public std::enable_shared_from_this<Socket>, public Noncopyable
{
	public:
		typedef std::shared_ptr<Socket> Ptr;
		typedef std::weak_ptr<Socket> WeakPtr;

		Socket(int family, int type, int protocol);
		~Socket();


	public:
		bool initFrom(int sock);
		bool bind(const Address::Ptr addr);
		bool listen(int backlog = SOMAXCONN);
		Socket::Ptr accept();
		bool connect(const Address::Ptr addr, uint64_t timeout_ms = -1);
		bool close();
		bool reconnect(uint64_t timeout_ms = -1);

	public:
		virtual int recv(char* buf, size_t length, int flags = 0);
		virtual int recv(struct iovec* vec, size_t length, int flags = 0);
		virtual int recvFrom(char* buf, size_t length, Address::Ptr addr, int flags = 0);
		virtual int recvFrom(struct iovec* vec, size_t length, Address::Ptr addr, int flags = 0);
		
		virtual int send(const char* buf, size_t length, int flags = 0);
		virtual int send(const struct iovec* vec, size_t length, int flags = 0);
		virtual int sendTo(const char* buf, size_t length, Address::Ptr addr, int flags = 0);
		virtual int sendTo(const struct iovec* vec, size_t length, Address::Ptr addr, int flags = 0);

	public:
		Socket::Ptr getSharedPtr() { return shared_from_this(); }

		int family() { return family_; }
		int type() { return type_; }
		int protocol() { return protocol_; }

		bool isValid() { return (isInited() && (sock_ != -1)); }
		bool isInited() { return (status_ & INIT_STAT) > 0 ? true : false; }
		bool isBinded() { return (status_ & BIND_STAT) > 0 ? true : false;  }
		bool isConnected() { return (status_ & CONNECT_STAT) > 0 ? true : false; }
		bool isClosed() { return (status_ & CLOSE_STAT) > 0 ? true : false; }
		void setInited(bool);
		void setBinded(bool);
		void setConnected(bool);
		void setClosed(bool);

	public:
		Address::Ptr getRemoteAddress();
		Address::Ptr getLocalAddress();

	private:
		int sock_;
		int family_;
		int type_;
		int protocol_;
		int status_;

		Address::Ptr localAddress_;
		Address::Ptr remoteAddress_;

};













}
#endif
