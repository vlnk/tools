#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>

void display(int * t, int size)
{
	int i = 0;

	for (; i < size; ++i)
	{
		printf("%d => %d\n", i, t[i]);
	}
}

double get_time()
{
	struct rusage ru;
	struct timeval timer;

	getrusage(RUSAGE_SELF, &ru);
	timer = ru.ru_utime;

	return (double)timer.tv_sec + (double)timer.tv_usec / 1000000.0;
}

int * merge(int * u, int * t, int size)
{
	int i = 0;
	int j = size;
	int k = 0;

	for (; k < size*2; ++k)
	{
		if (u[i] < u[j])
		{
			t[k] = u[i];
			if (i==size-1) i = 2*size;
			else i += 1;
		}
		else
		{
			t[k] = u[j];
			j += 1;
		}
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
	int * tab = (int *) malloc ((2*n+1)*sizeof(int));
	tab[2*n] = (int) INFINITY;

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
	int * tab_sorted = (int*) malloc (2*n*sizeof(int));
	double start = get_time();

	tab_sorted = merge(tab, tab_sorted, n);

	printf("user time used for sequential merging : %f\n", get_time() - start);

	/*FREE RESOURCES*/
	free(tab_sorted);
	free(tab);

	return 0;
}
