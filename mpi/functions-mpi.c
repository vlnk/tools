/* @file	functions-mpi.c
 * @author	Robin Vermes
 * @author 	Valentin Laurent
 * @details	some functions for mpi
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** MACROS **/
#define TYPE_T int

#define ASSERT(A, B) \
		if(A) printf("error in function : %s with error code : %d\n", B, A);

#define FIRST(rank,p,n) \
		(rank*n/p)

#define LAST(rank,p,n) \
		(FIRST((rank+1),p,n)-1)

#define BLOCK_OWNER(index,p,n) \
		(((p)*(index+1)-1)/(n))

#define MIN(a,b) ((a<b)? a : b)
#define MAX(a,b) ((a>b)? a : b)

/** GLOBAL VALUES **/
int 		n; 		/*taille du tableau*/
int 		rank;	/*rang du processus*/
int 		p;		/*nombre de processus*/
MPI_Win 	win;	/*fenêtre RMA*/
int 		first;	/*premier élement RMA*/
int 		last;	/*dernier élement RMA*/
int 		already_split = -1;

/** BASIC FUNCTIONS **/
int * TD_init()
{
	int *T, i, size_of_int, err;
	int size = last - first + 1;

	MPI_Type_size(MPI_INT, &size_of_int);
	MPI_Alloc_mem(size_of_int*size, MPI_INFO_NULL, &T);
	MPI_Win_create(T, size*size_of_int, size_of_int, MPI_INFO_NULL,MPI_COMM_WORLD, &win);

	srand (time(NULL)*(rank+1));

	for (i = 0; i < size; ++i)
	{
		T[i] = rand()%(2*n);
	}

	return T;
}

void TD_get(int * T, int index, TYPE_T * get)
{
	int local_index, owner;

	if (index >= 0 && index < n)
	{
		owner = BLOCK_OWNER(index,p,n);

		if (rank == owner) { *get = T[index - first]; }
		else
		{
			local_index = index - FIRST(owner,p,n);
			MPI_Get(get, 1, MPI_INT, owner, local_index, 1, MPI_INT, win);
		}
	}
	else
	{
		*get = -1;
	}
}

int TD_put(int * T, int index, int put)
{
	int err = 0;
	int local_index, owner;

	if (index >= 0 && index < n)
	{
		err = 0;
		owner = BLOCK_OWNER(index,p,n);

		if (rank == owner){ T[index - first] = put; }
		else
		{
			local_index = index - FIRST(owner,p,n);
			MPI_Put(&put, 1, MPI_INT, owner, local_index, 1, MPI_INT, win);
		}
	}
	else
	{
		err = 1;
	}

	return err;
}

