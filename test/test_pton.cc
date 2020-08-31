#include <iostream>
#include <bitset>
#include <arpa/inet.h>
#include <string.h>

int main()
{
	char buf[128];
	memset(buf, 0, sizeof(buf));
	inet_pton(AF_INET, "255.255.255.0", buf);
	for (int i = 0; i < 128; ++i)
	{

		std::cout << std::bitset<8>(buf[i]) << " ";
	}

	std::cout << std::endl;
	return 0;

}
