
// structure utilisée pour stocker ce dont un client a besoin pour être créé
typedef struct
{
	int client_num;
	int nb_chefs;
	int cles_ipc[];
}
donnees_client;

typedef struct 
{
	long type;
	int numero_ordre;
	int nb_outil_1;
	int nb_outil_2;
	int nb_outil_3;
	int nb_outil_4;
} 
ordre;


typedef struct 
{
	long type;
	int resu;
} 
reponse_ordre;


#define FICHIER_CLE "./files_cle/cle.serv_"





/* 
   Utilisation (pour ecrire en rouge) :
   couleur(ROUGE); fprintf(stdout,"Hello\n"); couleur(REINIT);
*/