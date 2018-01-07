/*

 *
 *       Filename:  writer.c
 *
 *      Description: ecrit et affiche des messsages de trains
 *
 *        Version:  1.0
 *        Created:  01/02/2018 
 *       Revision:  none
 *       Compiler:  gcc(g++)
                    $gcc -Wall writer.c -o writer -lrt
 *                  $./writer
 *
 *         Author:  |Jiangning LIN|, |linjiangning90@gmail.com|
                    |Morvan CALMEL|, |morvan.calmel@gmail.com|
 *
 */

#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAXSIZE     1024   //definie la taille de buf
#define BUFFER      8192 //definie la taille de Msg
#define STOP        "q"

char debut_train1[2];
char fin_train1[2];
char debut_train2[2];
char fin_train2[2];
char debut_train3[2];
char fin_train3[2];

int rentre_train_un     = 0;
int rentre_train_deux   = 0;
int rentre_train_trois  = 0;

int a=0, b=0, c=0;
int stop = 0;
char stream[1024];
//Le temps
clock_t temps;
clock_t start,finish; 
double TheTimes; 

char *substring(size_t start, size_t stop, const char *src, char *dst, size_t size)
{
   int count = stop - start;
   if ( count >= --size ) {
      count = size;
   }
   sprintf(dst, "%.*s", count, src + start);
   return dst;
}

void TrainUn(){
    char train1[4][8] = {"A --> B", "B --> C", "C --> B", "B --> A"};
    if (a >= 0) {
        // Récupère le trajet du train 1 en global :
        substring(0, 1, train1[a%4], debut_train1, sizeof(debut_train1));
        substring(6, 1, train1[a%4], fin_train1, sizeof(fin_train1));

        // Compare avec les autres trains le trajet 
        if(strcmp(debut_train1, fin_train2) == 0 && strcmp(fin_train1, debut_train2) == 0) {
            sprintf(stream,"Train 1 en approche en sens inverse %s\n", train1[a%4] );
        }
        // Compare avec les autres trains le trajet 
        else if(strcmp(debut_train1, fin_train3) == 0 && strcmp(fin_train1, debut_train3) == 0) {
            sprintf(stream,"Train 1 en approche en sens inverse %s\n", train1[a%4] );
        } 
        
        //affiche les trajets de train1
        int i = 0;
        temps = clock() / 100;
        i  = sprintf(stream,"temps:%f\n", (double) temps);
        i += sprintf(stream+i,"Train 1 : %s\n", train1[a%4]);
        // Temps du trajet de 1 à 3 secondes
        sleep(rand()%3+1);
        i += sprintf(stream+i,"Le train 1 est arrivé à la gare : %s\n", fin_train1);
        stream[i-1]='\n';
        
        
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train1, "B") == 0 && strcmp(fin_train1, "A") == 0 && rentre_train_un == 0) {
            //s'evite le dernier trajet n'affiche pas
            temps = clock()/100;
            i  = sprintf(stream,"temps:%f\n", (double) temps);
            i += sprintf(stream+i,"Train 1 : %s\n", train1[(a%4)]);
            sleep(rand()%3+1);
            i += sprintf(stream+i,"Le train 1 est arrivé à la gare : %s\n", fin_train1);
            
            finish=clock();  
            TheTimes=(double)((finish-start)/CLOCKS_PER_SEC); 
            sprintf(stream+i,"Le temps moyen du train 1 est de : %f secondes\n", TheTimes);
            stream[i-1]='\n';
            rentre_train_deux = 1;
            i = 0;
        }

        fflush(stdout);
        a++;
    }
}

void TrainDeux() {
    char train2[5][8] = {"A --> B", "B --> D", "D --> C", "C --> B", "B --> A"};
    start=clock();
    if (b >= 0) {
        // Récupère le trajet du train 2 en global :
        substring(0, 1, train2[b%5], debut_train2, sizeof(debut_train2));
        substring(6, 1, train2[b%5], fin_train2, sizeof(fin_train2));
        

        // Compare avec les autres trains le trajet 
        if(strcmp(debut_train2, fin_train3) == 0 && strcmp(fin_train2, debut_train3) == 0) {
          sprintf(stream,"Train 2 en approche en sens inverse %s\n" , train2[b%5]);
        }
        // Compare avec les autres trains le trajet
        else if(strcmp(debut_train2, fin_train1) == 0 && strcmp(fin_train2, debut_train1) == 0) {
          sprintf(stream,"Train 2 en approche en sens inverse %s\n" , train2[b%5]);
        }
        //Affiche les trajets
        int i = 0;
        temps = clock() / 100;
        i  = sprintf(stream,"temps:%f\n", (double) temps);
        i += sprintf(stream +i,"Train 2 : %s\n", train2[(b%5)]);
        sleep(rand()%3+1);
        i += sprintf(stream+i,"Le train 2 est arrivé à la gare : %s\n", fin_train2);
        stream[i-1]='\n';
          
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train2, "B") == 0 && strcmp(fin_train2, "A") == 0 && rentre_train_deux == 0) {
            
            temps = clock()/100;
            i  = sprintf(stream,"temps:%f\n", (double) temps);
            i += sprintf(stream+i,"Train 2 : %s\n", train2[(b%5)]);
            sleep(rand()%3+1);
            i += sprintf(stream+i,"Le train 2 est arrivé à la gare : %s\n", fin_train3);
            finish=clock();  
            TheTimes=(double)((finish-start)/CLOCKS_PER_SEC); 
            sprintf(stream+i,"Le temps moyen du train 2 est de : %f secondes\n", TheTimes);
            stream[i-1]='\n';
            rentre_train_deux = 1;
            i = 0;
        }
        fflush(stdout);
        b++;
    }
}

