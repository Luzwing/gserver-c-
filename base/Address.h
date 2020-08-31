#ifndef CPAN_ADDRESS_H
#define CPAN_ADDRESS_H

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>

namespace cpan
{

class Address
{
	public:
		typedef std::shared_ptr<Address> Ptr;
		virtual ~Address() {};

		virtual const sockaddr* getAddr() const = 0;
		virtual sockaddr* getAddr() = 0;
		virtual socklen_t getAddrLen() const = 0;

		int getFamily() const;

		virtual std::ostream& insert(std::ostream& os) const = 0;
		std::string toString() const;

		bool operator<(const Address& addr) const;
		bool operator==(const Address& addr) const;
		bool operator!=(const Address& addr) const;

		static Address::Ptr Create(const sockaddr* address, socklen_t len);

		static bool Lookup(std::vector<Address::Ptr>& addrs, const std::string& host,
				int family = AF_INET, int type = 0, int protocol = 0);
};

class IPAddress : public Address
{
	public:
		typedef std::shared_ptr<IPAddress> Ptr;
		virtual ~IPAddress() {};

		virtual IPAddress::Ptr broadcastAddress(uint32_t prefix_len) = 0;
		virtual IPAddress::Ptr networkAddress(uint32_t prefix_len) = 0;
		virtual IPAddress::Ptr subnetMask(uint32_t prefix_len) = 0;

		virtual uint16_t getPort() const = 0;
		virtual void setPort(uint16_t) = 0;

		static IPAddress::Ptr Create(const char* address, uint16_t port);
};


class IPv4Address : public IPAddress
{
	public:
		typedef std::shared_ptr<IPv4Address> Ptr;
		IPv4Address(const sockaddr_in& addr);
		IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);
		
		const sockaddr* getAddr() const override;
		sockaddr* getAddr() override;
		socklen_t getAddrLen() const override;
		std::ostream& insert(std::ostream& os) const override;

		IPAddress::Ptr broadcastAddress(uint32_t prefix_len) override;
		IPAddress::Ptr networkAddress(uint32_t prefix_len) override;
		IPAddress::Ptr subnetMask(uint32_t prefix_len) override;

		uint16_t getPort() const override;
		void setPort(uint16_t) override;

		static IPv4Address::Ptr Create(const char* address, uint16_t port);
	private:
		sockaddr_in    addr_;
};


class IPv6Address : public IPAddress
{
	public:
		typedef std::shared_ptr<IPv6Address> Ptr;
		IPv6Address();
		IPv6Address(const sockaddr_in6& addr);
		IPv6Address(const uint8_t address[16], uint16_t port = 0);

		const sockaddr* getAddr() const override;
		sockaddr* getAddr() override;
		socklen_t getAddrLen() const override;
		std::ostream& insert(std::ostream& os) const override;

		IPAddress::Ptr broadcastAddress(uint32_t prefix_len) override;
		IPAddress::Ptr networkAddress(uint32_t prefix_len) override;
		IPAddress::Ptr subnetMask(uint32_t prefix_len) override;

		uint16_t getPort() const override;
		void setPort(uint16_t) override;

		static IPv6Address::Ptr Create(const char* address, uint16_t port);
	private:
		sockaddr_in6   addr_;
};


class UnixAddress : public Address
{
	public:
		typedef std::shared_ptr<UnixAddress> Ptr;
		UnixAddress();
		UnixAddress(const std::string& path);

		const sockaddr* getAddr() const override;
		sockaddr* getAddr() override;
		socklen_t getAddrLen() const override;
		void setAddrLen(uint32_t len);

		//sockaddr* getAddr() override;
		//void setAddrLen(uint32_t len) override;

		std::ostream& insert(std::ostream& os) const override;

	private:
		sockaddr_un    addr_;
		size_t         length_;
};


}

#endif
