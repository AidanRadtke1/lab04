/*----------------------------------------------------------------------
Demo of IPC using Message Queues:  The Calculator process

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

    int     queID , msgStatus , result , forever , msgflags ;
	long    msgtype ;
    
    printf("This is the Calculator process (id = %d).\t\n" , getpid() );

    /* Exclusively Create the message queue. 
	   All processes by my user can send and receive */
	msgflags = IPC_CREAT | IPC_EXCL | S_IRUSR  |  S_IWUSR  ;
    queID = msgget( BASE_MAILBOX_NAME, msgflags ) ; 

    if ( queID == -1 ) {
        printf("Calculator Failed to create mailbox %X. Error code=%d\n" 
               , BASE_MAILBOX_NAME , errno ) ;
        perror("Reason");
        exit(-2) ;  
    }
    else
        printf( "Calculator created mailbox %X with qid=%d\n" 
               , BASE_MAILBOX_NAME , queID ) ;

    /* Now, wait for messages to arrive from the User process */
    forever = 1 ;
    while ( forever )
    {
        printf ("\nI am now waiting to receive messages ...\n" );

        msgBuf  msg ;
		msgtype  = 1 ;   // Which msgs I am interested in
		msgflags = 0 ;   // Block me until I receive a msg

        msgStatus = msgrcv( queID , &msg , MSG_INFO_SIZE , msgtype , msgflags );     
        if ( msgStatus < 0 ) 
        {
            printf("Calculator Failed to receive from User on queuID %d. Error code=%d\n"
                   , queID , errno ) ;
            perror("Reason");
            exit(-2) ;  
        }
        else 
        {
            printf("Calculator received this message ( %d bytes ) from User:\n" 
			        , msgStatus ) ;
            printMsg( & msg );
            printf("\n");
        }

        /* prepare the result to send back to the User process */
        switch ( msg.body.operation )
        {
            case '+':   result = msg.body.num1 + msg.body.num2 ;
                        break ;
                        
            case '-':   result = msg.body.num1 - msg.body.num2 ;
                        break ;
                        
            case 'x':   
            case 'X':   result = msg.body.num1 * msg.body.num2 ;
                        break ;
                        
            case '/':   result = msg.body.num1 / msg.body.num2 ;
                        break ;
                        
            default:    printf("\nINVALID operation\n" );
                        result = -9999 ;
                        forever = 0 ;
        } ;    
        
        pid_t clientPID  =  msg.body.sender; 
        msg.msgType      = clientPID ;  /* "Reply" to a specific client */
        msg.body.result  = result ;
        msg.body.sender  = getpid() ;   /* Tell the client who te server is */
        
        /* Send the result message to the User process */
        msgflags = 0 ;   // Block me if queue has no room to send

		msgStatus = msgsnd( queID , &msg , MSG_INFO_SIZE , 0 ) ;  /* the msg flag is set to 0 */
        if ( msgStatus < 0 ) 
        {
            printf("Calculator Failed to send message to User process on queuID %d. Error code=%d\n" 
                   , queID , errno ) ;
            perror("Reason");
            exit(-2) ;  
        }
        else 
        {
            printf("Calculator sent the following message to the User:\n" );
            printMsg( & msg );
            printf("\n" );
        }
        printf("\n" );
    }
   
    /* Mission Accomplished, so destroy the message queue */
    /* could also use the command 'ipcrm -q' to do the same task */
    msgctl( queID , IPC_RMID , NULL ) ;    

    return 0 ;
}

