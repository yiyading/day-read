#include<stdio.h>
#include<stdarg.h>
#include<time.h>
#include<inttypes.h>

int main()
{
	printf("\tcycle\t\ttime\t instret\t counter exm!i\n");
	
	unsigned long cycle_start, cycle_end;
	unsigned long time_start,  time_end;
	unsigned long instret_start, instret_end;
	int tmp = 0;

	asm volatile("rdcycle %0" : "=r"(cycle_start));
	// unsigned long c1 = cycle_start + time_start;
	// unsigned long c11 = cycle_start + time_start;
	// unsigned long c111 = cycle_start + time_start;
	tmp += 1;
	asm volatile("rdcycle %0" : "=r"(cycle_end));

	asm volatile("rdtime %0" : "=r"(time_start));
	// unsigned long c2 = cycle_start + time_start;
	// unsigned long c22 = cycle_start + time_start;
	// unsigned long c222 = cycle_start + time_start;
	tmp += 1;
	asm volatile("rdtime %0" : "=r"(time_end));
	
	asm volatile("rdinstret %0" : "=r"(instret_start));
	// unsigned long c3 = cycle_start + time_start;
	// unsigned long c33 = cycle_start + time_start;
	// unsigned long c333 = cycle_start + time_start;
	tmp += 1;
	asm volatile("rdinstret %0" :  "=r"(instret_end));

	printf("end:   %lu, %lu, %lu\r\n", cycle_end, time_end, instret_end);
	printf("start: %lu, %lu, %lu\r\n", cycle_start, time_start, instret_start);
	printf("time:  %lu, %lu, %lu\r\n", cycle_end - cycle_start, time_end - time_start, instret_end - instret_start);
	return 0;
}
