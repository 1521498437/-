#include "TestDef.h"
#ifdef CPP17_DEMO
#define ENTRY int main(int argc, char *argv[])
#else
#define ENTRY static int func(int argc, char *argv[])
#endif

#include <cstdio>

ENTRY
{
	printf("hello, C++17!\n");
	return 0;
}
