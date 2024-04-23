#include <stdio.h>

void test(void* buffer, int length)
{
	unsigned char* c = buffer;
	for (int i=0; i < length; ++i)
		printf("%c", *c++);
}

int main()
{
	char* a = "Roger";
	test(a, 5);
	return (0);
}
