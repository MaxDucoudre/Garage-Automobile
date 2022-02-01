#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{


	if(argc != 6) 
	{
		printf("Chef d'atelier incorrect!\n");

		return EXIT_FAILURE;
	}

	// Convertion des arguments en entiersé	
	int numero_ordre = strtol(argv[1], NULL, 0);
	int nb_outil_1 = strtol(argv[2], NULL, 0); 
	int nb_outil_2 = strtol(argv[3], NULL, 0);
	int nb_outil_3 = strtol(argv[4], NULL, 0);
	int nb_outil_4 = strtol(argv[5], NULL, 0);


	// Reçoit requête du client

	while(1) {

		// Attend/reçoit requête d'un client


		// Envoi requête au mécanicien (durée & outils nécéssaires dans chaque catégories)

		// attente de la fin du travail du mécanicien

		// notification du résultat de la requête au client
	}



}
