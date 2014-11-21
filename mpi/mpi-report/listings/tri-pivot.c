int 	already_split = -1;

void tri_pivot(int * T, int debut, int fin, MPI_Comm comm, int deb_owner, int fin_owner, int rank_in_comm){

	/*Si le processeur possede la premiere valeur à trier*/
	if (deb_owner == rank_in_comm) /*Recuperer pivot*/

	/*Envoi du pivot aux autres processeurs*/

	/*Chaque processeur tri sa zone*/
	indice_milieu = sort_pivot(T, x, debut, fin);
	
	/*Positionnement des faibles valeurs au début de la zone à trier et des grandes à la fin de la zone*/
	/*et récupération de l'indice de séparation*/
	intertab = swap(T, debut, fin, indice_milieu, comm, deb_owner, fin_owner, rank_in_comm);

	/* ======== Ce qui suit décide et fait les bons appels récursifs ========*/
	/*indices permettant de savoir si on doit paralleliser ou pas*/
	proc_avant = BLOCK_OWNER(intertab-1, p, n);
	proc_apres = BLOCK_OWNER(intertab, p, n);

	/* ======== Si la séparation des sous tableaux peut permettre une parallélisation ========*/
	if (proc_avant != proc_apres){
		/*Si il y a eu déjà parallélisation à cet endroit*/
		if (already_split == intertab){
			if ((fin - intertab) > 1){
			    if (proc_apres == BLOCK_OWNER(intertab+1, p, n)){
			    	/*tri de la partie à droite du séparateur*/
			        tri_pivot(T, intertab + 1, fin, comm, 0, BLOCK_OWNER(fin, p, n) - proc_apres, rank_in_comm);
			    }
			    else /* cas ou les blocs sont de taille 1 */
			}
		}
		/*Sinon on parallelise*/
		else{
		    already_split = intertab; /*cet indice est sauvegarder pour ne pas reparalleliser ici*/
		    
		    /* sort DOWN subtable */
			if (rank <= proc_avant){
				/*Separation de la communication*/
				MPI_Comm_split(comm, 1, rank_in_comm, &comm1);

				if ((intertab - debut) > 1){
					/*appel du tri sur la partie avant la séparation avec les processeurs associés à ces parties*/	
					tri_pivot(T, debut, intertab - 1, comm1, BLOCK_OWNER(debut, p, n) - (rank -rank_in_comm), proc_avant - (rank - rank_in_comm), rank_in_comm);
				}
			}
			/* sort UP subtable */
			else{
				/*Separation de la communication*/
				MPI_Comm_split(comm, 2, rank - proc_apres, &comm2);

				/* s'il n'y avait pas d'éléments strictement inférieur au pivot, le premier élément est donc le plus petit */
				if (intertab - debut == 0) /*on incrémente l'indice de début de la partie apres la séparation*/
				if ((fin - intertab) > 0) {
					/*appel du tri sur la partie apres la séparation avec les processeurs associés à ces parties*/
					tri_pivot(T, intertab, fin, comm2, 0, BLOCK_OWNER(fin, p, n) - proc_apres, rank - proc_apres);
				}
			}
		}
	}

	/*======== Si la séparation des sous tableaux ne permet pas de parallélisation ========*/
	else
	{
		/* sort DOWN subtable */
		if ((intertab - debut) > 1) {
			/*appel du tri sur la partie avant la séparation*/
			tri_pivot(T, debut, (intertab - 1), comm, (BLOCK_OWNER(debut, p, n) - (rank - rank_in_comm)), proc_avant - (rank - rank_in_comm), rank_in_comm);
		}
		/*attente de la fin du tri de lapremiere partie*/
		MPI_Barrier(comm);
		
		/* sort UP subtable */
		/* s'il n'y avait pas d'éléments strictement inférieur au pivot, le premier élément est donc le plus petit */
		if (intertab == debut) /*on incrémente l'indice de début de la partie apres la séparation*/
		if ((fin - intertab) > 0) {
			/*appel du tri sur la partie apres la séparation*/
			tri_pivot(T, intertab, fin, comm, proc_apres - (rank - rank_in_comm), (BLOCK_OWNER(fin, p, n) - (rank - rank_in_comm)), rank_in_comm);
		}
	}	
}