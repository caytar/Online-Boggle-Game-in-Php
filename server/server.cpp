#include "ServerConnection.h"
#include <pthread.h>
#include <time.h>

// Create the server connection
ServerConnection server;

void *timeFunction( void *ptr )
{
	char *message;
	message = (char *) ptr;

	//time_t seconds;

	while(true)
	{
		server.CheckRoomsStatus();
	}
}

void *receiveAndSendInformation( void *ptr)
{
	char *message;
	unsigned short serverPort;     /* Server port */
	message = (char *) ptr;
	serverPort = atoi(message);  /* First arg:  local port */

	// Create the server connection
	server.StartConnection(serverPort);

	while(true)
	{
		server.AcceptConnection();

		server.HandleClient();
	}
}

int main(int argc, char *argv[])
{
	
	pthread_t thread1, thread2;
	char *message1 = "kontrol";
	char *message2 = "Thread 2";
	int  iret1, iret2;
	
	if (argc != 2)     /* Test for correct number of arguments */
	{
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}

	message2 = argv[1];
	
	
	/* Create independent threads each of which will execute function */
	
	iret1 = pthread_create( &thread1, NULL, timeFunction, (void*) message1);
	iret2 = pthread_create( &thread2, NULL, receiveAndSendInformation, (void*) message2);
	
	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	
	pthread_join( thread2, NULL);
	pthread_join( thread1, NULL);
	
	//	printf("Thread 1 returns: %d\n",iret1);
	//	printf("Thread 2 returns: %d\n",iret2);
	exit(0);
}
