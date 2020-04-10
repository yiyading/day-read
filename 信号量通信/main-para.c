// 该程序用来测试main函数的参数功能

#include<stdio.h>

// argc表示传入main函数参数的个数
// argv是指针数组，指向每个传入main函数中的参数的
int main(int argc, char *argv[])
{
	int i = 0;

	printf("%d\n", argc);

	// 传入的第一个参数是执行文件的绝对路径
	for( ; i<argc; i++)
		printf("%s\n", *argv[i]);

	return 0;
}

