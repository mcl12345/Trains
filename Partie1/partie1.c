#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // pour les semaphores
#include <fcntl.h>     // pour les flags O_CREAT, O_EXCL, ...
#include <unistd.h>    // sleep()
#include <string.h>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

#define _TEMPS_ 3

/*création des mutex*/
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fifo   = PTHREAD_MUTEX_INITIALIZER;

clock_t temps; // Le temps
//Declare un nombre de train dans le voie, pour permettre à avoir un unique train de circuler
int nb_train1 = 0; 
int nb_train2 = 0; 
int nb_train3 = 0; 

//Declare des elements de debut_train, fin_train
//debut_train, fin_train sont comme le trajet a fini
//possible_debut_train, possible_fin_train sont comme le trajet va faire
char debut_train1[2],possible_debut_train1[2];
char fin_train1[2],possible_fin_train1[2];
char debut_train2[2],possible_debut_train2[2];
char fin_train2[2],possible_fin_train2[2];
char debut_train3[2],possible_debut_train3[2];
char fin_train3[2],possible_fin_train3[2];

//Permet de limiter le calcul de la moyenne à 3 fois
int rentre_train_un     = 0;
int rentre_train_deux   = 0;
int rentre_train_trois = 0;

// Récupère le caractère du début et de la fin de la chaine de caractere (ex: A --> B , donne A et B )
char *substring(size_t start, size_t stop, const char *src, char *dst, size_t size)
{
   int count = stop - start;
   if ( count >= --size ) {
      count = size;
   }
   sprintf(dst, "%.*s", count, src + start);
   return dst;
}

//Décide deux trains si sont en inverse(ex: Train1 A-->B, Train2 B --> A)
int train_en_inverse(char *possible_debut1,char* possible_fin1,char* possible_debut2, char* possible_fin2 ){
    if(!strcmp(possible_debut1,possible_fin2) && !strcmp(possible_fin1,possible_debut2)){
        return 1;
    }
    return 0;
}
//Décide deux trains si sont en même sens (ex: Train1 et Train2: A-->B)
int train_en_ligne(char *possible_debut1,char* possible_fin1,char* possible_debut2, char* possible_fin2){
    if(!strcmp(possible_debut1,possible_debut2) && !strcmp(possible_fin1,possible_fin2)){
        return 1;
    }
    return 0;
}

//Train 1
void* TrainUn(void *p){
    int i = 0;
    char train1[4][8] = {"A --> B", "B --> C", "C --> B", "B --> A"};
    
    while (i >= 0 ){
        // Récupère le trajet du train 1 en global dans debut_train1 et dans fin_train1 :
        substring(0, 1, train1[i%4], possible_debut_train1, sizeof(possible_debut_train1));
        substring(6, 1, train1[i%4], possible_fin_train1, sizeof(possible_fin_train1));
        
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train1, "B") == 0 && strcmp(fin_train1, "A") == 0) {
            clock_t temps_moyen = clock();
            if (rentre_train_un == 0) {
              printf("\nLe temps moyen du trajet du train 1 est de : %f secondes\n\n", (double) temps_moyen / 100- 6);
            }
            else if (rentre_train_un == 1) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 1 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 2);
            }
            else if (rentre_train_un == 2) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 1 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 3);
            }
            rentre_train_un = rentre_train_un + 1;
        }

        
        printf("Le train1 doit aller le trajet: %s\n", train1[i%4]);
        pthread_mutex_lock(&fifo); // file d'attente Train2 et Train3
        pthread_mutex_lock(&mutex1);
        nb_train1++;//Train1 va aller le trajet
        if (nb_train1 == 1){
            //On verrouille le mutex d'Train2 et Train3 dans la direction en inverse ou le même trajet
            /*Deux cas dans la direction en inverse: 
             * 1. Train1 et Train2 veulent deux directionS en inverse (ex: Train1 veut aller A->B, Train2 veux aller B->A)
             * 2. Train1 a fini le trajet A->B mais Train2 veux aller B->A
             *Troisème cas en le même trajet (ex Train1 et Train2 veulent aller A->B)
             */
            if (train_en_inverse(possible_debut_train1,possible_fin_train1,possible_debut_train2,possible_fin_train2)||
                (train_en_inverse(debut_train1,fin_train1,possible_debut_train2,possible_fin_train2) && 
                !train_en_inverse(possible_debut_train1,possible_fin_train1,possible_debut_train2,possible_fin_train2))||
                train_en_ligne(possible_debut_train1,possible_fin_train1,possible_debut_train2,possible_fin_train2))
            {
                pthread_mutex_lock(&mutex2);
            }
            if (train_en_inverse(possible_debut_train1,possible_fin_train1,possible_debut_train3,possible_fin_train3)||
                (train_en_inverse(debut_train1,fin_train1,possible_debut_train3,possible_fin_train3) &&
                !train_en_inverse(possible_debut_train1,possible_fin_train1,possible_debut_train3,possible_fin_train3))||
                train_en_ligne(possible_debut_train1,possible_fin_train1,possible_debut_train3,possible_fin_train3))
            {
                pthread_mutex_lock(&mutex3);
            }
            
            
        } 
        pthread_mutex_unlock(&mutex1);//on verrouille le mutex de Train1 pour déverrouiller le mutex de Train2 et Train3
        pthread_mutex_unlock(&fifo);
        strcpy(debut_train1,possible_debut_train1);//Train1 est parti, donne le valeur pour debut_train1
        
         // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6);    // et l'affiche


        printf("Le train1 fait le trajet: %s\nLe train1 est parti de la gare: %s\n\n",train1[i%4],debut_train1);
        pthread_mutex_lock(&mutex1);
        strcpy(fin_train1,possible_fin_train1);//Train1 est arrivé, donne le valeur pour fin_train1
        
        // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6); // et l'affiche
        
        printf("Le train1 a fini le trajet : %s\nLe train1 est arrivé à la gare:%s\n\n",train1[i%4],fin_train1);
        nb_train1--;//Train1 a fini le trajet
        
        // on déverrouille le mutex de Train2 et Train3
        if(nb_train1 == 0){
            pthread_mutex_unlock(&mutex2);
            pthread_mutex_unlock(&mutex3);
        }
        i++;//Le trajet suivant
        pthread_mutex_unlock(&mutex1);
        sleep(rand() % 3); //Train1 fait une pause
       
    } 
    return NULL;
}

