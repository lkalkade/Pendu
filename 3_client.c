/*
Client du Jeu du pendu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define nom_tube_appel "appel"

#define TAILLE_STR 100
#define TAILLE_MOT 10

#define DEBUG 1

int main(int argc, char* argv[])
{
    int tube_appel;
    char nomTube[TAILLE_STR];
    char lettre[TAILLE_MOT];
    char message[TAILLE_STR];
    char pseudo[TAILLE_STR];
    
    char result[TAILLE_STR];
    
    float montant;
    
    int tube;
    int pid;
    int continuer;

    // Ecriture de la requete

    printf("Entrez un pseudo:\n");
    scanf("%s", pseudo);
    
    // Ecriture de la requete selon le protocole suivant : "PID LETTRE"
    if (sprintf(message, "/tmp/rep%d %s", getpid(), pseudo) < 0)
    {
        perror("Ecriture de la requete ");
        exit(EXIT_FAILURE);
    }

    printf("Bienvenue dans le Pendu !\n\n");

        // Ouverture tube d'appel
        tube_appel = open("/tmp/serveur", O_WRONLY);
        if (tube_appel < 0)
        {
            perror("open tube d'appel ");
            exit(EXIT_FAILURE);
        }

        // Ecriture de la requete dans le tube d'appel
        if (write(tube_appel, message, strlen(message)+1) < 0)
        {
            perror("write tube d'appel ");
            exit(EXIT_FAILURE);
        }
        close(tube_appel);

    // Ecriture dans un str du nom du tube client
    if (sprintf(nomTube, "/tmp/rep%d", getpid()) < 0)
    {
        perror("Ecriture du nom du tube client ");
        exit(EXIT_FAILURE);
    }
    
    mkfifo(nomTube, 0666);

            

    while (1)
    {

    // Ouverture du tube
    tube = open(nomTube, O_RDONLY);
    if (tube < 0)
    {
            perror("open tube");
            exit(EXIT_FAILURE);
    }
    
    // Lecture du tube
    if (read(tube, result, TAILLE_STR) < 0)
    {
            perror("read tube");
            exit(EXIT_FAILURE);
    }
    close(tube);
        
    if ((strstr(result, "Gagne") != NULL) || (strstr(result, "Perdu") != NULL))
    {
        printf("%s\n", result);
	printf("\n\nVoulez vous continuer ? Oui(1) ou Non(0)\n");
	scanf("%d",&continuer);
	sprintf(lettre,"%d",continuer);
	if(continuer==1)
	{
		// Ouverture tube
	        tube = open(nomTube, O_WRONLY);
	        if (tube < 0)
	        {
	            perror("open tube");
	            exit(EXIT_FAILURE);
	        }

	        // Ecriture du choix dans le tube
	        if (write(tube, lettre, strlen(lettre)+1) < 0)
	        {
	            perror("write tube");
	            exit(EXIT_FAILURE);
	        }
	        close(tube);
		// Ouverture du tube
    		tube = open(nomTube, O_RDONLY);
   		 if (tube < 0)
    		{
            		perror("open tube");
            		exit(EXIT_FAILURE);
    		}
    
    		// Lecture du tube
    		if (read(tube, result, TAILLE_STR) < 0)
    		{
        	    perror("read tube d'appel ");
        	    exit(EXIT_FAILURE);
    		}
    		close(tube);
	
	}
	else{
		printf("Fin de la partie\n");
        	break;
	}
    }

    printf("Le mot a trouvee est %s. \n", result);

        printf("\nProposez une lettre : ");
        scanf("%s", lettre);

        // Ouverture tube
        tube = open(nomTube, O_WRONLY);
        if (tube < 0)
        {
            perror("open tube");
            exit(EXIT_FAILURE);
        }

        // Ecriture de la requete dans le tube
        if (write(tube, lettre, strlen(lettre)+1) < 0)
        {
            perror("write tube");
            exit(EXIT_FAILURE);
        }
        close(tube);

        if (DEBUG == 1)
        {
            printf("Le message a envoyer est '%s'. \n", lettre);
        }

    }
     
   
    
    return 0;
}
