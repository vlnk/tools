void TD_get(int * T, int index, TYPE_T * get)
{
	int real_index, owner;

	if (index >= 0 && index < n) {
		owner = BLOCK_OWNER(index,p,n);

		if (rank == owner) { *get = T[index - first]; }
		else {
			real_index = index - FIRST(owner,p,n);
			MPI_Get(get, 1, MPI_INT, owner, real_index, 1, MPI_INT, win);
		}
	}
	else {
		*get = -1;
	}
}
