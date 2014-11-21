int TD_sum(int * T)
{
	int count = 0;
	int result, i;

	for (i = first; i <= last; ++i) {
		count += T[i - first];
	}

	MPI_Reduce(&count, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	return result;
}