//Train 2
void* TrainDeux(void *p){
    int i = 0;
    char train2[5][8] = {"A --> B", "B --> D", "D --> C", "C --> B", "B --> A"};
    
    while (i >= 0 ){
        // Récupère le trajet du train 1 en global dans debut_train1 et dans fin_train1 :
        substring(0, 1, train2[i%5], possible_debut_train2, sizeof(possible_debut_train2));
        substring(6, 1, train2[i%5], possible_fin_train2, sizeof(possible_fin_train2));
        
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train2, "B") == 0 && strcmp(fin_train2, "A") == 0) {
            clock_t temps_moyen = clock();
            if (rentre_train_deux == 0) {
              printf("\nLe temps moyen du trajet du train 2 est de : %f secondes\n\n", (double) temps_moyen / 100- 6);
            }
            else if (rentre_train_deux == 1) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 2 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 2);
            }
            else if (rentre_train_deux == 2) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 2 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 3);
            }
            rentre_train_deux = rentre_train_deux + 1;
        }


        printf("Le train2 doit faire le trajet: %s\n", train2[i%5]);
        pthread_mutex_lock(&fifo);// file d'attente Train1 et Train3
        pthread_mutex_lock(&mutex2);
        nb_train2++;//Train2 va aller le trajet
        if (nb_train2 == 1){
            //On verrouille le mutex d'Train1 et Train2 dans la direction en inverse ou le même trajet
            /*Deux cas dans la direction en inverse: 
             * 1. Train2 et Train1 veulent deux directions en inverse (ex: Train2 veut aller A->B, Train1 veux aller B->A)
             * 2. Train2 a fini le trajet A->B mais Train1 veux aller B->A
             *Troisème cas en le même trajet (ex Train1 et Train2 veulent aller A->B)
             */
            if (train_en_inverse(possible_debut_train2,possible_fin_train2,possible_debut_train1,possible_fin_train1)||
                (train_en_inverse(debut_train2,fin_train2,possible_debut_train1,possible_fin_train1) && 
                !train_en_inverse(possible_debut_train2,possible_fin_train2,possible_debut_train1,possible_fin_train1)) ||
                train_en_ligne(possible_debut_train2,possible_fin_train2,possible_debut_train1,possible_fin_train1))
            {
                pthread_mutex_lock(&mutex1);
            }
            if (train_en_inverse(possible_debut_train2,possible_fin_train2,possible_debut_train3,possible_fin_train3)||
                (train_en_inverse(debut_train2,fin_train2,possible_debut_train3,possible_fin_train3) &&
                !train_en_inverse(possible_debut_train2,possible_fin_train2,possible_debut_train3,possible_fin_train3))||
                train_en_ligne(possible_debut_train2,possible_fin_train2,possible_debut_train3,possible_fin_train3))
            {
                pthread_mutex_lock(&mutex3);
            }  
        } 
        pthread_mutex_unlock(&mutex2);//on verrouille le mutex de Train2 pour déverrouiller le mutex de Train1 et Train3
        pthread_mutex_unlock(&fifo);
        strcpy(debut_train2,possible_debut_train2);//Train2 est parti, donne le valeur pour debut_train2
        
        // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6); // et l'affiche
        
        printf("Le train2 fait le trajet: %s\nLe train2 est parti de la gare: %s\n\n",train2[i%5],debut_train2);
        pthread_mutex_lock(&mutex2);
        strcpy(fin_train2,possible_fin_train2);//Train2 est arrivé, donne le valeur pour fin_train2
        
        // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6); // et l'affiche
        
        printf("Le train2 a fini le trajet : %s\nLe train2 est arrivé à la gare:%s\n\n",train2[i%5],fin_train2);
       
        nb_train2--;//Train2 a fini le trajet
        
        if(nb_train2 == 0){
           pthread_mutex_unlock(&mutex1);
           pthread_mutex_unlock(&mutex3); 
        }
    
        i++;//Le trajet suivant
        pthread_mutex_unlock(&mutex2);
        sleep(rand() % 3); //Train2 fait une pause
    }
    return NULL;
}

