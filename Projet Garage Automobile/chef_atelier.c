#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>


#include "verif.h"



// Fonction mettant fin au processus ched d'atelier proprement (a condition qu'il ait terminé )
void endChefAtelier() {
	// enlever processus & IPC

	printf("\nFin du chef d'atelier\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{


	// VERIFICATION DES ARGUMENTS
	if(argc != 6) 
	{
		printf("Chef d'atelier incorrect!\n");

		return EXIT_FAILURE;
	}

	int i;
	for(i = 1; i < argc; i++)
	{
		if(isNumber(argv[i]) == 0) 
		{
			printf("Il faut mettre des entiers en arguments du chef d'atelier!\n");
			return EXIT_FAILURE;
		}
	}
	// FIN VERIFICATION DES ARGUMENTS

	// Convertion des arguments en entiersé	
	int numero_ordre = strtol(argv[1], NULL, 0);
	int nb_outil_1 = strtol(argv[2], NULL, 0); 
	int nb_outil_2 = strtol(argv[3], NULL, 0);
	int nb_outil_3 = strtol(argv[4], NULL, 0);
	int nb_outil_4 = strtol(argv[5], NULL, 0);

	printf("CHEF_%d - Nouveau chef d'atelier !\n", numero_ordre);

	signal(SIGINT, endChefAtelier); 


	while(1) {
		// Attend/reçoit requête d'un client

		// Envoi requête au mécanicien (durée & outils nécéssaires dans chaque catégories)

		// attente de la fin du travail du mécanicien

		// notification du résultat de la requête au client

	}

	endChefAtelier();

}
