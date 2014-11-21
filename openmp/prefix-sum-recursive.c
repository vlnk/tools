/* @file	prefix-sum-recursive.c
* @author	Robin Vermes
* @author 	Valentin Laurent
* @details	theoretical log^2(n) prefix-sum
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
		int * S = (int*) malloc ((n/2+1)*sizeof(int));
		int i = 0;
		S[n/2] = 0;

		#pragma omp parallel for
		for (i = 0; i < n/2; ++i)
		{
			S[i] = T[2*i] + T[2*i + 1];
		}

		#pragma omp task
		{
			prefix_sum(S, n/2);
		}
		#pragma omp taskwait

		#pragma omp parallel for
		for (i = 0; i < n/2; ++i)
		{
			T[i*2 + 1] = S[i];
			T[i*2 + 2] = S[i] + T[i*2 + 2];
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
		tab[i] = (int)rand()%10;
	}
	tab[n+1] = 0;

	start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
    	    prefix_sum(tab, n);
        }
    }

	end = omp_get_wtime();

	printf("start SP1 with n = %d\n", n);
	printf("user time used for SP1 : %f\n", end - start);
	printf("-----------------------------------------------------------------\n");

	/*FREE RESOURCES*/
	free(tab);

	return 0;
}
