#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../base/Endian.h"


using namespace std;


int main()
{
	//cout << inet_ntoa(cpan::byteSwapOnLittleEndian<uint32_t>(16777215)) << endl;

	uint32_t l1;
	l1 = inet_addr("255.255.255.0");
	cout << l1 << " " << cpan::byteSwapOnLittleEndian<uint32_t>(l1) << " " << cpan::byteSwapOnBigEndian<uint32_t>(l1) << endl;
	struct in_addr addr1;
	memcpy(&addr1, &l1, 4);
	cout << inet_ntoa(addr1) << endl;
	return 0;

}
