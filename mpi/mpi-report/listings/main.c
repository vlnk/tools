int 		n; 		/*taille du tableau*/
int 		rank;	/*rang du processus*/
int 		p;		/*nombre de processus*/

int main(int argc, char **argv)
{
	/* mpi initialization */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &p);
	
	n = atoi(argv[1]);
	int *T = NULL;
	int choice = -1;

	T = TD_init(); /*initialisation du tableau*/

	while (choice != 0) { /*entrer du choix*/
		if (!rank) {
			welcome(); printf("veuillez saisir un choix : ");
			scanf("%d*",&choice);
		}

		switch (choice) {
			case 1: /*re-initialisation*/
			case 2: /*get*/
			case 3: /*put*/
			case 4: /*somme*/
			case 5: /*tri*/
			case 6: /*affichage*/
			case 7: /*info*/
			default: /*exit*/
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if (T) { /*libération de la mémoire*/
		MPI_Win_free(&win);
		MPI_Free_mem(T);
	}

	MPI_Finalize(); /*fin du programme mpi*/
	return 0;