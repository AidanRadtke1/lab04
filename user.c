/*----------------------------------------------------------------------
Demo of IPC using Message Queues:  The User process

Written By: Team-00
    1- Dr. Mohamed Aboutabl
    2- Dr. Mohamed Aboutabl

Submitted on: mm/dd/yyyy
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include "message.h"
/* -------------------------------------------------------------------*/
int main ( int argc , char * argv[] )
{

    int     queID ;
    int     msgStatus , msgflags ;

    if( argc < 4 )
    {
        printf("Missing command-line arguments\n") ;
        exit(-1) ;
    }    
    printf("This is the user process (id = %d).\t\n" , getpid() );

    /* Find the message queue already created by the Calculator process */
	msgflags  = S_IRUSR  |  S_IWUSR  ;
    queID     = msgget( BASE_MAILBOX_NAME , msgflags ) ;
    if ( queID == -1 ) {
        printf("User Failed to find mailbox %X. Error code=%d\n" 
               , BASE_MAILBOX_NAME , errno ) ;
        perror("Reason");
        exit(-2) ;  
    }
    else
        printf( "User found mailbox %0X with qid=%d\n" 
               , BASE_MAILBOX_NAME , queID ) ;

    pid_t clientPID     = getpid() ;
    msgBuf  msg1 , msg2 ;
    
    /* prepare a message to send to the Calculator process */
    msg1.msgType        = 1 ;  /* this is a "Request" message */
    msg1.body.sender    = clientPID ; 
    msg1.body.num1      = atoi( argv[1] )  ;  
    msg1.body.num2      = atoi( argv[3] ) ; 
    msg1.body.operation = *argv[2] ;
    

    /* Send one message to the Calculator process */
	msgflags = 0 ;  // Block me if queue has no room to send

    msgStatus = msgsnd( queID , &msg1 , MSG_INFO_SIZE , msgflags ) ; 

    if ( msgStatus != 0 ) 
    {
        printf("User Failed to send on queuID %d. Error code=%d\n" 
               , queID , errno ) ;
        perror( "Reason" );
        exit( -2 ) ;  
    }
    else 
    {
        printf("\nUser sent this message to  Calculator\n" );
        printMsg( & msg1 );
    }

    /* Now, wait for a message to arrive from the Calculator process */
    printf ("\nUser is now waiting to receive result ...\n" );

	msgflags = 0 ;  // Block me until I get a message
    msgStatus = msgrcv( queID , &msg1 , MSG_INFO_SIZE , clientPID , msgflags );     
    if ( msgStatus < 0 ) 
    {
        printf("User Failed to receive message on queuID %d. Error code=%d\n" 
               , queID , errno ) ;
        perror("Reason");
        exit(-2) ;  
    }
    else 
    {
        printf("User received this message ( %d bytes ) :\n" , msgStatus );
        printMsg( & msg1 );
    }
    
    return 0 ;
}

