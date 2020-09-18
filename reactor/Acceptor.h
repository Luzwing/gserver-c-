#ifndef CPAN_ACCEPTOR_H
#define CPAN_ACCEPTOR_H

#include "../base/Address.h"
#include "../base/Socket.h"
#include "EventLoop.h"
#include "Channel.h"

namespace cpan
{

class Acceptor
{
	public:
		typedef std::function<void(Socket::Ptr peerAddr)> NewConnectionCallback;

		Acceptor(EventLoop* loop, const Address::Ptr addr);

		void setNewConnectionCallback(const NewConnectionCallback& cb);

		void listen();
		bool isListening() { return listening_; }

	private:
		void handleRead();

		EventLoop* loop_;
		//EventLoop::Ptr loop_;
		Socket acceptsocket_;
		Channel acceptchannel_;
		NewConnectionCallback newconnectioncb_;
		bool listening_;
};

}
#endif