//Train 3
void* TrainTrois(void *p){
    int i = 0;
    char train3[5][8] = {"A --> B", "B --> D", "D --> C", "C --> E", "E --> A"};
    
    while (i >= 0 ){
        // Récupère le trajet du train 1 en global dans debut_train1 et dans fin_train1 :
        substring(0, 1, train3[i%5], possible_debut_train3, sizeof(possible_debut_train3));
        substring(6, 1, train3[i%5], possible_fin_train3, sizeof(possible_fin_train3));
        
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train3, "E") == 0 && strcmp(fin_train3, "A") == 0) {
            clock_t temps_moyen = clock();
            if (rentre_train_trois == 0) {
              printf("\nLe temps moyen du trajet du train 3 est de : %f secondes\n\n", (double) temps_moyen / 100- 6);
            }
            else if (rentre_train_trois == 1) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 3 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 2);
            }
            else if (rentre_train_trois == 2) {
                temps_moyen = clock();
                printf("\nLe temps moyen du trajet du train 3 est de : %f secondes\n\n", (double) (temps_moyen / 100 - 6) / 3);
            }
            rentre_train_trois = rentre_train_trois + 1;
        }
        
        printf("Le train3 doit faire le trajet: %s\n", train3[i%5]);
        pthread_mutex_lock(&fifo);
        pthread_mutex_lock(&mutex3);
        nb_train3++;
        
        if (nb_train3 == 1){
                //Direction inverse ou la meme trajet
            if (train_en_inverse(possible_debut_train3,possible_fin_train3,possible_debut_train1,possible_fin_train1)||
                (train_en_inverse(debut_train3,fin_train3,possible_debut_train1,possible_fin_train1) &&
                !train_en_inverse(possible_debut_train3,possible_fin_train3,possible_debut_train1,possible_fin_train1))||
                train_en_ligne(possible_debut_train3,possible_fin_train3,possible_debut_train1,possible_fin_train1))
            {
                pthread_mutex_lock(&mutex1);
            }
            if (train_en_inverse(possible_debut_train3,possible_fin_train3,possible_debut_train2,possible_fin_train2)||
                (train_en_inverse(debut_train3,fin_train3,possible_debut_train2,possible_fin_train2) &&
                !train_en_inverse(possible_debut_train3,possible_fin_train3,possible_debut_train2,possible_fin_train2))||
                train_en_ligne(possible_debut_train3,possible_fin_train3,possible_debut_train2,possible_fin_train2))
            {
                pthread_mutex_lock(&mutex2);
            } 
        } 
        pthread_mutex_unlock(&mutex3);
        pthread_mutex_unlock(&fifo);
        strcpy(debut_train3,possible_debut_train3);
        
        // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6); // et l'affiche

        printf("Le train3 fait le trajet: %s\nLe train3 est parti de la gare: %s\n\n",train3[i%5],debut_train3);
        pthread_mutex_lock(&mutex3);
        strcpy(fin_train3,possible_fin_train3);
        
        // Récupère le temps
        temps = clock();
        printf("%f - ", (double) temps / 100 - 6); // et l'affiche
        
        printf("Le train3 a fini le trajet : %s\nLe train3 est arrivé à la gare:%s\n\n",train3[i%5],fin_train3);
        nb_train3--;
        
        if(nb_train3 == 0){
             pthread_mutex_unlock(&mutex1);
             pthread_mutex_unlock(&mutex2);
        }
        i++;
        pthread_mutex_unlock(&mutex3);
        sleep(rand() % 3); //Train3 fait une pause
        
    }
    
    return NULL;
}

int main(){
    int i;
    srand(time(NULL)); // initialisation de rand
    pthread_t tid[3];
    
    pthread_create(&tid[0], NULL, TrainUn,NULL);
    pthread_create(&tid[1], NULL, TrainDeux,NULL);
    pthread_create(&tid[2], NULL, TrainTrois,NULL);
	

    for (i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&fifo);
        
    return 0;
}