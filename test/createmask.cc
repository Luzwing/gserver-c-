#include <iostream>
#include <bitset>
#include <stdint.h>
#include "../base/Endian.h"

using namespace std;

int main()
{
	{
	int i = (1 << (sizeof(int) * 8 - 24)) - 1;
	cout  << bitset<sizeof(uint32_t)*8>(i)  << endl;;
	uint8_t *j = (uint8_t*)&i;
	for (int index = 0; index < 4; index++)
	{
		cout  << bitset<sizeof(uint8_t)*8>(j[index])  << endl;;
	}
	cout << i << endl;
	}

	int i = cpan::byteSwapOnLittleEndian<uint32_t>((1 << (sizeof(int) * 8 - 8)) - 1);
	uint8_t *j = (uint8_t*)&i;
	for (int index = 0; index < 4; index++)
	{
		cout  << bitset<sizeof(uint8_t)*8>(j[index])  << endl;;
	}
	cout << i << endl;
	return 0;
}
