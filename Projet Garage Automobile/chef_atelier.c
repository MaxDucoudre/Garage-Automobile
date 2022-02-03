#include <stdio.h>
#include <stdlib.h>

#include "verif.h"
#include "types.h"


#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include <string.h>


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

	// Permet de récupérer la clé de la file relié au chef d'atelier
	key_t cle;
	char fichier_cle[50];
	char buffer[5];
	sprintf(buffer, "%d", numero_ordre);
	strcat(strcpy(fichier_cle, FICHIER_CLE), buffer); 

	cle = ftok(fichier_cle, 'a');
	int file_mess = msgget(cle, IPC_CREAT);
	printf("CHEF_%d - Récupération de la clé %d et de la file %d\n",numero_ordre, cle, file_mess);


	// variables
	ssize_t nb_lus;
	requete_garage requete;

	while(1) {
		// Récupération de la clé


		// Attend/reçoit requête d'un client
		nb_lus = msgrcv(file_mess, &requete, sizeof(requete)-sizeof(long int), 1, 0); // bloquant

		//printf("CHEF_%d - Récupération d'une requête : %d\n",numero_ordre, requete.nb_outil_2);



		// Envoi requête au mécanicien (durée & outils nécéssaires dans chaque catégories)

		// attente de la fin du travail du mécanicien

		// notification du résultat de la requête au client

	}

	endChefAtelier();

}
