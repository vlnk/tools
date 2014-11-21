int swap(TYPE_T * T, int debut, int fin, int indice_milieu, MPI_Comm comm, int deb_owner, int fin_owner, int rank_in_comm){
	int err; /* mpi initialization */
	MPI_Status status; /*elements de communication Recv*/
	int deb_valide, fin_valide; /* swap initialization */
	int x;
	int indice = indice_milieu;
	int last_move = 0; /*dernier mouvement*/

	while (deb_owner < fin_owner){
		deb_valide = 0;
		fin_valide = 0;

		/*si ce processeur est le deb_owner*/
		if (rank_in_comm == deb_owner){
			/*si élément à échanger*/
			if(indice <= (last - first)){
				deb_valide = 1;
				/*Envoi de la validation du deb_owner aux autres processeurs*/
				/*Reception de la validation du fin_owner*/

				/* Si fin_owner valide on peut traiter, sinon on recommmence */
				if (fin_valide) /*Echange des 2 valeurs et incrémentation de l'indice*/
				else /*decrementation du fin_owner et mise à jour de last_move*/
			}
			/* Si pas d'élément à échanger pour le deb_owner */
			else{
				/*Envoi de la non-validation du deb_owner*/
				/*Incrementation du deb_owner et mise à jour de last_move*/
			}
		}
		/*Pour les autres processeurs*/
		else{
			/*Reception de la vérification du deb_owner*/
			/*Si deb_owner valide*/
			if(deb_valide){
				/*si ce processeur est le fin_owner*/
				if (fin_owner == rank_in_comm){
					/*s'il a un élément à échanger*/
					if((indice - 1) >= 0){
						fin_valide = 1;
						/*Envoi de la validation du fin_owner*/
						/*Echange des 2 valeurs*/
						/*Decrementation de l'indice*/
					}
					/* Si pas d'élément à échanger pour le deb_owner */
					else{
						/*Envoi de la non-validation du fin_owner*/
						/*Decrementation du fin_owner et mise à jour de last_move*/
					}
				}
				/*Si autre processeur*/
				else{
					/*Reception de la vérification du fin_owner*/
					/*Si fin_owner n'a pas d'élément à échanger*/
					if (!fin_valide) /*Decrementation du fin_owner et mise à jour de last_move*/
				}
			}
			/*Si le deb_owner n'est pas valide*/
			else	/*Incrementation du deb_owner et mise à jour de last_move*/
		}
		err = MPI_Barrier(comm);
	}
	/*transformation de l'indice local en indice global*/
	/*Transmission de l'indice de séparation en fonction du last_move aux autres processeurs*/
	return (indice);
}