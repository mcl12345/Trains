/*
 *
 *       Filename:  reader.c
 *
 *    Description:  lit et affiche des messages de trains
 *
 *        Version:  1.0
 *        Created:  01/02/2018
 *       Revision:  none
 *       Compiler:  gcc(g++)
 *                  $gcc -Wall reader.c -o reader -lrt
 *                  $./reader
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

struct MsgType{
    int len;
    char buf[MAXSIZE+1];
};

int main()
{
    /*File d'attente*/
    mqd_t msgq_id;
    struct MsgType msg;

    unsigned int sender;
    struct mq_attr msgq_attr;

    unsigned int recv_size = BUFFER;
    const char *file = "/QUEUE";

    /* mq_open() for opening an existing queue */
    msgq_id = mq_open(file, O_RDWR);
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
        large at most %ld bytes each\n\t- currently holds %ld messages\n",
        file, msgq_attr.mq_maxmsg, msgq_attr.mq_msgsize, msgq_attr.mq_curmsgs);

    if(recv_size < msgq_attr.mq_msgsize)
        recv_size = msgq_attr.mq_msgsize;

    int i = 0;
    while(1)  
    {
        msg.len = -1;
        memset(msg.buf, 0, MAXSIZE);
 
        /* getting a message */

        /*mq_receive() : Le message le plus ancien avec la priorité la plus élevée est supprimé de la file d'attente référencée par le descripteur mqdes et placé dans le tampon de msg_ptr.*/
        if (mq_receive(msgq_id, (char*)&msg, recv_size, &sender) == -1)
        {
            perror("mq_receive");
            exit(1);
        }

        printf("msg.len = %d \n%s\n", msg.len, msg.buf);

        i++;
        sleep(2);
    }

    if(mq_close(msgq_id) == -1)
    {
        perror("mq_close");
        exit(1);
    }

    return 0;
}