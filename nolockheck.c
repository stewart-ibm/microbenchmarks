/*
 *
 * GPL v2
 * Written by Stewart Smith
 * (C) IBM 2015
 */

#include <pthread.h>
#include <time.h>
#include <stdio.h>


int main()
{
	int i;
	time_t start, end;
	int j = 0;

	start = time(NULL);

	for(i = 0; i < 400000000; i++) {
		asm volatile ("" : : : "memory");
		j++;
	}

	end = time(NULL);

	printf("Took %d seconds\n", end - start);

	return 0;
}
