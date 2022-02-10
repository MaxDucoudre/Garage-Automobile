#include <sys/wait.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <signal.h>

#include "types.h"
#include "verif.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_CLIENT_TO_CREATE 10

int nb_chefs;
int nb_mecanicien;
key_t* cle_ipcs; // files IPC
int sem_id; // Semaphore outils
int sem_id_chef; // semaphore occupation chef d'atelier


// fonction permettant de supprimer tous les fichiers temporaires générés par le programme
void eraseCleFiles() {
	system("rm -rf ./files_cle/cle.*");
}

void destroyIPCObject()
{
	int i;
	int t;
	char buffer[256];
	int file_mess;

	semctl(sem_id, 0, IPC_RMID); // destruction sémaphore outils
	semctl(sem_id, 1, IPC_RMID); 
	semctl(sem_id, 2, IPC_RMID); 
	semctl(sem_id, 3, IPC_RMID); 


	for(i = 0; i<nb_chefs; i++)
	{
		t = semctl(sem_id_chef, i, IPC_RMID);
		//assert(t != -1);
	}

	// Destruction des files client-chef
	for(i = 0; i <nb_chefs; i ++)
	{
		file_mess = msgget(cle_ipcs[i], 0);
		t = msgctl(file_mess, IPC_RMID, 0);
		//assert(t != -1);

	}

}



// fonction permettant de mettre fin au programme proprement
void endGarage() {
	couleur(GRIS);
	printf("GARAGE - Début de la fermeture du garage...\n");


	// Vérification que les enfants sont morts

	destroyIPCObject(); // Détruit tous les objets IPC 
	eraseCleFiles(); // Suppression des fichiers de clé générées 
	//endProc(); 	// Termine tous les thread qui ne se seraient pas fini)	

	couleur(GRIS);
	printf("GARAGE - Garage fermé, fin du programme!\n");

	couleur(REINIT);
	exit(EXIT_SUCCESS);
}


// Permet de créer un chef d'atelier (dans un thread pour éviter que le garage s'arrête)
// Cette fonction est appellée lors de la création des chefs d'ateliers dans le main()
void *createChefAtelier (void *args)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		int * arg =(int*)args; // Récupération des arguments du thread
		char buffer[256];

		couleur(GRIS);
		printf("GARAGE - Génération du CHEF_%d...\n" ,arg[0]);

		//sprintf(buffer, "./chef_atelier %d %d %d %d %d", arg[0], arg[1], arg[2], arg[3], arg[4]); // création de la commande
		//system(buffer); // lancement du chef d'atelier
		

		char nb_chef[32];
		sprintf(nb_chef, "%d", arg[0]);

		char nb_1[32];
		sprintf(nb_1, "%d", arg[1]);

		char nb_2[32];
		sprintf(nb_2, "%d", arg[2]);

		char nb_3[32];
		sprintf(nb_3, "%d", arg[3]);

		char nb_4[32];
		sprintf(nb_4, "%d", arg[4]);

		char nb_5[32];
		sprintf(nb_5, "%d", arg[5]);


		sprintf(buffer, "./chef_atelier %d %d %d %d %d", arg[0], arg[1], arg[2], arg[3], arg[4]);
		execl("./chef_atelier", "./chef_atelier" , nb_chef,nb_1,nb_2,nb_3,nb_4,nb_5, NULL);
	}
}


// Fonction permettant de créer un mécanicien via un thread
void *createMecanicien (void *args)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		// Création d'un mécanicien & conversion de son argument (même principe que pour les chefs d'atelier)
		int * arg =(int*)args;
		char buffer[256];

		
		//sprintf(buffer, "./mecanicien %d", arg[0]);
		couleur(GRIS);
		printf("GARAGE - Génération du MECANICIEN_%d...\n", arg[0]);
		//system(buffer);

		
		sprintf(buffer, "%d", arg[0]);
		//printf("%s\n", buffer);
		couleur(GRIS);
		execl("./mecanicien", "./mecanicien", buffer, NULL);
	}

}


// Fonction permettant de créer un client via un thread
// Le clients a besoin du nombre de chef d'atelier et des files IPC
void *createClient (void *thread_arg)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		int * arg =(int*)thread_arg;
		int i;
		char key_file_str[20]; 
		char nb_chef_str[3]; 
		char command[20 + sizeof(key_file_str)/sizeof(key_file_str[0])*(arg[0]+1)]; // le nombre de caractère de la commande dépend du nombre de chefs
		
		//sprintf(command, "./client %d ", arg[0]);

		sprintf(nb_chef_str, "%d", arg[0]);

		//char argument_of_client[arg[0]+2][20];

		char *argument_of_client[arg[0]+2];


		argument_of_client[0] = "./client";
		argument_of_client[1] = nb_chef_str;


		for(i = 0; i < arg[0]; i++) 
		{

			sprintf(key_file_str, "%d", arg[i+1]);

			argument_of_client[i+2] = key_file_str;

		}

			argument_of_client[i+3] = NULL;


		couleur(GRIS);
		printf("GARAGE - Génération d'un nouveau CLIENT...\n");
		execvp("./client", argument_of_client);
		//system(command);
	}

}




