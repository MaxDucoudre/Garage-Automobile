
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

#include <pthread.h>


#define NB_CLIENT_TO_CREATE 5

int nb_file;
int nb_chefs;
int nb_mecanicien;
int* cle_ipc_client_to_chef;
int sem_id;
int sem_id_chef;


pthread_t *threads_mecanicien;
pthread_t *threads_chef;



// fonction permettant de supprimer tous les fichiers temporaires générés par le programme
void eraseCleFiles() {
	system("rm -rf ./files_cle/cle.*");
}

void destroyIPCObject()
{
	int i;
	int t;
	char buffer[256];

	semctl(sem_id,0,IPC_RMID); // destruction sémaphore outils
	semctl(sem_id,1,IPC_RMID); 
	semctl(sem_id,2,IPC_RMID); 
	semctl(sem_id,3,IPC_RMID); 


	for(i = 0; i<nb_chefs; i++)
	{
		t = semctl(sem_id_chef, i, IPC_RMID);
		assert(t = -1);
	}

	// Destruction des files client-chef
	for(i = 0; i <nb_chefs; i ++)
	{

		t = msgctl(cle_ipc_client_to_chef[i],IPC_RMID, 0);
		assert(t = -1);

	}

}

void endProc() 
{
	int i;

	// envoi de signaux aux autres processus pour qu'ils se ferment proprement
	for(i = 0; i < nb_chefs; i++)
	{
		printf("PTHREAD %d", threads_chef[i]);

		pthread_kill(threads_chef[i], SIGINT); // envoyer un signal au processus chef
	}

	for(i = 0; i < nb_mecanicien; i++)
	{
		pthread_kill(threads_mecanicien[i], SIGINT); // envoyer un signal au processus mecanicien
	}
}


// fonction permettant de mettre fin au programme proprement
void endGarage() {
	printf("\nGARAGE - Début de la fermeture du garage...\n");

	destroyIPCObject(); // Détruit tous les objets IPC 
	eraseCleFiles(); // Suppression des fichiers de clé générées 
	//endProc(); 	// Termine tous les thread qui ne se seraient pas fini)	

	printf("GARAGE - Garage fermé, fin du programme!\n");
	exit(EXIT_SUCCESS);
}


// Permet de créer un chef d'atelier (dans un thread pour éviter que le garage s'arrête)
// Cette fonction est appellée lors de la création des chefs d'ateliers dans le main()
void *createChefAtelier (void *args)
{
	int * arg =(int*)args; // Récupération des arguments du thread
	char buffer[256];
	
	printf("GARAGE - Génération du CHEF_%d...\n", arg[0]);

	sprintf(buffer, "./chef_atelier %d %d %d %d %d", arg[0], arg[1], arg[2], arg[3], arg[4]); // création de la commande
	system(buffer); // lancement du chef d'atelier
	
	
	/* Version avec exec, mais pas de suivi console */
	/*
	sprintf(buffer, "%d %d %d %d %d", arg[0], arg[1], arg[2], arg[3], arg[4]);
	printf("./chef_atelier %s\n", buffer);
	execl("./chef_atelier", "./chef_atelier", ,buffer);
	*/
	

	
}


// Fonction permettant de créer un mécanicien via un thread
void *createMecanicien (void *args)
{

	// Création d'un mécanicien & conversion de son argument (même principe que pour les chefs d'atelier)
	int * arg =(int*)args;
	char buffer[256];


	sprintf(buffer, "./mecanicien %d", arg[0]);
	printf("GARAGE - Génération du MECANICIEN_%d...\n", arg[0]);
	system(buffer);

	/*
	sprintf(buffer, "%d", arg[0]);
	//printf("%s\n", buffer);
	execl("./mecanicien", "./mecanicien", buffer);
	*/

}