int TD_sum(TYPE_T * T)
{
	int count = 0;
	int result, i;

	for (i = first; i <= last; ++i)
	{
		count += T[i - first];
	}

	MPI_Reduce(&count, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	return result;
}

void TD_afficher(TYPE_T * T, int i, int j)
{
	MPI_Status status;
	int k, err;
	TYPE_T x;

	for (k = i; k <= j; k++)
	{
		MPI_Win_fence((MPI_MODE_NOPUT | MPI_MODE_NOPRECEDE), win);

		if (!rank)
		{
			TD_get(T, k, &x);
		}

		MPI_Win_fence(MPI_MODE_NOSUCCEED, win);

		if (!rank)
		{
			printf("%d ", x);
		}
	}

	err = MPI_Barrier(MPI_COMM_WORLD);
	ASSERT(err, "MPI_Affichage")
}

void welcome()
{
	printf("** menu **\n");
	printf("[0] quitter le programme\n");
	printf("[1] réinitialisation du tableau\n");
	printf("[2] obtenir une valeur\n");
	printf("[3] remplacer une valeur\n");
	printf("[4] sommer les éléments du tableau\n");
	printf("[5] trier le tableau\n");
	printf("[6] afficher le tableau entre les indices i et j\n");
	printf("[7] afficher les informations\n");
}

void info()
{
	printf("Taille du tableau : %d\n", n);
	printf("Nombre de processeurs : %d\n", p);
}

/** SORT FUNCTIONS **/
int sort_pivot(TYPE_T * T, TYPE_T x, int indice_debut, int indice_fin)
{
	/* mpi initialization */
	int err;

	/* pivot initialization */
	int deb = (MAX(indice_debut, first)) - first;
	int fin = (MIN(indice_fin, last)) - first;
	TYPE_T temp;

	/* sort pivot */
	while(deb < fin)
	{
		while (T[deb] < x && deb < fin) deb++;

		while (T[fin] >= x && deb < fin) fin --;

		if (deb < fin)
		{
			temp = T[deb];
			T[deb] = T[fin];
			T[fin] = temp;
		}
	}

	if (T[deb] < x) deb++;
	return deb;
}

int swap(TYPE_T * T, int debut, int fin, int indice_milieu, MPI_Comm comm, int deb_owner, int fin_owner, int rank_in_comm)
{
	/* mpi initialization */
	int err;

	MPI_Status status;

	/* swap initialization */
	int deb_valide, fin_valide;
	int x;
	int indice = indice_milieu;

	int last_move = 0;

	while (deb_owner < fin_owner)
	{
		deb_valide = 0;
		fin_valide = 0;
		/*si ce processeur est le deb_owner*/
		if (rank_in_comm == deb_owner)
		{
			if(indice <= (last - first))
			{
				deb_valide = 1;

				/*Envoi de la validation du deb_owner*/
				MPI_Bcast(&deb_valide, 1, MPI_INT, rank_in_comm, comm);
				/*Validation du fin_owner*/
				MPI_Bcast(&fin_valide, 1, MPI_INT, fin_owner, comm);
				/* Si fin_owner valide on peut traiter, sinon on recommmence */
				if (fin_valide)
				{
					/*Echange des 2 valeurs*/
					MPI_Send(&(T[indice]), 1, MPI_INT, fin_owner, fin_owner, comm);
					MPI_Recv(&(T[indice]), 1, MPI_INT, fin_owner, fin_owner, comm, &status);
					indice++;
				}
				else
				{
					fin_owner--;
					last_move = 1;
				}
			}
			/* Si deb_owner invalide on le dit puis on recommence */
			else
			{
				MPI_Bcast(&deb_valide, 1, MPI_INT, rank_in_comm, comm);
				deb_owner++;
				last_move = 0;
			}
		}
		/*Pour les autres processeurs*/
		else
		{
			/*Vérification du deb_owner*/
			MPI_Bcast(&deb_valide, 1, MPI_INT, deb_owner, comm);
			if(deb_valide){
				/*Traitement du fin_owner*/
				if (fin_owner == rank_in_comm){
					if((indice - 1) >= 0)
					{
						fin_valide = 1;
						/*Envoi de la validation du fin_owner*/
						MPI_Bcast(&fin_valide, 1, MPI_INT, rank_in_comm, comm);

						/*Echange des 2 valeurs*/
						MPI_Recv(&x, 1, MPI_INT, deb_owner, rank_in_comm, comm, &status);
						MPI_Send(&(T[indice - 1]), 1, MPI_INT, deb_owner, rank_in_comm, comm);
						T[indice-1] = x;
						indice--;
					}
					/*le fin_owner est faux*/
					else
					{
						MPI_Bcast(&fin_valide, 1, MPI_INT, rank_in_comm, comm);
						fin_owner--;
						last_move = 1;
					}
				}
				/*Si autre processeur*/
				else
				{
					/*Validation du fin_owner*/
					MPI_Bcast(&fin_valide, 1, MPI_INT, fin_owner, comm);
					if (!fin_valide)
					{
						fin_owner--;
						last_move = 1;
					}
				}
			}
			/*le deb_owner est faux*/
			else
			{
				deb_owner++;
				last_move = 0;
			}
		}

		err = MPI_Barrier(comm);
		ASSERT(err, "MPI_Barrier")
	}
	indice += first;
	if (last_move == 0)
	{
		MPI_Bcast(&indice, 1, MPI_INT, fin_owner, comm);
	}
	else
	{
		MPI_Bcast(&indice, 1, MPI_INT, deb_owner, comm);
	}
	return (indice);
}

void tri_pivot(TYPE_T * T, int debut, int fin, MPI_Comm comm, int deb_owner, int fin_owner, int rank_in_comm)
{
	/* sort initialization */
	TYPE_T x;
	int indice_milieu; /* de 0 à last - first*/
	int intertab; //indice du tableau indiquant ou s'arrete la premiere demi partie suivante et ou commence la deuxieme demi partie suivante
	MPI_Comm comm1;
	MPI_Comm comm2;
	int proc_avant = p - 1;
	int proc_apres = 0;
	/* mpi initialization */
	int err;

	/* recuperer le pivot */
	if (deb_owner == rank_in_comm)
	{
		x = T[debut - first];
	}

	err = MPI_Bcast(&x, 1, MPI_INT, deb_owner, comm);
	ASSERT(err, "MPI_Bcast --> td_trier")

	/*propre a chaque processeur*/
	indice_milieu = sort_pivot(T, x, debut, fin);

	intertab = swap(T, debut, fin, indice_milieu, comm, deb_owner, fin_owner, rank_in_comm);

	/*-----------------------------------------------------------------------*/
	/* -------- Ce qui suit décide et fait les bons appels récursifs --------*/
	/*-----------------------------------------------------------------------*/

	proc_avant = BLOCK_OWNER(intertab-1, p, n);
	proc_apres = BLOCK_OWNER(intertab, p, n);

	/* -------- Si la séparation des sous tableaux peut permettre une parallélisation --------*/

	if (proc_avant != proc_apres)
	{
		if (already_split == intertab)
		{
			if ((fin - intertab) > 1)
			{
			    if (proc_apres == BLOCK_OWNER(intertab+1, p, n))
			    {
			        tri_pivot(T, intertab + 1, fin, comm, 0, BLOCK_OWNER(fin, p, n) - proc_apres, rank_in_comm);
			    }
			    else /* si les blocs sont de taille 1 */
			    {
			        if(rank_in_comm != 0)
			        {
			            MPI_Comm_split(comm, MPI_UNDEFINED, rank_in_comm - 1, &comm1);
			            tri_pivot(T, intertab + 1, fin, comm, 0, BLOCK_OWNER(fin, p, n) - proc_apres + 1, rank_in_comm - 1);
			        }
			        else
			        {
			            MPI_Comm_split(comm, 1, rank_in_comm, &comm2);
			        }
			    }

			}
		}
		else
		{
		    already_split = intertab;

		    /* sort DOWN subtable */
			if (rank <= proc_avant)
			{
				MPI_Comm_split(comm, 1, rank_in_comm, &comm1);
				if ((intertab - debut) > 1)
				{
					tri_pivot(T, debut, intertab - 1, comm1, BLOCK_OWNER(debut, p, n) - (rank -rank_in_comm), proc_avant - (rank - rank_in_comm), rank_in_comm);
				}
			}
			/* sort UP subtable */
			else
			{
				MPI_Comm_split(comm, 2, rank - proc_apres, &comm2);

				if (intertab - debut == 0) /* il n'y avait pas d'éléments strictement inférieur au pivot, le premier élément est donc le plus petit */
				{
					intertab++;
					proc_apres = BLOCK_OWNER(intertab, p, n); // au cas ou les block sont de taille 1
				}
				if ((fin - intertab) > 0)
				{
					tri_pivot(T, intertab, fin, comm2, 0, BLOCK_OWNER(fin, p, n) - proc_apres, rank - proc_apres);
				}
			}
		}
	}

	/* ------- Si la séparation des sous tableaux ne peut pas permettre de parallélisation --------*/

	else
	{
		/* sort DOWN subtable */
		if ((intertab - debut) > 1)
		{
			tri_pivot(T, debut, (intertab - 1), comm, (BLOCK_OWNER(debut, p, n) - (rank - rank_in_comm)), proc_avant - (rank - rank_in_comm), rank_in_comm);
		}

		MPI_Barrier(comm);


		/* sort UP subtable */
		if (intertab == debut)
		{
			intertab++; /* il n'y avait pas d'éléments strictement inférieur au pivot, le premier élément est donc le plus petit */
			proc_apres = BLOCK_OWNER(intertab, p, n);
		}
		if ((fin - intertab) > 0)
		{
			tri_pivot(T, intertab, fin, comm, proc_apres - (rank - rank_in_comm), (BLOCK_OWNER(fin, p, n) - (rank - rank_in_comm)), rank_in_comm);
		}
	}
}

void TD_trier(TYPE_T * T)
{
	int debut = 0;
	int fin = n - 1;

	already_split = -1; /*réinitialiser cette variable globale, sinon on pourrait avoir des problemes*/
	tri_pivot(T, debut, fin, MPI_COMM_WORLD, 0, p - 1, rank);
}

/** MAIN **/
int main(int argc, char **argv)
{
	/* global initialization */
	int err, i, j;
	double time, max_time;

	/* mpi initialization */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &p);

	/* arguments checking */
	if (argc != 2)
	{
		if (!rank) printf ("usage : %s <N>\n", argv[0]);
		MPI_Finalize(); exit (1);
	}

	/****** local initialization ******/
	n = atoi(argv[1]);
	first = FIRST(rank,p,n);
	last = LAST(rank,p,n);

	int indice_get, indice_put, x, sum;
	int *T = NULL;
	int choice = -1;

	if (!rank)
	{
		printf("** 8inf856 devoir 3 : fonctions MPI **\n");
		info();
		printf("\n");
	}

	time = -MPI_Wtime();
	T = TD_init();
	time += MPI_Wtime();
	MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (!T)
	{
		MPI_Abort(MPI_COMM_WORLD, 911);
	}

	if (!rank) printf("[tableau initialisé (durée : %lf sec)]\n", max_time);
	TD_afficher(T, 0, n-1);
	if (!rank) printf("\n");

	while (choice != 0)
	{
		err = 0;

		if (!rank) /*selection du choix*/
		{
			printf("\n");
			welcome();
			printf("veuillez saisir un choix : ");
			scanf("%d*",&choice); getchar ();
			if (!rank) printf("\n");
		}

		MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

		switch (choice)
		{
			case 1: /*re-initialisation*/
				for (i = 0; i < last - first + 1; ++i)
				{
					T[i] = rand()%(2*n);
				}

				if (!rank) printf("[tableau réinitialisé]\n");
				TD_afficher(T, 0, n-1);
				if (!rank) printf("\n");
				break;

			case 2: /*get*/
				if (!rank)
				{
					printf("[get] veuillez saisir un indice : ");
					scanf("%d*",&indice_get); getchar ();
				}

				MPI_Bcast(&indice_get, 1, MPI_INT, 0, MPI_COMM_WORLD);

				time = -MPI_Wtime();

				MPI_Win_fence((MPI_MODE_NOPUT | MPI_MODE_NOPRECEDE), win);
				if (rank == p - 1) TD_get(T, indice_get, &x);
				MPI_Win_fence(MPI_MODE_NOSUCCEED, win);

				time += MPI_Wtime();
				MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

				MPI_Bcast(&x, 1, MPI_INT, p - 1, MPI_COMM_WORLD);

				if (!rank)
				{
					if (x != -1) printf("[get] valeur reçue : %d (durée : %lf sec)\n", x, max_time);
					else printf("[ERREUR : indice invalide]\n");
				}
				break;

			case 3: /*put*/
				if (!rank)
				{
					printf("[put] veuillez saisir un indice : ");
					scanf("%d*",&indice_put); getchar ();

					printf("[put] veuillez saisir une valeur de remplacement : ");
					scanf("%d*",&x); getchar ();
				}

				MPI_Bcast(&indice_put, 1, MPI_INT, 0, MPI_COMM_WORLD);
				MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);

				time = -MPI_Wtime();

				MPI_Win_fence((MPI_MODE_NOPUT | MPI_MODE_NOPRECEDE), win);
				if (rank == p - 1) err = TD_put(T, indice_put, x);
				MPI_Win_fence(MPI_MODE_NOSUCCEED, win);

				time += MPI_Wtime();
				MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

				MPI_Bcast(&err, 1, MPI_INT, p - 1, MPI_COMM_WORLD);

				if (!rank)
				{
					if (err == 1) printf("[ERREUR : indice invalide]\n");
					else printf("[put] valeur remplacée (durée : %lf sec)\n", max_time);
				}
				break;

			case 4: /*somme*/
				time = -MPI_Wtime();

				sum = TD_sum(T);

				time += MPI_Wtime();
				MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

				if (!rank) printf("résultat de la somme : %d (durée : %lf sec)\n", sum, max_time);
				break;

			case 5: /*tri*/
				time = -MPI_Wtime();

				TD_trier(T);

				time += MPI_Wtime();
				MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

				MPI_Barrier(MPI_COMM_WORLD);

				if (!rank) printf("[tri effectué (durée : %lf sec)]\n", max_time);
				TD_afficher(T, 0, n-1);
				if (!rank) printf("\n");
				break;

			case 6: /*affichage*/
				if (!rank)
				{
					printf("[affiche] veuillez saisir un indice i : ");
					scanf("%d*",&i); getchar ();

					printf("[affiche] veuillez saisir un indice j : ");
					scanf("%d*",&j); getchar ();
				}

				MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD);
				MPI_Bcast(&j, 1, MPI_INT, 0, MPI_COMM_WORLD);

				if (i < 0 || i > n)
				{
					if (!rank) printf("[ERREUR : indice i invalide]\n");
					err = 1;
				}

				if (j < i || j > n)
				{
					if (!rank) printf("[ERREUR : indice j invalide]\n");
					err = 1;
				}

				if (err != 1)
				{
					TD_afficher(T, i, j);
				 	if (!rank) printf("\n");
				}
				break;

			case 7:
				if (!rank) info();
				break;

			default:
				break;
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	// Release the window

	if (!rank) printf("au revoir ;)\n");

	if (T)
	{
		MPI_Win_free(&win);
		MPI_Free_mem(T);
	}

	MPI_Finalize();

	return 0;
}
