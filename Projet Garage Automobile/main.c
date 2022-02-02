
#include <sys/wait.h>


#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>


#include "types.h"
#include "verif.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>


#define NB_CLIENT_TO_CREATE 1

int nb_file;





// fonction permettant de supprimer tous les fichiers temporaires générés par le programme
void eraseCleFiles() {
	system("rm -rf ./files_cle/cle.serv_*");
}




// fonction permettant de mettre fin au programme proprement
void endGarage() {
	printf("\nGARAGE - Début de la fermeture du garage...\n");

	// enlever processus & IPC
	// Finir le travail si un travail est en cours (ne pas interrompre mécanicien & chef d'atelier)



	// Termine tous les thread qui ne se seraient pas fini)	



	// Suppression des fichiers de clé générées :
	eraseCleFiles();

	
	
	printf("GARAGE - Garage fermé, fin du programme!\n");
	exit(EXIT_SUCCESS);
}


// Permet de créer un chef d'atelier (dans un thread pour éviter que le garage s'arrête)
// Cette fonction est appellée lors de la création des chefs d'ateliers dans le main()
void *createChefAtelier (void *args)
{

	// Récupération des arguments
	int * arg =(int*)args;

	// initialisation des varaibles qui vont permettre de convertir les arguments int en string
	char numero_ordre_str[5];
	char nb_outil_1_str[5];
	char nb_outil_2_str[5];
	char nb_outil_3_str[5];
	char nb_outil_4_str[5];


	// Convertion des arguments de int vers chaine de caractère
	sprintf(numero_ordre_str, "%d ", arg[0]);
	sprintf(nb_outil_1_str, "%d ", arg[1]);
	sprintf(nb_outil_2_str, "%d ", arg[2]);
	sprintf(nb_outil_3_str, "%d ", arg[3]);
	sprintf(nb_outil_4_str, "%d ", arg[4]);


	// Création de la commande permettant de lancer le un chef d'atelier
	char command[50];
	strcat(strcpy(command, "./chef_atelier "), numero_ordre_str);
	strcat(strcpy(command, command), nb_outil_1_str);
	strcat(strcpy(command, command), nb_outil_2_str);
	strcat(strcpy(command, command), nb_outil_3_str);
	strcat(strcpy(command, command), nb_outil_4_str);


	// Lancement d'un chef d(atelier)
	printf("GARAGE - Génération du CHEF_%s...\n", numero_ordre_str);
	system(command);


}


// Fonction permettant de créer un mécanicien via un thread
void *createMecanicien (void *args)
{

	// Création d'un mécanicien & conversion de son argument (même principe que pour les chefs d'atelier)
	int * arg =(int*)args;
	char numero_ordre_str[5];
	sprintf(numero_ordre_str, "%d ", arg[0]);
	char command[50];
	strcat(strcpy(command, "./mecanicien "), numero_ordre_str);
	printf("GARAGE - Génération du MECANICIEN_%s...\n", numero_ordre_str);
	system(command);
}


// Fonction permettant de créer un client via un thread
void *createClient (void *args)
{

	donnees_client * arg =(donnees_client*)args;

	char nb_chef_str[3];
	sprintf(nb_chef_str, "%d ", arg->nb_chefs);

	char command[500];
	strcat(strcpy(command, "./client "), nb_chef_str);
	
	int i;
	char key_file_str[20];

	for(i = 0; i < arg->nb_chefs; i++) 
	{

		sprintf(key_file_str, "%d ", arg->cles_ipc[i]);

		strcat(strcpy(command, command), key_file_str);

	}

	printf("GARAGE - Génération d'un nouveau CLIENT...\n");
	system(command);

}


// Fonction pour vérifier si une chaine de caractère contient bien un nombre
// return 1 si oui, 0 si non
/*
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
*/




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


	nb_file = nb_chefs; // nombre de file a créer



	// PARTIE CREATION DES PROCESSSUS ET DES OBJETS IPCS

	// On s'assure que la précédente fermeture s'est bien passée :
	eraseCleFiles();


	// Thread pour les chefs d'ateliers
     pthread_t threads_chef[nb_chefs]; // id thread
	int args_chef[5]; // data thread
		// les arguments nécéssaires pour un chef d'atelier (les outils du garage)
		args_chef[1] = nb_1;
		args_chef[2] = nb_2;
		args_chef[3] = nb_3;
		args_chef[4] = nb_4;


	// Thread pour les mécaniciens
     pthread_t threads_mecanicien[nb_mecanicien]; // id thread
	int args_mecanicien[1]; // data thread


	int cle_ipcs[nb_chefs]; // Variable qui vas stocker les clés pour accéder aux files IPC

	for(i = 0; i<nb_chefs; i++) 
	{

		// CREATION DES OBJETS IPCS
		// Création de fichier pour stocker les clés des objets IPC
		sprintf(buffer, "%d", i);
		strcat(strcpy(fichier_cle, FICHIER_CLE), buffer);


		strcat(strcpy(command_create_file, "touch "), fichier_cle);
		system(command_create_file); // J'utilise la commande touch pour créer les fichiers nécéssaires

		
		// Génération d'une clé
		cle = ftok(fichier_cle, 'a');
		printf("GARAGE - cle : %d générée!\n", cle);

		// Création des objets IPC avec la clé générée
		file_mess = msgget(cle, IPC_CREAT|0600);
		printf("GARAGE - File de message : %d générée!\n", file_mess);

		cle_ipcs[i] = cle; // stockage des clés pour les files IPC (nécéssaire au client)


		// CREATION DES PROCESSUS CHEFS D'ATELIER (avec des threads))

		args_chef[0] = i; // correspond à l'ordre d'un chef d'atelier

		// Puis on lance un processus pour chaque chefs d'atelier
		pthread_create(&threads_chef[i], NULL, createChefAtelier, &args_chef);



		// CREATION DES PROCESSUS MECANICIENS (avec des threads)
		// même principe que pour les chefs d'atelier
		args_mecanicien[0] = i;
		pthread_create(&threads_mecanicien[i], NULL, createMecanicien, &args_mecanicien);


	}

	// On attend que chefs d'ateliers & les mécaniciens finissent de se préparer
	sleep(5);


	// INFORMATIONS NECESSAIRES POUR CREER UN CLIENT :
	int numero_client = 1;


	donnees_client args_client; // Données néccésaires pour un client (struct donnees_client)
	args_client.nb_chefs  = nb_chefs; // nombre de chefs d'atelier


	// le client vas également avoir besoin des clés pour accéder aus files IPC
	for(i = 0; i<nb_chefs; i++) 
	{
		args_client.cles_ipc[i] = cle_ipcs[i];
	}

	pthread_t threads_client[NB_CLIENT_TO_CREATE];


	while(1) { 

		if(numero_client <= NB_CLIENT_TO_CREATE) 
		{

			// Création "infinie" de clients (prévoir de laisser un temps d'interval)
			args_client.client_num = numero_client; // attribution d'un numéro à un client
			pthread_create(&threads_client[i], NULL, createClient, &args_client);
			numero_client++;
		}





	}


	printf("GARAGE - Le nombre maximale de client à été atteind...");
	endGarage();
}

