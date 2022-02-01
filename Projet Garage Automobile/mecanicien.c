#include <stdio.h>
#include <stdlib.h>

int isWorking;



// Fonction mettant fin au processus mécanicien proprement (a condition qu'il ait terminé )
void endMecanicien() {
	// enlever processus & IPC

	printf("\nFin du mécanicien\n");

	while()
	exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
	int numero_ordre;

	isWorking = false;

	if(argc != 2) 
	{
		printf("Mécanicien incorrect!\n");

		return EXIT_FAILURE;
	}

	// Convertion de l'argument en entier
	numero_ordre = strtol(argv[1], NULL, 0);

	// Fin du mécanicien à la récéption du signal SIGINT 
	signal(SIGINT, endMecanicien); 

	while(1) {

	// Reçoit requête du chef d'atelier (travail a faire)
		// requête ayant le nb d'outils par catégorie

		isWorking = true;

	// Réservation des outils (sémaphores)


	// Réalise le travail demandé (via (u)sleep)



	// Envoie sa réponse au chef d'atelier (notification)


		isWorking = false;
	}

}
