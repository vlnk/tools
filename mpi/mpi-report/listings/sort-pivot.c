int sort_pivot(TYPE_T * T, TYPE_T x, int indice_debut, int indice_fin)
{
	/* mpi initialization */
	int err;

	/* initialisation des deb et fin locaux */
	int deb = (MAX(indice_debut, first)) - first;
	int fin = (MIN(indice_fin, last)) - first;
	TYPE_T temp;

	/* sort pivot */
	while(deb < fin) {
		while (T[deb] < x && deb < fin) deb++;

		while (T[fin] >= x && deb < fin) fin --;

		if (deb < fin) {
			temp = T[deb];
			T[deb] = T[fin];
			T[fin] = temp;
		}
	}

	if (T[deb] < x) deb++;
	return deb;
}