// Fonction permettant de créer un client via un thread
// Le clients a besoin du nombre de chef d'atelier et des files IPC
void *createClient (void *thread_arg)
{
	int * arg =(int*)thread_arg;
	int i;
	char key_file_str[20]; 
	char nb_chef_str[3]; 
	char command[20 + sizeof(key_file_str)/sizeof(key_file_str[0])*(arg[0]+1)]; // le nombre de caractère de la commande dépend du nombre de chefs
	
	sprintf(nb_chef_str, "%d ", arg[0]);
	strcat(strcpy(command, "./client "), nb_chef_str);
		
	for(i = 0; i < arg[0]; i++) 
	{
		sprintf(key_file_str, "%d ", arg[i+1]);
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

		// VARIABLES
	// Conversion des arguments de chaînes de caractère vers des entiers
	nb_chefs = strtol(argv[1], NULL, 0);
	nb_mecanicien = strtol(argv[2], NULL, 0);
	int nb_1 = strtol(argv[3], NULL, 0); 
	int nb_2 = strtol(argv[4], NULL, 0);
	int nb_3 = strtol(argv[5], NULL, 0);
	int nb_4 = strtol(argv[6], NULL, 0);

	key_t sem_key_chef;
	key_t sem_key;
	key_t cle;
	int file_mess;
	FILE *fp;
	char buffer[2];
	char fichier_cle[40];
	char command_create_file[40];
	char buffer_i[256];

	// arguments qui vont passer dans les threads
	int args_mecanicien[1]; // argument des mécaniciens
	int cle_ipcs[nb_chefs]; // Variable qui vas stocker les clés (nécéssaire au client)
	cle_ipc_client_to_chef = cle_ipcs; // enregistrement des clés pour pouvoir fermer les files de message à la fin du programme
	//variables pour créer un client
	int numero_client = 1;
	int arguments_client[1+nb_chefs];

	nb_file = nb_chefs; // nombre de file a créer

	// Id de thread
    
	pthread_t threads_client[NB_CLIENT_TO_CREATE];

	threads_mecanicien = (pthread_t*) malloc(nb_mecanicien * sizeof(pthread_t));
   threads_chef = (pthread_t*) malloc(nb_chefs * sizeof(pthread_t));

	// Signal de fin du programme
	signal(SIGINT, endGarage); // le programme vas lancer la fonction endGarage à la récéption du signal SIGINT (CTRL+C)
	printf("GARAGE - Début du garage...\n");


	// PARTIE CREATION DES PROCESSSUS ET DES OBJETS IPCS
 	// Creation sémaphores : (Correspond actuellement a tous les outils du garage)

 	char command_create_sem[20]; 
 	sprintf(command_create_sem, "touch %s", FICHIER_CLE_SEM);
 	system(command_create_sem);

 	sem_key = ftok(FICHIER_CLE_SEM, 1);
 	sem_id = semget(sem_key, 4, 0666|IPC_CREAT|IPC_EXCL);
 	assert(sem_id >= 0);
 	semctl(sem_id, 0, SETVAL, nb_1);
 	semctl(sem_id, 1, SETVAL, nb_2);
 	semctl(sem_id, 2, SETVAL, nb_3);
 	semctl(sem_id, 3, SETVAL, nb_4);




	// semaphore stockant a quel point un cher d'atelier est occupé
	char command_create_chef_sem_tmp[20];
	sprintf(command_create_chef_sem_tmp, "touch %s", FICHIER_CLE_SEM_OCCUPATION);
	system(command_create_chef_sem_tmp);

	sem_key_chef = ftok(FICHIER_CLE_SEM_OCCUPATION, 1);

	sem_id_chef = semget(sem_key_chef, nb_chefs, 066|IPC_CREAT|IPC_EXCL);
		printf("GARAGE - Sémaphore pour chef d'atelier id : %d | clé : %d \n", sem_id_chef, sem_key_chef);
		int value;
	for(i = 0; i<nb_chefs; i++)
	{
		semctl(sem_id_chef,i, SETVAL, 0); 
	 	value = semctl(sem_id_chef, i, GETVAL);
		printf("Value : %d \n", value);

	}






		// données pour un chef d'atelier qui vont passer au thread
		int args_chef[5]; 
		// les arguments nécéssaires pour un chef d'atelier (les outils du garage)
		args_chef[1] = nb_1;
		args_chef[2] = nb_2;
		args_chef[3] = nb_3;
		args_chef[4] = nb_4;


	for(i = 0; i<nb_chefs; i++)
	{
		// CREATION DES OBJETS IPCS
		// Création de fichier pour stocker les clés des objets IPC
		sprintf(fichier_cle, "%s%d", FICHIER_CLE, i);
		sprintf(buffer_i, "touch %s", fichier_cle);
		system(buffer_i); // J'utilise la commande touch pour créer les fichiers nécéssaires

		
				cle = ftok(fichier_cle, 'a');

		// Génération d'une clé
		printf("GARAGE - cle : %d générée!\n", cle);
		cle_ipcs[i] = cle; // stockage des clés pour les files IPC (nécéssaire au client)

		// Création des objets IPC avec la clé générée
		file_mess = msgget(cle, IPC_CREAT|0600);
		printf("GARAGE - File de message : %d générée!\n", file_mess);





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



		// CREATION DES CLIENTS
	// INFORMATIONS NECESSAIRES POUR CREER UN CLIENT :

	arguments_client[0] = nb_chefs;

	// le client vas également avoir besoin des clés pour accéder aus files IPC
	for(i = 0; i < nb_chefs; i++) {
		arguments_client[i+1] = cle_ipcs[i];
	}


	while(1) { 
		sleep(1);
		if(numero_client <= NB_CLIENT_TO_CREATE) 
		{
			// Création "infinie" de clients (prévoir de laisser un temps d'interval)
			pthread_create(&threads_client[i], NULL, (void *) createClient, &arguments_client);
			numero_client++;
		}

	}


	printf("GARAGE - Le nombre maximale de client à été atteind...");
	endGarage();
}

