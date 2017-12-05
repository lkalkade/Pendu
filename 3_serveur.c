/*
Serveur du Jeu du pendu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <ctype.h>


#include "dico.c"


#define nom_tube_appel "appel" //son nom ne peut être modifier

#define TAILLETAUX 10
#define TAILLE_MOT 10

#define TAILLE_STR 100
#define MAXCLI 500


#define DEBUG 1

int piocherMot(char *motPioche);
int nombreAleatoire(int nombreMax);

int gagne(int lettreTrouvee[], long tailleMot);
int rechercheLettre(char lettre, char motSecret[], int lettreTrouvee[]);
char lireCaractere(char lettre);

char lireCaractere(char lettre)
{
    char caractere = lettre;
    
    caractere = toupper(caractere); // On met la lettre en majuscule si elle ne l'est pas déjà

    return caractere; // On retourne le premier caractère qu'on a lu
}


int gagne(int lettreTrouvee[], long tailleMot)
{
    long i = 0;
    int joueurGagne = 1;

    for (i = 0 ; i < tailleMot ; i++)
    {
        if (lettreTrouvee[i] == 0)
            joueurGagne = 0;
    }

    return joueurGagne;
}

int rechercheLettre(char lettre, char motSecret[], int lettreTrouvee[])
{

    long i = 0;
    int bonneLettre = 0;
         
    // On parcourt motSecret pour vérifier si la lettre proposée y est
    for (i = 0 ; motSecret[i] != '\0' ; i++)
    {
        if (lettre == motSecret[i]) // Si la lettre y est
        {
            bonneLettre = 1; // On mémorise que c'était une bonne lettre
            lettreTrouvee[i] = 1; // On met à 1 le case du tableau de booléens correspondant à la lettre actuelle
        }
    }
    
                    
    return bonneLettre;
}


int main(int argc, char* argv[])
{
    int tube_appel;
    char nomTube[MAXCLI][TAILLE_STR];
    char message[MAXCLI][TAILLE_STR];   
    char mot[TAILLE_STR];
    char pseudos[MAXCLI][TAILLE_STR];
    char score[MAXCLI][TAILLE_STR];  
    
    int pid;
	
    pid_t fils;

    int tube[MAXCLI];
    int numcli;
    char result[TAILLE_STR];
    
    int i, points,continuer,choix;
    
    char lettre = 0; // Stocke la lettre proposée par l'utilisateur (retour du scanf)
    char motSecret[100] = {0}; // Ce sera le mot à trouver
    int *lettreTrouvee = NULL; // Un tableau de booléens. Chaque case correspond à une lettre du mot secret. 0 = lettre non trouvée, 1 = lettre trouvée
    long coupsRestants = 10; // Compteur de coups restants (0 = mort)
    //long i = 0; // Une petite variable pour parcourir les tableaux
    long tailleMot = 0;
    //char mot[100]={0};

    numcli=0;
    points=0;
    continuer=1;

    // Creation tube appel
    mkfifo("/tmp/serveur", 0666);


while(1){

    /*connexion client*/

    // Ouverture tube d'appel
    tube_appel = open("/tmp/serveur", O_RDONLY);
    if (tube_appel < 0)
    {
            perror("open tube d'appel ");
            exit(EXIT_FAILURE);
    }
        
    // Lecture du tube d'appel
    if (read(tube_appel, message[numcli], TAILLE_STR) < 0)
    {
            perror("read tube d'appel ");
            exit(EXIT_FAILURE);
    }
    close(tube_appel);

    sscanf(message[numcli], "%s %s", nomTube[numcli], pseudos[numcli]);

    printf("Connexion de %s sur le tube %s \n", pseudos[numcli], nomTube[numcli]);

    numcli++;

    //renvoie le id du client au client
	if((fils=fork())==0){while(continuer==1){
        printf("moi %d m'occupe de %d \n", getpid(), numcli-1);
    /*à chaque connection d'un client on pioche un mot*/
    if (!piocherMot(motSecret))
        exit(0);

    tailleMot = strlen(motSecret);

    lettreTrouvee = malloc(tailleMot * sizeof(int)); // On alloue dynamiquement le tableau lettreTrouvee (dont on ne connaissait pas la taille au départ)
    if (lettreTrouvee == NULL)
        exit(0);

    for (i = 0 ; i < tailleMot ; i++)
        lettreTrouvee[i] = 0;

    // On continue à jouer tant qu'il reste au moins un coup à jouer ou qu'on
    // n'a pas gagné
    while (coupsRestants > 0 && !gagne(lettreTrouvee, tailleMot))
    {
        printf("\n\nIl vous reste %ld coups a jouer\n", coupsRestants);
        //printf("\nQuel est le mot secret ? ");

        // On affiche le mot secret en masquant les lettres non trouvées
        //Exemple : *A**ON 
        for (i = 0 ; i < tailleMot ; i++)
        {
            if (lettreTrouvee[i]){ // Si on a trouvé la lettre n°i
                mot[i]=motSecret[i];// On l'affiche
                }

            else
                mot[i]='*';
                // Sinon, on affiche une étoile pour les lettres non trouvées
        }

            fflush(stdin);
	    fflush(stdout);
         if (DEBUG == 1)
            {
                printf("Le resultat du serveur est: %s. \n", mot);
            }
            
            // Ouverture du tube client
            tube[numcli-1] = open(nomTube[numcli-1], O_WRONLY);
            if (tube[numcli-1] < 0)
            {
                perror("open tube client ");
                exit(EXIT_FAILURE);
            }
            
            // Ecriture de la reponse a la requete dans le tube client
            if (write(tube[numcli-1], mot, strlen(mot)+1) < 0)
            {
                perror("write tube client ");
                exit(EXIT_FAILURE);
            }
            close(tube[numcli-1]);
        
            
        // Ouverture tube
        tube[numcli-1] = open(nomTube[numcli-1], O_RDONLY);
        if (tube[numcli-1] < 0)
        {
            perror("open tube");
            exit(EXIT_FAILURE);
        }
        
        // Lecture du tube
        if (read(tube[numcli-1], message[numcli-1], TAILLE_STR) < 0)
        {
            perror("read tube");
            exit(EXIT_FAILURE);
        }
        close(tube[numcli-1]);


        printf("La lettre reçu est %s\n", message[numcli-1]);

        
         for (i = 0 ; message[numcli-1][i] != '\0' ; i++)
        {    
            lettre = message[numcli-1][i]; 
        }
        
        // Si ce n'était PAS la bonne lettre
        if (!rechercheLettre(lettre, motSecret, lettreTrouvee))
        {            
            coupsRestants--; // On enlève un coup au joueur
        }
    }


    if (gagne(lettreTrouvee, tailleMot))
    {
	points++;
	sprintf(score[numcli-1],"%d",points);	
	
	
        printf("\n\nGagne ! Le mot secret etait bien : %s %d\n", motSecret,points);
        sprintf(result, "Gagne ! Le mot secret etait bien : %s \nScore:%s", motSecret,score[numcli-1]);
	
    }
    else
    {
        printf("\n\nPerdu ! Le mot secret etait : %s\n", motSecret);
        sprintf(result, "Perdu ! Le mot secret etait : %s", motSecret);
    }

    // Ouverture du tube client
    tube[numcli-1] = open(nomTube[numcli-1], O_WRONLY);
    if (tube[numcli-1] < 0)
    {
        perror("open tube client ");
        exit(EXIT_FAILURE);
    }
            
    // Ecriture de la reponse a la requete dans le tube client
    if (write(tube[numcli-1], result, strlen(result)+1) < 0)
    {
        perror("write tube client ");
        exit(EXIT_FAILURE);
    }
    close(tube[numcli-1]);

    free(lettreTrouvee); // On libère la mémoire allouée manuellement (par malloc)
    continuer=0;
    if (continuer==0)
    {
	// Ouverture tube d'appel
        tube[numcli-1] = open(nomTube[numcli-1], O_RDONLY);
        if (tube[numcli-1] < 0)
        {
            perror("open tube d'appel ");
            exit(EXIT_FAILURE);
        }
        
        // Lecture du tube d'appel
        if (read(tube[numcli-1], message[numcli-1], TAILLE_STR) < 0)
        {
            perror("read tube d'appel ");
            exit(EXIT_FAILURE);
        }
        close(tube[numcli-1]);
	sscanf(message[numcli-1],"%d",&choix);
	continuer=choix;
	
    }
    else
    	exit(EXIT_SUCCESS);
	}
    } //fin de fils
    }
    return 0;
}
