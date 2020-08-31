#include <iostream>
#include "../base/Endian.h"

using namespace std;


int main()
{
#if BYTE_ORDER == BIG_ENDIAN
	cout << "大端" << endl;
#endif

	cout << cpan::byteSwapOnBigEndian<int>(10) << endl;
	cout << cpan::byteSwapOnLittleEndian<int>(10) << endl;
	return 0;	
}
