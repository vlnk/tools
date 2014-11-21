void TD_afficher(int * T, int i, int j)
{
	MPI_Status status;
	int k, err, x;

	for (k = i; k <= j; k++) {
		MPI_Win_fence((MPI_MODE_NOPUT | MPI_MODE_NOPRECEDE), win);

		if (!rank) {
			TD_get(T, k, &x);
		}

		MPI_Win_fence(MPI_MODE_NOSUCCEED, win);

		if (!rank) printf("%d ", x);
	}
	err = MPI_Barrier(MPI_COMM_WORLD);
}