void TrainTrois() {
    char train3[5][8] = {"A --> B", "B --> D", "D --> C", "C --> E", "E --> A"};
    int i = 0 ;
    start=clock(); 
    if (c >= 0) {
        // Récupère le trajet du train 3 en global :
        substring(0, 1, train3[c%5], debut_train3, sizeof(debut_train3));
        substring(6, 1, train3[c%5], fin_train3, sizeof(fin_train3));
        
        // Compare avec les autres trains le trajet :
        if(strcmp(debut_train3, fin_train2) == 0 && strcmp(fin_train3, debut_train2) == 0) {
          sprintf(stream,"Train 3 en approche en sens inverse %s\n", train3[(c%5)] );
        }
        if(strcmp(debut_train3, fin_train1) == 0 && strcmp(fin_train3, debut_train1) == 0) {
          sprintf(stream,"Train 3 en approche en sens inverse %s\n", train3[(c%5)]);
        }
         
         temps = clock() / 100;
         i  = sprintf(stream,"temps:%f\n", (double) temps);
         i += sprintf(stream+i,"Train 3 : %s\n", train3[(c%5)]);
         sleep(rand()%3+1);
         sprintf(stream+i,"Le train 3 est arrivé à la gare : %s\n", fin_train3);
         stream[i-1]='\n';
         fflush(stdout);
         i = 0;
        // Récupère le temps moyen d'un trajet complet
        if ( strcmp(debut_train3, "E") == 0 && strcmp(fin_train3, "A") == 0 && rentre_train_trois == 0) {
            temps = clock()/100;
            i  = sprintf(stream,"temps:%f\n", (double) temps);
            i += sprintf(stream+i,"Train 3 : %s\n", train3[(c%5)]);
            sleep(rand()%3+1);
            i += sprintf(stream+i,"Le train 3 est arrivé à la gare : %s\n", fin_train3);
            stream[i-1]='\n';
            
            finish=clock();  
            TheTimes=(double)((finish-start)/CLOCKS_PER_SEC); 
            sprintf(stream+i,"Le temps moyen du train 3 est de : %f secondes\n", TheTimes);
            rentre_train_trois = 1;
            i = 0;
        }
        fflush(stdout);
        c++;
    }
}


struct MsgType{
    int len;
    char buf[MAXSIZE+1];
};

int main()
{
    /*File d'attente*/
    mqd_t msgq_id;
    struct MsgType msg;

    unsigned int prio = 1;

    struct mq_attr msgq_attr;
    const char *file = "/QUEUE";
    
    /* initialize the queue attributes */
    msgq_attr.mq_flags = 0;
    msgq_attr.mq_maxmsg = 1024;
    msgq_attr.mq_msgsize = sizeof(msg);
    msgq_attr.mq_curmsgs = 0;

    
    /*mq_open() for creating a new queue (using default attributes) */
    msgq_id = mq_open(file, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, NULL);
    if(msgq_id == (mqd_t)-1)
    {
        perror("mq_open");
        exit(1);
    }

    /* getting the attributes from the queue        --  mq_getattr() */
    if(mq_getattr(msgq_id, &msgq_attr) == -1)
    {
        perror("mq_getattr");
        exit(1);
    }

    printf("Queue \"%s\":\n\t- stores at most %ld messages\n\t- \
        large at most %ld bytes each\n\t- currently holds %ld messages\n\t- \
        with 3 trains\n",
        file, msgq_attr.mq_maxmsg, msgq_attr.mq_msgsize, msgq_attr.mq_curmsgs);
  
    /*setting the attributes of the queue        --  mq_setattr() */
    if(mq_setattr(msgq_id, &msgq_attr, NULL) == -1)
    {
        perror("mq_setattr");
        exit(1);
    }   

    int i = 0;
    int train = 0;
    do {
        train = rand()%3 +1;
        printf("train:%d\n",train);
        printf("> ");
        fflush(stdout);
        
        msg.len = i;
        memset(msg.buf, 0, MAXSIZE);
        
        //choisi le train
        if(train == 1){
            TrainUn();
        }else if (train == 2){
            TrainDeux();
        }else
            TrainTrois();
        
        strcpy(msg.buf,stream);;
        printf("msg.len = %d \n%s\n", msg.len, msg.buf);
        
        /*sending the message      --  mq_send() */
        /*mq_send(): Ajouter le message pointé par msg_ptr a la file de massages referencee par mqdes*/
        if(mq_send(msgq_id, (char*)&msg, sizeof(struct MsgType), prio) == -1)
        {
            perror("mq_send");
            exit(1);
        }
        i++;
        sleep(1);  
    }while(1);

    sleep(30); //En attente de la sortie du processus reader

    /*closing the queue        -- mq_close() */
    if(mq_close(msgq_id) == -1)
    {
        perror("mq_close");
        exit(1);
    }

    /*mq_unlink() Delete the message queue named name*/
    if(mq_unlink(file) == -1)
    {
        perror("mq_unlink");
        exit(1);
    }
    

    return 0;
}

 