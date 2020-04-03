#include<stdio.h>
#include<stdlib.h>

void test_exit(int i){

	for( ; i<10; i++){
		printf("%d\n", i);

		// exit直接退出了进程		
		if(i==9)
			exit(0);
	}
}

int main(int argc, char *argv[]){

	test_exit(3);

	printf("return main\n");

	exit(0);
}
