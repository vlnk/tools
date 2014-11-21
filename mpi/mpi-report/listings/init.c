int * TD_init()
{
	int *T, i, size_of_int, err;
	int size = last - first + 1;

	MPI_Type_size(MPI_INT, &size_of_int);
	MPI_Alloc_mem(size_of_int*size, MPI_INFO_NULL, &T);
	MPI_Win_create(T, size*size_of_int, size_of_int, MPI_INFO_NULL,MPI_COMM_WORLD, &win);

	srand (time(NULL)*rank);

	for (i = 0; i < size; ++i)
	{
		T[i] = rand()%(2*n);
	}

	return T;
}