// LE GARAGE
int main(int argc, char *argv[])
{
	pid_t pid = 1;

if(pid != 0) {
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

		// VARIABLES
	// Conversion des arguments de chaînes de caractère vers des entiers
	nb_chefs = strtol(argv[1], NULL, 0);
	nb_mecanicien = strtol(argv[2], NULL, 0);
	int nb_1 = strtol(argv[3], NULL, 0); 
	int nb_2 = strtol(argv[4], NULL, 0);
	int nb_3 = strtol(argv[5], NULL, 0);
	int nb_4 = strtol(argv[6], NULL, 0);

	key_t sem_key_chef;
	key_t cle;
	key_t sem_key;
	int file_mess;
	FILE *fp;
	char buffer[2];
	char fichier_cle[40];
	char command_create_file[40];
	char buffer_i[256];

	// arguments des autres programmes
	int args_mecanicien[1]; // argument des mécaniciens
	cle_ipcs = malloc(nb_chefs * sizeof(key_t)); // Variable qui vas stocker les clés (nécéssaire au client)
	//variables pour créer un client
	int numero_client = 1;
	int arguments_client[1+nb_chefs];



	// Signal de fin du programme
	signal(SIGINT, endGarage); // le programme vas lancer la fonction endGarage à la récéption du signal SIGINT (CTRL+C)
	couleur(GRIS);
	printf("GARAGE - Début du garage...\n");


	// PARTIE CREATION DES PROCESSSUS ET DES OBJETS IPCS
 	// Creation sémaphores : (Correspond à tous les outils du garage)

 	char command_create_sem[20]; 
 	sprintf(command_create_sem, "touch %s", FICHIER_CLE_SEM);
 	system(command_create_sem);

 	sem_key = ftok(FICHIER_CLE_SEM, 'a');
 	sem_id = semget(sem_key, 4, 0600|IPC_CREAT|IPC_EXCL);

 	semctl(sem_id, 0, SETVAL, nb_1);
 	semctl(sem_id, 1, SETVAL, nb_2);
 	semctl(sem_id, 2, SETVAL, nb_3);
 	semctl(sem_id, 3, SETVAL, nb_4);



	// semaphore stockant a quel point un chef d'atelier est occupé
	char command_create_chef_sem_tmp[20];
	sprintf(command_create_chef_sem_tmp, "touch %s", FICHIER_CLE_SEM_OCCUPATION);
	system(command_create_chef_sem_tmp);


	sem_key_chef = ftok(FICHIER_CLE_SEM_OCCUPATION, 'a');
	sem_id_chef = semget(sem_key_chef, nb_chefs, 0600|IPC_CREAT|IPC_EXCL);

	couleur(GRIS);
	printf("GARAGE - Sémaphore pour les chefs d'atelier :  id = %d | clé = %d \n", sem_id_chef, sem_key_chef);

	int value2;
	for(i = 0; i < nb_chefs; i++)
	{
		value2 = semctl(sem_id_chef, i, SETVAL, 0);
	}


		// données pour un chef d'atelier qui vas passer au thread
		int args_chef[5]; 
		// les arguments nécéssaires pour un chef d'atelier (les outils du garage)
		args_chef[1] = nb_1;
		args_chef[2] = nb_2;
		args_chef[3] = nb_3;
		args_chef[4] = nb_4;
		args_chef[5] = nb_mecanicien;


	for(i = 0; i<nb_chefs; i++)
	{

		// CREATION DES OBJETS IPCS

		// Création de fichier pour stocker les clés des objets IPC
		sprintf(fichier_cle, "%s%d", FICHIER_CLE, i);
		sprintf(buffer_i, "touch %s", fichier_cle);
		system(buffer_i); // J'utilise la commande touch pour créer les fichiers nécéssaires

		cle = ftok(fichier_cle, 'a');

		// Génération d'une clé
		couleur(GRIS);
		printf("GARAGE - cle : %d générée!\n", cle);
		cle_ipcs[i] = cle; // stockage des clés pour les files IPC (nécéssaire au client)

		// Création des objets IPC avec la clé générée
		file_mess = msgget(cle, IPC_EXCL|IPC_CREAT|0600);
		couleur(GRIS);
		printf("GARAGE - File de message : %d générée!\n", file_mess);

		// CREATION DES PROCESSUS CHEFS D'ATELIER 
		args_chef[0] = i; // correspond à l'ordre d'un chef d'atelier
		// Puis on lance un processus pour chaque chefs d'atelier
		createChefAtelier(args_chef);


	}


	for(i = 0; i<nb_mecanicien; i++)
	{

		// CREATION DES PROCESSUS MECANICIENS 
		// même principe que pour les chefs d'atelier
		args_mecanicien[0] = i;
		createMecanicien(args_mecanicien);
	}



		// CREATION DES CLIENTS
	// INFORMATIONS NECESSAIRES POUR CREER UN CLIENT :

	arguments_client[0] = nb_chefs;

	// le client vas également avoir besoin des clés pour accéder aus files IPC
	for(i = 0; i < nb_chefs; i++) {
		arguments_client[i+1] = cle_ipcs[i];
	}

	while(1) { 
		if(numero_client <= NB_CLIENT_TO_CREATE) 
		{
			couleur(ROUGE);
			//printf("PID OF GARAGE %d\n", getpid());
				// Interval entre la création de client
			sleep(rand() % 10);

			// Création "infinie" de clients (prévoir de laisser un temps d'interval)
			createClient(arguments_client);
			numero_client++;
		}

	}

	couleur(GRIS);
	printf("GARAGE - Le nombre maximale de client à été atteint...");
	endGarage();
	}
}

