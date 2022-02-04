
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
} 
requete_garage;


typedef struct 
{
	long type;
	long nb_outil[4];

} 
requete_chef_vers_mecanicien;



typedef struct 
{
	long type;
	int resu;
} 
reponse_garage;

typedef struct 
{
	long type;
	int temps_seconde;
} 
notification;

#define FICHIER_CLE "./files_cle/cle.chef_"
#define FICHIER_CLE_MECA "./files_cle/cle.meca_"
#define FICHIER_CLE_SEM "./files_cle/cle.sem" 




/* 
   Utilisation (pour ecrire en rouge) :
   couleur(ROUGE); fprintf(stdout,"Hello\n"); couleur(REINIT);
*/