#ifndef _SHMDATA_H_HEADER
#define _SHMDATA_H_HEADER

#define TEXT_SZ 2048

struct shared_use_st
{
	int written; // 作为一个标志，非0：表示可读；0：表示可写
	char text[TEXT_SZ];
};

#endif
