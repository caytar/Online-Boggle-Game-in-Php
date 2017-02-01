// Definition of the ServerConnection class

#ifndef ServerConnection_class
#define ServerConnection_class

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>
#include "NetworkConstants.h"
#include "xmlParser.h"
#include "dictionary.h"

struct PlayerRanking
{
	char *userId;
	int points;
	int gameCount;
};

struct Player
{
	char *userId;
	char **words;
	int wordCount;
	int points;
	int ranking;
};

struct Room
{
	//wordList *list;
	char *room;
	int rowCount;
	int columnCount;
	bool isUniqueCounts;
	int time;
	int wordPoints[10];
	Player *players;
	Player *lastPlayers;
	int playerCount;
	int lastPlayerCount;
	time_t starttime;
	time_t breaktime;
	bool gameStarted;
	bool resultsCalculated;
	char *board;	
};

class ServerConnection
{
	public:
		ServerConnection();
		void StartConnection(unsigned short port);		
		bool AcceptConnection();
		void HandleClient();
		void CheckRoomsStatus();

	private:
		void CreateConnection();
		void GiveErrorAndDie(char *errorMessage);
		void ImportRooms();
		char* CreateRandomBoard(int rows, int columns);
		void CalculateResults(int roomIndex);
		void CalculateAndSavePlayerRatings(int roomIndex);
		char* ChangeToUserId(int n);
		int ChangetoInt(char *temp);

		int serverSock;                    /* Socket descriptor for server */
		struct sockaddr_in serverAddr;     /* Local address */
		unsigned short serverPort;         /* Server port */

		int clientSock;                    /* Socket descriptor for client */
		struct sockaddr_in clientAddr;     /* Client address */

		Room *rooms;
		int roomCount;
		char messageCode;
		char userId[4];
		char *word;
		
		
};

#endif
