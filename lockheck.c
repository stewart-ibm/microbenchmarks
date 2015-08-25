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
	pthread_mutex_t m;
	int i;
	time_t start, end;
	int j = 0;

	pthread_mutex_init(&m, NULL);

	start = time(NULL);

	for(i = 0; i < 400000000; i++) {
		pthread_mutex_lock(&m);
		j++;
		pthread_mutex_unlock(&m);
	}

	end = time(NULL);

	printf("Took %d seconds\n", end - start);

	return 0;
}
