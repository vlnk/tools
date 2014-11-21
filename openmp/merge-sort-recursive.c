/* @file	prefix-sum-parallel.c
 * @author	Robin Vermes
 * @author 	Valentin Laurent
 * @details	recursive merge-sort with openmp
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void display(int * t, int length_t)
{
	int i = 0;

	for (; i < length_t; ++i)
	{
		printf("%d => %d\n", i, t[i]);
	}
}

void exchange(int * a, int * b)
{
	int temp = *b;
	*b = *a;
	*a = temp;
}

int binary_search(int x, int * array, int first, int last)
{
	int low = first;
	int high = fmax(first, last + 1);
	int mid = 0;

	while (low < high)
	{
		mid = (int) floor((low + high)/2.0);

		if (x <= array[mid])
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	}

	return high;
}

int * merge(int * u, int first_u, int last_u, int first_v, int last_v, int * t, int first_t, int n)
{
	int n1 = last_u - first_u + 1;
	int n2 = last_v - first_v + 1;

	if (n1 < n2)
	{
		exchange(&n1, &n2);
		exchange(&first_u, &first_v);
		exchange(&last_u, &last_v);
	}

	if (n1 != 0)
	{
		int mid_u = floor((first_u + last_u)/2);
		int mid_v = binary_search(u[mid_u], u, first_v, last_v);
		int mid_t = first_t + (mid_u - first_u) + (mid_v - first_v);

		t[mid_t] = u[mid_u];

        #pragma omp task shared(u, t) firstprivate(mid_u, last_u, mid_v, last_v, mid_t)
        merge(u, first_u, mid_u - 1, first_v, mid_v - 1, t, first_t, n);

        #pragma omp task shared(u, t) firstprivate(mid_u, last_u, mid_v, last_v, mid_t)
	    merge(u, mid_u + 1, last_u, mid_v, last_v, t, mid_t + 1, n);

		/*sync*/
        #pragma omp taskwait
	}

	return t;
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
	printf("----------------------------------\n");
	printf("start merging with n = %d\n", n);

	/*tables creation*/
	int * tab = (int *) malloc (2*n*sizeof(int));

	/*indices*/
	int i = 0;
	int i_even = 0;
	int i_odd = n;



	/*initialization of tables*/
	for (; i < 2*n; ++i)
	{
		if (i % 2 == 0)
		{
			tab[i_even] = i;
			i_even++;
		}
		else
		{
			tab[i_odd] = i;
			i_odd++;
		}
	}

	/*MERGE SORT*/
	int * tab_sorted = (int *) malloc (2*n*sizeof(int));

	double start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
    	    tab_sorted = merge(tab, 0, n-1, n, 2*n-1, tab_sorted, 0, n);

			printf("number of threads used : %d\n", omp_get_num_threads());
        }
    }

    printf("user time used for parallel merging : %f\n", omp_get_wtime() - start);

	/*DISPLAY*/
    display(tab_sorted, 2*n);

	/*FREE RESOURCES*/
	free(tab_sorted);
	free(tab);

	return 0;
}
