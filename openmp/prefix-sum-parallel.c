/* @file	prefix-sum-parallel.c
 * @author	Robin Vermes
 * @author 	Valentin Laurent
 * @details	theoretical n*log(n) prefix-sum
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <omp.h>

void display(int * t, int length_t)
{
	int i = 0;

	for (; i < length_t; ++i)
	{
		printf("%d ", t[i]);
	}
	printf("\n");
}

void prefix_sum(int* T, int n)
{
	if (n > 1)
	{
		int i;
		int j;
		int l;
		if (n==2)
		{
			T[1] += T[0];
		}
		else
		{
			#pragma omp parallel num_threads(n) private(i)
			for(i=2; i <= n; i*=2)
			{
				if ((omp_get_thread_num() % i) == (i - 1))
				{
					T[omp_get_thread_num()] += T[omp_get_thread_num() - i/2];
				}
				#pragma omp barrier
			}

			for(i=i/2; i >1; i/=2)
			{
				if ((omp_get_thread_num() % i) == (i/2 - 1))
				{
					if (!(omp_get_thread_num() < (i/2 - 1)))
					{
						T[omp_get_thread_num()] += T[omp_get_thread_num() - i/2];
					}
				}
				#pragma omp barrier
			}
		}
	}
}


int main(int argc, char const *argv[])
{
	/*ARGUMENT CHECKING*/
	if (argc < 2)
	{
		printf("USAGE : %s <N>\n", argv[0]);
		exit(1);
	}

	/*INITIALIZATION*/
	/*sizes of table*/
	int n = atoi(argv[1]);

	/*tables creation*/
	int * tab = (int *) malloc ((n+1)*sizeof(int));

	/*indices*/
	int i = 0;

	double start;
	double end;

	/*initialize rng*/
	srand (time(NULL));

	/*initialization of tables*/
	for (; i < n; ++i)
	{
		tab[i] = rand()%10;
	}
	tab[n+1] = 0;

	display(tab, n);
	printf("start SP2 with n = %d\n", n);

	start = omp_get_wtime();

    prefix_sum(tab, n);

	end = omp_get_wtime();

	/*DISPLAY*/
    display(tab, n);


	printf("user time used for SP2 : %f\n", end - start);
	printf("-----------------------------------------------------------------\n");



	/*FREE RESOURCES*/
	free(tab);

	return 0;
}
