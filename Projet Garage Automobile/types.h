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
	int temps_seconde;
} 
notification;

#define FICHIER_CLE "./files_cle/cle.chef_"
#define FICHIER_CLE_MECA "./files_cle/cle.meca_"
#define FICHIER_CLE_SEM "./files_cle/cle.sem" 
#define FICHIER_CLE_SEM_OCCUPATION "./files_cle/cle.occupation_sem" 


#define couleur(param) fprintf(stdout,"\033[%sm",param)
#define NOIR  "30"
#define ROUGE "31"
#define GRIS "37"
#define VERT  "32"
#define JAUNE "33"
#define BLEU  "34"
#define CYAN  "36"
#define BLANC "37"
#define REINIT "0"


/* 
   Utilisation (pour ecrire en rouge) :
   couleur(ROUGE); fprintf(stdout,"Hello\n"); couleur(REINIT);
*/