#include <iostream>


using namespace std;

int main()
{
	const char*  ch = NULL;

	const char* ch1 = "abcdefg";

	ch = ch1;
	
	const int* i1;
	const int i2 = 1;
	i1 = &i2;

	return 0;


}
