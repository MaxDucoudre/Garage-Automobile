
#include <sys/wait.h>
#include <ctype.h>


#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define R 0 
#define W 1

int nb_file;

// Fonction pour vérifier si une chaine de caractère contient bien un nombre
// return 1 si oui, 0 si non
int isNumber(char string[])
{
	int i;
    for (i = 0; string[i]!= '\0'; i++)
    {
        if (isdigit(string[i]) == 0)
        {
           return 0;
        }
    }
    return 1;
}



void eraseCleFiles() {

	system("rm -rf ./files_cle/cle.serv_*");
}

// fonction permettant de mettre fin au programme proprement
void endGarage() {
	// enlever processus & IPC
	// Finir le travail si un travail est en cours (ne pas interrompre mécanicien & chef d'atelier)

	

	// Suppression des fichiers de clé générées :
	eraseCleFiles();



	
	

	printf("\nFin du programme : fermeture du garage!\n");
	exit(EXIT_SUCCESS);
}







// LE GARAGE
int main(int argc, char *argv[])
{


	// VERIFICATION A LA LIGNE DE COMMANDE
	int i;

	if(argc != 7) // Si le nombre d'arguments n'est pas bon
	{
		printf("usage : %s nb_chefs nb_mecaniciens nb_1 nb_2 nb_3 nb_4\n", argv[0]);

		return EXIT_FAILURE;
	} 
	else 
	{
		if(strtol(argv[1],NULL,0) < 2) // si le nombre de chefs d'atelier est inférieur à 2
		{
			printf("Il faut au moins 2 chefs d'ateliers!\n");
			return EXIT_FAILURE;
		} 
		else if(strtol(argv[2],NULL,0) < 3) // si le nombre de mécaniciens est inférieur à 3
		{

			printf("Il faut au moins 3 mécaniciens!\n");
			return EXIT_FAILURE;
		} else {

			// Vérification si les arguments sont bien des entiers (sauf le premier)
			for(i = 1; i< argc; i++) { 
				if(isNumber(argv[i]) == 0) 
				{
					printf("Il faut des entiers en argument !\n");
					return EXIT_FAILURE;
				}
			}

		}
	}
	// FIN VERIFICATION A LA LIGNE DE COMMANDE



	// Conversion des arguments de chaînes de caractère vers des entiers
	int nb_chefs = strtol(argv[1], NULL, 0);
	int nb_mecanicien = strtol(argv[2], NULL, 0);
	int nb_1 = strtol(argv[3], NULL, 0); 
	int nb_2 = strtol(argv[4], NULL, 0);
	int nb_3 = strtol(argv[5], NULL, 0);
	int nb_4 = strtol(argv[6], NULL, 0);

	// Signal de fin du programme
	signal(SIGINT, endGarage); // le programme vas lancer la fonction endGarage à la récéption du signal SIGINT (CTRL+C)


	printf("GARAGE - Début du garage...\n");



	// Création des objets IPC nécéssaires
	key_t cle ;
	int file_mess;
	FILE *fp;
	char buffer[2];
	char fichier_cle[40];
	char command_create_file[40];
	nb_file = nb_chefs;


	// On s'assure que la précédente fermeture s'est bien passée :
	eraseCleFiles();

	for(i = 0; i<nb_chefs; i++) 
	{
		// Création de fichier pour stocker les clés des objets IPC
		sprintf(buffer, "%d", i);
		strcat(strcpy(fichier_cle, FICHIER_CLE), buffer);
		printf("%s\n", fichier_cle);


		strcat(strcpy(command_create_file, "touch "), fichier_cle);
		system(command_create_file); // J'utilise la commande touch pour créer les fichiers nécéssaires


		
		// Génération d'une clé
		cle = ftok(fichier_cle, 'a');
		printf("GARAGE - cle : %d générée!\n", cle);


		// Création des objets IPC avec la clé générée
		file_mess = msgget(cle, IPC_CREAT|0600);
		printf("GARAGE - File de message : %d générée!\n", file_mess);




	}

	

	// Création des processus de chef d'atelier



	// Création des processus mécanicien
	





	while(1) { 

		// Création "infinie" de clients (prévoir de laisser un temps d'interval)

	}


	return EXIT_SUCCESS;

}

