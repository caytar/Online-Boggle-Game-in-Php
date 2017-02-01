// Implementation of the ServerConnection class

#include "ServerConnection.h"


searchDictionary words;

ServerConnection::ServerConnection()
{
	
}

void ServerConnection::StartConnection(unsigned short port)
{
	serverPort = port;
	roomCount = 0;
	ImportRooms();
	CreateConnection();
	printf("Server started succesfully and listening on port %d\n", port);
}

void ServerConnection::CreateConnection()
{	
	/* Create socket for incoming connections */
	if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		GiveErrorAndDie("socket() hata verdi");
	
	/* Construct local address structure */
	memset(&serverAddr, 0, sizeof(serverAddr));       /* Zero out structure */
	serverAddr.sin_family = AF_INET;                  /* Internet address family */
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any incoming interface */
	serverAddr.sin_port = htons(serverPort);          /* Local port */
	
	/* Bind to the local address */
	if (bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
		GiveErrorAndDie("bind() hata verdi");
	
	/* Mark the socket so it will listen for incoming connections */
	if (listen(serverSock, MAXPENDING) < 0)
		GiveErrorAndDie("listen() hata verdi");
	
	
}

void ServerConnection::ImportRooms()
{
	XMLNode xMainNode=XMLNode::openFileHelper("server.xml","SERVER");

	XMLNode xNode = xMainNode.getChildNode("Rooms");

	int n = xNode.nChildNode("Room");

	rooms = (Room *)malloc(sizeof(Room)*n);

	int i,myIterator=0;
	const char *temp;
	for (i=0; i<n; i++)
	{
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("name").getAttribute("value");
		int j=0;
		while(temp[j]) j++;
		rooms[i].room = new char[j+1];
		for(int k=0;k<j;k++) rooms[i].room[k] = temp[k];
		rooms[i].room[j] = '\0'; 	
		myIterator--;	
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("rowCount").getAttribute("value");
		rooms[i].rowCount = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("columnCount").getAttribute("value");
		rooms[i].columnCount = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("isUniqueCounts").getAttribute("value");
		int temp2 = atoi(temp);
		rooms[i].isUniqueCounts = false;
		if(temp2 == 1)  rooms[i].isUniqueCounts = true;
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("time").getAttribute("value");
		rooms[i].time = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v1");
		rooms[i].wordPoints[0] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v2");
		rooms[i].wordPoints[1] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v3");
		rooms[i].wordPoints[2] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v4");
		rooms[i].wordPoints[3] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v5");
		rooms[i].wordPoints[4] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v6");
		rooms[i].wordPoints[5] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v7");
		rooms[i].wordPoints[6] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v8");
		rooms[i].wordPoints[7] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v9");
		rooms[i].wordPoints[8] = atoi(temp);
		myIterator--;
		temp = xNode.getChildNode("Room", &myIterator).getChildNode("wordPoints").getAttribute("v10");
		rooms[i].wordPoints[9] = atoi(temp);
		
		rooms[i].playerCount = 0;
		rooms[i].lastPlayerCount = 0;
		rooms[i].lastPlayers = NULL;
		rooms[i].players = NULL;
		rooms[i].gameStarted = true;
		rooms[i].resultsCalculated = false;
		rooms[i].board = CreateRandomBoard(rooms[i].rowCount, rooms[i].columnCount);
		//words.findAllWords(rooms[i].list, rooms[i].rowCount, rooms[i].columnCount, rooms[i].board);
		rooms[i].starttime = time(NULL);
		rooms[i].breaktime = rooms[i].starttime + rooms[i].time;
		printf("%s Odasinda oyun basladi\n", rooms[i].room);
		roomCount++;
	}
}

char* ServerConnection::CreateRandomBoard(int rows, int columns)
{
	char* temp = (char*)malloc(sizeof(char)*rows*columns + 1);
	
	for(int i=0; i<rows;i++)
	{
		for(int j=0;j<columns;j++)
		{
			temp[i*columns+j] = rand() % 26 + 'a';
		}
	}	
	
	temp[rows*columns] = '\0';
	
	return temp;
}

bool ServerConnection::AcceptConnection()
{
	unsigned int clientLength = sizeof(clientAddr);
		
	/* Wait for a client to connect */
	if ((clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientLength)) < 0)
		GiveErrorAndDie("accept() hata verdi");
	
	return true;
}

void ServerConnection::HandleClient()
{	
	char messageBuffer[MESSAGEBUFFER];
	int recvMsgSize;

	/* Receive message from client */
	if ((recvMsgSize = recv(clientSock, messageBuffer, MESSAGEBUFFER, 0)) < 0)
		GiveErrorAndDie("recv() hata verdi");

	printf("clienttan gelen: %s\n", messageBuffer);
	
	messageCode = messageBuffer[0];
	
	userId[0] = messageBuffer[1];
	userId[1] = messageBuffer[2];
	userId[2] = messageBuffer[3];
	userId[3] = '\0';
	
	char wordL[2], roomL[2];
	int wordLength, roomLength;
	char *word, *room, *word2;
	XMLNode xMainNode;
	XMLNode xNode;
	
	char *sendRoomList;
	int totalMessageSize=0, araHesap, araHesap2;
	char number[2];
	int k=0, t=0, wordPoint=0, totalPoints=0;
	int dogru = 0;
	
	int roomNum = 0, logSayisi = 0;
	int myIterator=0, gi=0, myIterator4=0;
	const char *temp;
	int ii=0;
	
	int totalPlayerCount = 0;
	bool kayittaVar = false;
	
	char tempRanksL[10];
	
	switch(messageCode)
	{
		case ROOMCREATE:
			roomL[0] = messageBuffer[4];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(ii=0; ii<roomLength; ii++)
				room[ii] = messageBuffer[5+ii];
			room[roomLength] = '\0'; 
			
			gi = 5+ii;
			
			for(ii=0;ii<roomCount;ii++)
			{
				if(strcmp(rooms[ii].room, room) == 0)
				{
					kayittaVar = true;
					break;
				}
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*3);
			
			sendRoomList[0] = 'A';
			
			if(!(kayittaVar))			// kaydet
			{
				int totalXmlLength = 191+roomLength;
				
				char rowC = messageBuffer[gi];
				char columnC = messageBuffer[gi+1];
				
				gi = gi+2;
				
				char tempT[2];
				tempT[0] = messageBuffer[gi];
				tempT[1] = '\0';
				int time_length = atoi(tempT);
				
				totalXmlLength += time_length;
				
				char tempTime[(time_length+1)];
				
				for(ii=0;ii<time_length;ii++)
					tempTime[ii] = messageBuffer[gi+1+ii];
				tempTime[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 1 point
				tempT[1] = '\0';
				int wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v1[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v1[ii] = messageBuffer[gi+1+ii];
				v1[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 2 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v2[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v2[ii] = messageBuffer[gi+1+ii];
				v2[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 3 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v3[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v3[ii] = messageBuffer[gi+1+ii];
				v3[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 4 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v4[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v4[ii] = messageBuffer[gi+1+ii];
				v4[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 5 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v5[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v5[ii] = messageBuffer[gi+1+ii];
				v5[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 6 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v6[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v6[ii] = messageBuffer[gi+1+ii];
				v6[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 7 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v7[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v7[ii] = messageBuffer[gi+1+ii];
				v7[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 8 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v8[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v8[ii] = messageBuffer[gi+1+ii];
				v8[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 9 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v9[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v9[ii] = messageBuffer[gi+1+ii];
				v9[ii] = '\0';
				gi = gi+1+ii;
				
				tempT[0] = messageBuffer[gi];		// word 10 point
				tempT[1] = '\0';
				wpoint_length = atoi(tempT);
				
				totalXmlLength += wpoint_length;
				
				char v10[(wpoint_length+1)];
				
				for(ii=0;ii<wpoint_length;ii++)
					v10[ii] = messageBuffer[gi+1+ii];
				v10[ii] = '\0';
				
				char *xmlString = (char *)malloc(sizeof(char)*(totalXmlLength+1));
				
				// new room icin xml olustur
				
				char *temp = "<Room><name value=";
			
				int j=0;
				for(j=0; j<strlen(temp); j++)
					xmlString[j] = temp[j];
				xmlString[j] = '\"';
				
				gi = j+1;
				
				for(j=0; j<roomLength; j++)
					xmlString[gi+j] = room[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = "/><rowCount value=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				xmlString[gi] = rowC;				
				xmlString[gi+1] = '\"';
				
				gi = gi+2;
				
				temp = "/><columnCount value=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				xmlString[gi] = columnC;			
				xmlString[gi+1] = '\"';
				
				gi = gi+2;
				
				temp = "/><isUniqueCounts value=\"0\"/><time value=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0;j<time_length;j++)
					xmlString[gi+j] = tempTime[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = "/><wordPoints v1=";			// word length 1
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v1); j++)
					xmlString[gi+j] = v1[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v2=";			// word length 2
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v2); j++)
					xmlString[gi+j] = v2[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v3=";			// word length 3
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v3); j++)
					xmlString[gi+j] = v3[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v4=";			// word length 4
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v4); j++)
					xmlString[gi+j] = v4[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v5=";			// word length 5
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v5); j++)
					xmlString[gi+j] = v5[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v6=";			// word length 6
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v6); j++)
					xmlString[gi+j] = v6[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v7=";			// word length 7
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v7); j++)
					xmlString[gi+j] = v7[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v8=";			// word length 8
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v8); j++)
					xmlString[gi+j] = v8[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v9=";			// word length 9
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v9); j++)
					xmlString[gi+j] = v9[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = " v10=";			// word length 10
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<strlen(v10); j++)
					xmlString[gi+j] = v10[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = "/></Room>";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				xmlString[gi+j+1] = '\0';
				
				printf("str: %s\n", xmlString);
				
				XMLNode xMainNode=XMLNode::openFileHelper("server.xml","SERVER");
				
				XMLNode xNode = XMLNode::parseString(xmlString);
				xMainNode.getChildNode("Rooms").addChild(xNode,0);
			
				char *t = xMainNode.createXMLString(true);
				
				FILE * pFile;
				pFile = fopen ("server.xml","w");
				
				fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n",pFile);
				fputs (t,pFile);
				fclose (pFile);
				
				free(t);
				
				// yeni room u ekle
				
				rooms = (Room *)realloc(rooms, sizeof(Room)*(roomCount+1));
				rooms[roomCount].room = room;
				
				tempT[0] = rowC;
				tempT[1] = '\0';
				rooms[roomCount].rowCount = atoi(tempT);
				
				tempT[0] = columnC;
				tempT[1] = '\0';
				rooms[roomCount].columnCount = atoi(tempT);
				
				rooms[roomCount].isUniqueCounts = false;
				
				rooms[roomCount].time = atoi(tempTime);
				
				rooms[roomCount].wordPoints[0] = atoi(v1);
				rooms[roomCount].wordPoints[1] = atoi(v2);
				rooms[roomCount].wordPoints[2] = atoi(v3);
				rooms[roomCount].wordPoints[3] = atoi(v4);
				rooms[roomCount].wordPoints[4] = atoi(v5);
				rooms[roomCount].wordPoints[5] = atoi(v6);
				rooms[roomCount].wordPoints[6] = atoi(v7);
				rooms[roomCount].wordPoints[7] = atoi(v8);
				rooms[roomCount].wordPoints[8] = atoi(v9);
				rooms[roomCount].wordPoints[9] = atoi(v10);
				
				rooms[roomCount].playerCount = 0;
				rooms[roomCount].lastPlayerCount = 0;
				rooms[roomCount].lastPlayers = NULL;
				rooms[roomCount].players = NULL;
				rooms[roomCount].gameStarted = true;
				rooms[roomCount].resultsCalculated = false;
				rooms[roomCount].board = CreateRandomBoard(rooms[roomCount].rowCount, rooms[roomCount].columnCount);
				//words.findAllWords(rooms[i].list, rooms[i].rowCount, rooms[i].columnCount, rooms[i].board);
				rooms[roomCount].starttime = time(NULL);
				rooms[roomCount].breaktime = rooms[roomCount].starttime + rooms[roomCount].time;
				printf("%s Odasinda oyun basladi\n", rooms[roomCount].room);
				
				roomCount++;
				
				sendRoomList[1] = '1';
			}
			else					// roomname kayitta var, kaydetme
			{
				sendRoomList[1] = '0';
			}
			
			sendRoomList[2] = '\0';
			
			printf("new room sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, 3, 0) != 3)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case WORDCONTROL:
			wordL[0] = messageBuffer[4];
			wordL[1] = '\0';
			wordLength = atoi(wordL)+1;
			word = new char[wordLength+1];
			for(int i=0; i<wordLength; i++)
				word[i] = messageBuffer[5+i];
			word[wordLength] = '\0';
			
			roomL[0] = messageBuffer[5+wordLength];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[6+wordLength+i];
			room[roomLength] = '\0'; 
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					int j=0;
					for(j=0;j<rooms[i].playerCount;j++)
					{
						if(strcmp(userId, rooms[i].players[j].userId) == 0) // oyuncu bu room da oynuyor
						{
							int m=0;
							for(m=0; m<rooms[i].players[j].wordCount; m++)
							{
								if(strcmp(rooms[i].players[j].words[m], word) == 0)
								{
									dogru = 2;
									totalPoints = rooms[i].players[j].points;
									break;
								}
							}
							
							if(m == rooms[i].players[j].wordCount)
							{
								rooms[i].players[j].words = (char **)realloc(rooms[i].players[j].words, sizeof(char *)*(rooms[i].players[j].wordCount+1));
							
								rooms[i].players[j].words[rooms[i].players[j].wordCount] = (char *)malloc(sizeof(char)*(wordLength+1));
								for(int k=0;k<wordLength+1;k++) rooms[i].players[j].words[rooms[i].players[j].wordCount][k] = word[k];
							
								rooms[i].players[j].wordCount++;
							
								if(words.linearSearch(word) == 1)
								{
									rooms[i].players[j].points += rooms[i].wordPoints[(wordLength-1)];
									dogru = 1;
									wordPoint = rooms[i].wordPoints[(wordLength-1)];
								}
								
								totalPoints = rooms[i].players[j].points;
							}
							
							
							
							break;
						}
					}
					
					if(j == rooms[i].playerCount) // oyuncuyu room a ekle
					{
						rooms[i].playerCount++;
						if(j == 0) // players a yer al
						{
							rooms[i].players = (Player *)malloc(sizeof(Player));
							rooms[i].players[0].wordCount = 0;
						}
						else // players in yerini buyut
						{
							rooms[i].players = (Player *)realloc(rooms[i].players, sizeof(Player)*rooms[i].playerCount);
						}
						
						rooms[i].players[j].userId = (char *)malloc(sizeof(char)*4);
						rooms[i].players[j].userId[0] = userId[0]; rooms[i].players[j].userId[1] = userId[1];
						rooms[i].players[j].userId[2] = userId[2]; rooms[i].players[j].userId[3] = userId[3];
						
						rooms[i].players[j].wordCount = 0;
						
						//if(rooms[i].players[j].wordCount == 0) // ilk kelime
						//{
							rooms[i].players[j].words = (char **)malloc(sizeof(char*));
							rooms[i].players[j].points = 0;
						//}
						//else
						//	rooms[i].players[j].words = (char **)realloc(rooms[i].players[j].words, sizeof(char *)*(rooms[i].players[j].wordCount+1));
						
						rooms[i].players[j].words[rooms[i].players[j].wordCount] = (char *)malloc(sizeof(char)*(wordLength+1));
						for(int k=0;k<wordLength+1;k++) rooms[i].players[j].words[rooms[i].players[j].wordCount][k] = word[k];
						
						rooms[i].players[j].wordCount++;
						
						if(words.linearSearch(word) == 1)
						{
							rooms[i].players[j].points += rooms[i].wordPoints[(wordLength-1)];
							dogru = 1;
							wordPoint = rooms[i].wordPoints[(wordLength-1)];							
						}
						
						totalPoints = rooms[i].players[j].points;
					}
					
					break;
				}
			}
			
			printf("New Point: %d\n", totalPoints);
			
			totalMessageSize += 11;
			totalMessageSize += wordLength;
			
			sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
			
			sendRoomList[0] = 'B';
			sendRoomList[1] = userId[0];
			sendRoomList[2] = userId[1];
			sendRoomList[3] = userId[2];
			
			sprintf(number, "%d", (wordLength-1));
			sendRoomList[4] = number[0];
			
			for(int j=0; j<wordLength;j++)
			{
				sendRoomList[5+j] = word[j];
			}
			
			if(dogru == 0) sendRoomList[5+wordLength] = '0';
			else if(dogru == 1) sendRoomList[5+wordLength] = '1';
			else sendRoomList[5+wordLength] = '2';
						
			araHesap = wordPoint % 10;
			araHesap = (wordPoint - araHesap)/10;
			sprintf(number, "%d", araHesap);
			sendRoomList[6+wordLength] = number[0];
			
			araHesap = wordPoint % 10;
			sprintf(number, "%d", araHesap);
			sendRoomList[7+wordLength] = number[0];
			
			araHesap = totalPoints % 100;
			araHesap = (totalPoints - araHesap)/100;
			sprintf(number, "%d", araHesap);
			sendRoomList[8+wordLength] = number[0];
				
			araHesap2 = totalPoints - (araHesap*100);	
			araHesap = araHesap2 % 10;
			araHesap = (araHesap2 - araHesap)/10;
			sprintf(number, "%d", araHesap);
			sendRoomList[9+wordLength] = number[0];
				
			araHesap = araHesap2 % 10;
				
			sprintf(number, "%d", araHesap);
			sendRoomList[10+wordLength] = number[0];
			
			sendRoomList[totalMessageSize] = '\0';
			
			printf("word control sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, (totalMessageSize+1), 0) != (totalMessageSize+1))
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case ROOMLIST:
			totalMessageSize += 3;
			totalMessageSize += 10*roomCount;
			for(int i=0;i<roomCount;i++)
			{
				int j=0;
				while(rooms[i].room[j]) j++;
				totalMessageSize += j+1;
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
			
			sendRoomList[0] = 'D';
			
			araHesap = roomCount % 10;
			araHesap = (roomCount - araHesap)/10;
			sprintf(number, "%d", araHesap);
			
			sendRoomList[1] = number[0];
			
			araHesap = roomCount % 10;
			sprintf(number, "%d", araHesap);
			
			sendRoomList[2] = number[0];
			
			for(int i=0;i<roomCount;i++)
			{
				int j=0;
				while(rooms[i].room[j]) j++;
				
				sprintf(number, "%d", (j-1));
				
				k++;
				sendRoomList[2+k] = number[0];
				
				for(int l=0; l<j; l++)
				{
					k++;
					sendRoomList[2+k] = rooms[i].room[l];
				}
				
				sprintf(number, "%d", rooms[i].rowCount);
				k++;
				sendRoomList[2+k] = number[0];
				
				sprintf(number, "%d", rooms[i].columnCount);
				k++;
				sendRoomList[2+k] = number[0];
				
				k++;
				if(rooms[i].isUniqueCounts) sendRoomList[2+k] = '1';
				else sendRoomList[2+k] = '0';
				
				araHesap = rooms[i].time % 100;
				araHesap = (rooms[i].time - araHesap)/100;
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
				
				araHesap2 = rooms[i].time - (araHesap*100);	
				araHesap = araHesap2 % 10;
				araHesap = (araHesap2 - araHesap)/10;
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
				
				araHesap = araHesap2 % 10;
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
				
				k++;
				if(rooms[i].gameStarted) sendRoomList[2+k] = '1';
				else sendRoomList[2+k] = '0';
				
				time_t seconds = time(NULL);
				int remainingTime;
				if(rooms[i].gameStarted) remainingTime = seconds - rooms[i].starttime;
				else remainingTime = seconds - rooms[i].breaktime;
				
				araHesap = remainingTime % 100;
				araHesap = (remainingTime - araHesap)/100;
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
				
				araHesap2 = remainingTime - (araHesap*100);	
				araHesap = araHesap2 % 10;
				araHesap = (araHesap2 - araHesap)/10;
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
				
				araHesap = araHesap2 % 10;
				
				sprintf(number, "%d", araHesap);
				k++;
				sendRoomList[2+k] = number[0];
			}
			
			sendRoomList[totalMessageSize] = '\0';
			
			printf("room list sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, 1024, 0) != 1024)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case STARTGAME:
			roomL[0] = messageBuffer[4];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[5+i];
			room[roomLength] = '\0'; 
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					totalMessageSize += 8;
					totalMessageSize += rooms[i].rowCount * rooms[i].columnCount;
					
					sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
					
					sendRoomList[0] = 'C';
					sendRoomList[1] = userId[0];
					sendRoomList[2] = userId[1];
					sendRoomList[3] = userId[2];
					
					if(rooms[i].gameStarted) sendRoomList[4] = '1';
					else sendRoomList[4] = '0';
					
					time_t seconds = time(NULL);
					int remainingTime;
					if(rooms[i].gameStarted) remainingTime = seconds - rooms[i].starttime;
					else remainingTime = seconds - rooms[i].breaktime;
				
					araHesap = remainingTime % 100;
					araHesap = (remainingTime - araHesap)/100;
					sprintf(number, "%d", araHesap);
					sendRoomList[5] = number[0];
				
					araHesap2 = remainingTime - (araHesap*100);	
					araHesap = araHesap2 % 10;
					araHesap = (araHesap2 - araHesap)/10;
					sprintf(number, "%d", araHesap);
					sendRoomList[6] = number[0];
				
					araHesap = araHesap2 % 10;
				
					sprintf(number, "%d", araHesap);
					sendRoomList[7] = number[0];
					
					for(int j=0; j<rooms[i].rowCount*rooms[i].columnCount;j++)
					{
						sendRoomList[8+j] = rooms[i].board[j];
					}
					
				}
			}
			
			sendRoomList[totalMessageSize] = '\0';
			
			printf("start game sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, (totalMessageSize+1), 0) != (totalMessageSize+1))
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case ENDGAME:
			roomL[0] = messageBuffer[4];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[5+i];
			room[roomLength] = '\0'; 
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					totalMessageSize += 6;
					totalMessageSize += roomLength;
					
					sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
					
					sendRoomList[0] = 'E';
					sendRoomList[1] = userId[0];
					sendRoomList[2] = userId[1];
					sendRoomList[3] = userId[2];
					
					sprintf(number, "%d", (roomLength-1));
					sendRoomList[4] = number[0];
					
					for(int j=0;j<roomLength;j++) sendRoomList[5+j] = rooms[i].room[j];
					
					if(!(rooms[i].gameStarted)) sendRoomList[5+roomLength] = '0';
					else sendRoomList[5+roomLength] = '1';
				}
			}
			
			sendRoomList[totalMessageSize] = '\0';
			
			printf("end game sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, (totalMessageSize+1), 0) != (totalMessageSize+1))
				GiveErrorAndDie("send() hata verdi");
			
			break;	
		case GAMERESULTS:
			roomL[0] = messageBuffer[4];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[5+i];
			room[roomLength] = '\0'; 
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					totalMessageSize += 6;
					totalMessageSize += roomLength;
					
					if(rooms[i].resultsCalculated)
					{
						totalMessageSize += 1;
						
						if(rooms[i].lastPlayerCount < 10) totalMessageSize += 1;
						else if(rooms[i].lastPlayerCount < 100) totalMessageSize += 2;
						else if(rooms[i].lastPlayerCount < 1000) totalMessageSize += 3;
						else if(rooms[i].lastPlayerCount < 10000) totalMessageSize += 4;
						else if(rooms[i].lastPlayerCount < 100000) totalMessageSize += 5;
						else if(rooms[i].lastPlayerCount < 1000000) totalMessageSize += 6;
					
						totalMessageSize += rooms[i].lastPlayerCount * 9;
					}
					
					sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
					
					sendRoomList[0] = 'F';
					sendRoomList[1] = userId[0];
					sendRoomList[2] = userId[1];
					sendRoomList[3] = userId[2];
					
					sprintf(number, "%d", (roomLength-1));
					sendRoomList[4] = number[0];
					
					for(int j=0;j<roomLength;j++) sendRoomList[5+j] = rooms[i].room[j];
					
					if(!(rooms[i].resultsCalculated)) sendRoomList[5+roomLength] = '0';
					else
					{
						sendRoomList[5+roomLength] = '1';
						
						if(rooms[i].lastPlayerCount < 10) sendRoomList[6+roomLength] = '0';
						else if(rooms[i].lastPlayerCount < 100) sendRoomList[6+roomLength] = '1';
						else if(rooms[i].lastPlayerCount < 1000) sendRoomList[6+roomLength] = '2';
						else if(rooms[i].lastPlayerCount < 10000) sendRoomList[6+roomLength] = '3';
						else if(rooms[i].lastPlayerCount < 100000) sendRoomList[6+roomLength] = '4';
						else if(rooms[i].lastPlayerCount < 1000000) sendRoomList[6+roomLength] = '5';
						
						k = 1;
						if(rooms[i].lastPlayerCount > 99)
						{
							araHesap = rooms[i].lastPlayerCount % 100;
							araHesap = (rooms[i].lastPlayerCount - araHesap)/100;
							sprintf(number, "%d", araHesap);
							sendRoomList[6+roomLength+k] = number[0];
							k++;
						}
						if(rooms[i].lastPlayerCount > 9)
						{
							araHesap = rooms[i].lastPlayerCount % 100;
							araHesap2 = araHesap % 10;
							araHesap = (araHesap - araHesap2)/10;
							sprintf(number, "%d", araHesap);
							sendRoomList[6+roomLength+k] = number[0];
							k++;
						}
						
						araHesap = rooms[i].lastPlayerCount % 10;
						sprintf(number, "%d", araHesap);
						sendRoomList[6+roomLength+k] = number[0];
						
						t=7+roomLength+k;
						for(int j=0;j<rooms[i].lastPlayerCount;j++)
						{
							sendRoomList[t+j*9] = rooms[i].lastPlayers[j].userId[0];
							sendRoomList[t+j*9+1] = rooms[i].lastPlayers[j].userId[1];
							sendRoomList[t+j*9+2] = rooms[i].lastPlayers[j].userId[2];
							
							if(rooms[i].lastPlayers[j].points > 99)
							{
								araHesap = rooms[i].lastPlayers[j].points % 100;
								araHesap = (rooms[i].lastPlayers[j].points - araHesap)/100;
								sprintf(number, "%d", araHesap);
								sendRoomList[t+j*6+3] = number[0];
							}
							else
								sendRoomList[t+j*9+3] = '0';
							
							if(rooms[i].lastPlayers[j].points > 9)
							{
								araHesap = rooms[i].lastPlayers[j].points % 100;
								araHesap2 = araHesap % 10;
								araHesap = (araHesap - araHesap2)/10;
								sprintf(number, "%d", araHesap);
								sendRoomList[t+j*6+4] = number[0];
							}
							else
								sendRoomList[t+j*9+4] = '0';
							
							araHesap = rooms[i].lastPlayers[j].points % 10;
							sprintf(number, "%d", araHesap);
							sendRoomList[t+j*9+5] = number[0];
							
							// oyuncu rankingini al ve gonder
							
							xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
	
							totalPlayerCount = xMainNode.nChildNode("player");
							
							myIterator = 0;
							for(int ij=0;ij<totalPlayerCount;ij++)
							{
								XMLNode player = xMainNode.getChildNode("player",&myIterator);
								if(strcmp(player.getChildNode("id").getAttribute("v"), rooms[i].lastPlayers[j].userId) == 0)
								{
									char *tempRank = (char *)player.getChildNode("rating").getAttribute("v");
									
									sendRoomList[t+j*9+6] = tempRank[0];
									sendRoomList[t+j*9+7] = tempRank[1];
									sendRoomList[t+j*9+8] = tempRank[2];
									
									break;
								}
							}
							
						}
					}
				}
			}
			
			sendRoomList[totalMessageSize] = '\0';
			
			printf("game results sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, (totalMessageSize+1), 0) != (totalMessageSize+1))
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case LOGREQUEST:
			xMainNode=XMLNode::openFileHelper("log.xml","BOGGLELOG");
	
			roomNum = xMainNode.nChildNode("room");
			
			totalMessageSize = 0;
			
			for (int i=0; i<roomNum; i++)
			{
				xNode = xMainNode.getChildNode("room", &myIterator);
				
				int plaNum = xNode.getChildNode("players").nChildNode("player");
				
				int j,myIterator1=0;
				//XMLNode xNode1;
				for(j=0;j<plaNum;j++)
				{
					temp = xNode.getChildNode("players").getChildNode("player", &myIterator1).getChildNode("id").getAttribute("v");
					
					if(strcmp(temp, userId) == 0)
						break;
				}
				
				if(j == plaNum)
					continue;
				
				logSayisi++;
				
				int roomIndex;
				temp = xNode.getAttribute("v");
				
				for(j=0;j<roomCount;j++)
					if(strcmp(temp, rooms[j].room) == 0) 
						roomIndex = j;
				
				totalMessageSize += 1;			// room name length basamak sayisi
				totalMessageSize += strlen(temp);	// room name length
				totalMessageSize += 2;			// time length basamak sayisi = 2
				
				temp = xNode.getChildNode("time").getAttribute("v");
				
				totalMessageSize += strlen(temp);	// time length
				totalMessageSize += 2;			// row and column basamak sayilari 1, 1
				totalMessageSize += rooms[roomIndex].rowCount * rooms[roomIndex].columnCount;	// board length
				
				int myIterator2 = 0;
				XMLNode xNode1;
				int totalWordsSize = 0;
				for(j=0;j<plaNum;j++)
				{
					xNode1 = xNode.getChildNode("players").getChildNode("player", &myIterator2);
					temp = xNode1.getChildNode("id").getAttribute("v");
					
					if(strcmp(temp, userId) == 0)
					{
						totalMessageSize += 1;		// puan length basamak sayisi
						
						temp = xNode1.getChildNode("points").getAttribute("v");
						
						totalMessageSize += strlen(temp);	// puan length
						totalMessageSize += 1;			// words length basamak sayisi
						
						int wordCount = xNode1.getChildNode("words").nChildNode("word");
						
						int myIterator3 = 0;
						for(int r=0;r<wordCount;r++)
						{
							temp = xNode1.getChildNode("words").getChildNode("word", &myIterator3).getAttribute("v");
							
							if(words.linearSearch((char *)temp) == 1)
							{
								totalMessageSize += 21;
								totalMessageSize += strlen(temp);
								char tempPoints[10];
								sprintf(tempPoints, "%d", rooms[roomIndex].wordPoints[(strlen(temp)-1)]);
								totalMessageSize += strlen(tempPoints);
								totalWordsSize += 21 + strlen(temp) + strlen(tempPoints);
							}
							else
							{
								totalMessageSize += 31;
								totalMessageSize += strlen(temp);
								totalWordsSize += 31 + strlen(temp);
							}
						}
						
						break;
					}
				}
				
				char tempPoints[10];
				sprintf(tempPoints, "%d", totalWordsSize);
				totalMessageSize += strlen(tempPoints);
			}
			
			totalMessageSize += 5;
			char tempPoints[10];
			sprintf(tempPoints, "%d", logSayisi);
			totalMessageSize += strlen(tempPoints);
			
			sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
			
			sendRoomList[0] = 'G';
			sendRoomList[1] = userId[0];
			sendRoomList[2] = userId[1];
			sendRoomList[3] = userId[2];
			sendRoomList[4] = strlen(tempPoints)+'0';
			
			for(ii=0;ii<strlen(tempPoints);ii++)
				sendRoomList[5+ii] = tempPoints[ii];
			gi = 5+ii;
			
			for (int i=0; i<roomNum; i++)
			{
				xNode = xMainNode.getChildNode("room", &myIterator4);
				
				int plaNum = xNode.getChildNode("players").nChildNode("player");
				
				int j,myIterator1=0;
				//XMLNode xNode1;
				for(j=0;j<plaNum;j++)
				{
					temp = xNode.getChildNode("players").getChildNode("player", &myIterator1).getChildNode("id").getAttribute("v");
					
					if(strcmp(temp, userId) == 0)
						break;
				}
				
				if(j == plaNum)
					continue;
				
				int roomIndex;
				temp = xNode.getAttribute("v");
				
				for(j=0;j<roomCount;j++)
					if(strcmp(temp, rooms[j].room) == 0) 
						roomIndex = j;
				
				sendRoomList[gi] = (strlen(temp)-1)+'0';		// room name length basamak sayisi
				
				for(j=0;j<strlen(temp);j++) 
					sendRoomList[gi+1+j] = temp[j];		// room name 
				
				gi = gi+1+j;
				
				temp = xNode.getChildNode("time").getAttribute("v");
				
				char temp2[10];
				sprintf(temp2, "%d", strlen(temp));
				
				sendRoomList[gi] = temp2[0];			// time length basamak sayisi = 2
				sendRoomList[gi+1] = temp2[1];
				
				for(j=0;j<strlen(temp);j++) 
					sendRoomList[gi+2+j] = temp[j];		// time 
				
				sendRoomList[gi+2+j] = rooms[roomIndex].rowCount+'0';	// row count
				sendRoomList[gi+3+j] = rooms[roomIndex].columnCount+'0';	// column count
				
				gi = gi+4+j;
				
				temp = xNode.getChildNode("board").getAttribute("v");
				
				for(j=0;j<strlen(temp);j++) 
					sendRoomList[gi+j] = temp[j];		// board 
				gi = gi+j;
				
				int myIterator2 = 0;
				XMLNode xNode1;
				int totalWordsSize = 0;
				for(j=0;j<plaNum;j++)
				{
					xNode1 = xNode.getChildNode("players").getChildNode("player", &myIterator2);
					temp = xNode1.getChildNode("id").getAttribute("v");
					
					if(strcmp(temp, userId) == 0)
					{
						totalMessageSize += 1;		// puan length basamak sayisi
						
						temp = xNode1.getChildNode("points").getAttribute("v");
						
						totalMessageSize += strlen(temp);	// puan length
						totalMessageSize += 1;			// words length basamak sayisi
						
						int wordCount = xNode1.getChildNode("words").nChildNode("word");
						
						int myIterator3 = 0;
						for(int r=0;r<wordCount;r++)
						{
							temp = xNode1.getChildNode("words").getChildNode("word", &myIterator3).getAttribute("v");
							
							if(words.linearSearch((char *)temp) == 1)
							{
								totalMessageSize += 21;
								totalMessageSize += strlen(temp);
								char tempPoints[10];
								sprintf(tempPoints, "%d", rooms[roomIndex].wordPoints[(strlen(temp)-1)]);
								totalMessageSize += strlen(tempPoints);
								totalWordsSize += 21 + strlen(temp) + strlen(tempPoints);
							}
							else
							{
								totalMessageSize += 31;
								totalMessageSize += strlen(temp);
								totalWordsSize += 31 + strlen(temp);
							}
						}
						
						break;
					}
				}
				
				char tempPoints[10];
				sprintf(tempPoints, "%d", totalWordsSize);
				
				int myIterator5 = 0;
				for(j=0;j<plaNum;j++)
				{
					xNode1 = xNode.getChildNode("players").getChildNode("player", &myIterator5);
					temp = xNode1.getChildNode("id").getAttribute("v");
					
					if(strcmp(temp, userId) == 0)
					{
						temp = xNode1.getChildNode("points").getAttribute("v");
						
						sendRoomList[gi] = strlen(temp)+'0';		// puan length
						
						int jj=0;
						for(jj=0;jj<strlen(temp);jj++)
							sendRoomList[gi+1+jj] = temp[jj];	// puan
						gi = gi+1+jj;
						
						sendRoomList[gi] = strlen(tempPoints)+'0';		// words length length
						
						for(jj=0;jj<strlen(tempPoints);jj++)
							sendRoomList[gi+1+jj] = tempPoints[jj];	// words length
						gi = gi+1+jj;
						
						int wordCount = xNode1.getChildNode("words").nChildNode("word");
						
						int myIterator3 = 0;
						for(int r=0;r<wordCount;r++)
						{
							temp = xNode1.getChildNode("words").getChildNode("word", &myIterator3).getAttribute("v");
							
							if(words.linearSearch((char *)temp) == 1)
							{
								for(jj=0;jj<strlen(temp);jj++)
									sendRoomList[gi+jj] = temp[jj];		// word
								
								sendRoomList[gi+jj] = ' ';
								sendRoomList[gi+jj+1] = '<';
								sendRoomList[gi+jj+2] = 'i';
								sendRoomList[gi+jj+3] = '>';
								sendRoomList[gi+jj+4] = '(';
								
								gi = gi+jj+5;
								
								char tempPoints[10];
								sprintf(tempPoints, "%d", rooms[roomIndex].wordPoints[(strlen(temp)-1)]);
								
								for(jj=0;jj<strlen(tempPoints);jj++)
									sendRoomList[gi+jj] = tempPoints[jj];	
								gi = gi+jj;
								
								char *temp2 = " points)</i><br>";
								
								for(jj=0;jj<strlen(temp2);jj++)
									sendRoomList[gi+jj] = temp2[jj];	
								gi = gi+jj;
							}
							else
							{
								for(jj=0;jj<strlen(temp);jj++)
									sendRoomList[gi+jj] = temp[jj];		// word
								
								gi = gi+jj;
								
								char *temp2 = " <i>(Not in dictionary)</i><br>";
								
								for(jj=0;jj<strlen(temp2);jj++)
									sendRoomList[gi+jj] = temp2[jj];	
								gi = gi+jj;
							}
						}
						
						break;
					}
				}
				
				
			}
			
			sendRoomList[gi] = '\0';
			
			printf("log request sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, (totalMessageSize+1), 0) != (totalMessageSize+1))
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case REGISTER:
			wordL[0] = messageBuffer[4];
			wordL[1] = '\0';
			wordLength = atoi(wordL)+1;
			word = new char[wordLength+1];
			for(int i=0; i<wordLength; i++)
				word[i] = messageBuffer[5+i];
			word[wordLength] = '\0';			// username
			
			roomL[0] = messageBuffer[5+wordLength];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[6+wordLength+i];
			room[roomLength] = '\0'; 			// password
			
			xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
	
			totalPlayerCount = xMainNode.nChildNode("player");
			
			for(int j=0; j<totalPlayerCount; j++)
			{
				XMLNode player = xMainNode.getChildNode("player",&myIterator);
				if(strcmp(player.getChildNode("username").getAttribute("v"), word) == 0)
				{
					kayittaVar = true;
					break;
				}
			}
		
			sendRoomList = (char *)malloc(sizeof(char)*3);
			
			sendRoomList[0] = 'H';
			
			if(!(kayittaVar))			// kaydet
			{
				int totalXmlLength = 109+wordLength+roomLength;
				char *xmlString = (char *)malloc(sizeof(char)*(totalXmlLength+1));
				
				char *temp = "<player><username v=";
			
				int j=0;
				for(j=0; j<strlen(temp); j++)
					xmlString[j] = temp[j];
				xmlString[j] = '\"';
				
				gi = j+1;
				
				for(j=0; j<wordLength; j++)
					xmlString[gi+j] = word[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = "/><password v=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				for(j=0; j<roomLength; j++)
					xmlString[gi+j] = room[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = "/><id v=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = ChangeToUserId(totalPlayerCount+1);
				
				xmlString[gi] = temp[0];
				xmlString[gi+1] = temp[1];
				xmlString[gi+2] = temp[2];
				xmlString[gi+3] = '\"';
				
				gi = gi+4;
				
				temp = "/><rating v=";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\"';
				
				gi = gi+j+1;
				
				temp = ChangeToUserId(totalPlayerCount+1);
				
				xmlString[gi] = temp[0];
				xmlString[gi+1] = temp[1];
				xmlString[gi+2] = temp[2];
				xmlString[gi+3] = '\"';
				
				gi = gi+4;
				
				temp = "/><points v=\"0\"/><gcount v=\"0\"/></player>";
			
				for(j=0; j<strlen(temp); j++)
					xmlString[gi+j] = temp[j];
				xmlString[gi+j] = '\0';
				
				printf("str: %s\n", xmlString);
			
				XMLNode xNode = XMLNode::parseString(xmlString);
				xMainNode.addChild(xNode,0);
			
				char *t = xMainNode.createXMLString(true);
				
				FILE * pFile;
				pFile = fopen ("players.xml","w");
				
				fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n",pFile);
				fputs (t,pFile);
				fclose (pFile);
				
				free(t);				
				
				sendRoomList[1] = '1';
			}
			else					// username kayitta var, kaydetme
			{
				sendRoomList[1] = '0';
			}
			
			sendRoomList[2] = '\0';
			
			printf("register sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, 3, 0) != 3)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case LOGIN:
			wordL[0] = messageBuffer[4];
			wordL[1] = '\0';
			wordLength = atoi(wordL)+1;
			word = new char[wordLength+1];
			for(int i=0; i<wordLength; i++)
				word[i] = messageBuffer[5+i];
			word[wordLength] = '\0';			// username
			
			roomL[0] = messageBuffer[5+wordLength];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[6+wordLength+i];
			room[roomLength] = '\0'; 			// password
			
			xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
	
			totalPlayerCount = xMainNode.nChildNode("player");
			
			for(int j=0; j<totalPlayerCount; j++)
			{
				XMLNode player = xMainNode.getChildNode("player",&myIterator);
				if((strcmp(player.getChildNode("username").getAttribute("v"), word) == 0) &&
				   (strcmp(player.getChildNode("password").getAttribute("v"), room) == 0))
				{
 					kayittaVar = true;
					myIterator--;
					break;
				}
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*6);
			
			sendRoomList[0] = 'I';
			
			if(kayittaVar)			// login basarili
			{
				XMLNode player = xMainNode.getChildNode("player",&myIterator);
				
				temp = player.getChildNode("id").getAttribute("v");
				
				sendRoomList[1] = temp[0];
				sendRoomList[2] = temp[1];
				sendRoomList[3] = temp[2];				
				sendRoomList[4] = '1';
			}
			else				// login basarisiz
			{
				sendRoomList[1] = '0';
				sendRoomList[2] = '0';
				sendRoomList[3] = '0';
				sendRoomList[4] = '0';
			}
			
			sendRoomList[5] = '\0';
			
			printf("login sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, 6, 0) != 6)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case LEARNNAME:
			xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
	
			totalPlayerCount = xMainNode.nChildNode("player");
			
			for(int ij=0;ij<totalPlayerCount;ij++)
			{
				XMLNode player = xMainNode.getChildNode("player",&myIterator);
				if(strcmp(player.getChildNode("id").getAttribute("v"), userId) == 0)
				{
					word = (char *)player.getChildNode("username").getAttribute("v");
					break;
				}
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*(3+strlen(word)));
			
			sendRoomList[0] = 'J';
			sendRoomList[1] = (strlen(word)-1) + '0'; 
			
			for(ii=0;ii<strlen(word);ii++)
				sendRoomList[2+ii] = word[ii];
			
			sendRoomList[2+ii] = '\0';
			
			printf("learn name sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, 3+ii, 0) != 3+ii)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case ACTIVEUSERS:
			roomL[0] = messageBuffer[4];
			roomL[1] = '\0';
			roomLength = atoi(roomL)+1;
			room = new char[roomLength+1];
			for(int i=0; i<roomLength; i++)
				room[i] = messageBuffer[5+i];
			room[roomLength] = '\0'; 
			
			xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
			
			totalPlayerCount = xMainNode.nChildNode("player");
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					totalMessageSize = 2;
					
					char tempP[10];
					sprintf(tempP, "%d", rooms[i].playerCount);
					
					totalMessageSize += strlen(tempP);
					
					for(int j=0;j<rooms[i].playerCount;j++)
					{
						int myIterator8=0;
						for(int ij=0;ij<totalPlayerCount;ij++)
						{
							XMLNode player = xMainNode.getChildNode("player",&myIterator8);
							if(strcmp(player.getChildNode("id").getAttribute("v"), rooms[i].players[j].userId) == 0)
							{
								word = (char *)player.getChildNode("username").getAttribute("v");
								break;
							}
						}
						
						totalMessageSize += 4;
						totalMessageSize += strlen(word);
					}
					
				}
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
			
			sendRoomList[0] = 'K';
			
			for(int i=0;i<roomCount;i++)
			{
				if(strcmp(room, rooms[i].room) == 0)
				{
					char tempP[10];
					sprintf(tempP, "%d", rooms[i].playerCount);
					
					sendRoomList[1] = strlen(tempP) + '0';
					
					int ji=0;
					for(ji=0; ji<strlen(tempP); ji++)
						sendRoomList[2+ji] = tempP[ji];
					gi = 2+ji;
					
					for(int j=0;j<rooms[i].playerCount;j++)
					{
						int myIterator8=0;
						for(int ij=0;ij<totalPlayerCount;ij++)
						{
							XMLNode player = xMainNode.getChildNode("player",&myIterator8);
							if(strcmp(player.getChildNode("id").getAttribute("v"), rooms[i].players[j].userId) == 0)
							{
								word = (char *)player.getChildNode("username").getAttribute("v");
								break;
							}
						}
						
						sendRoomList[gi] = rooms[i].players[j].userId[0];
						sendRoomList[gi+1] = rooms[i].players[j].userId[1];
						sendRoomList[gi+2] = rooms[i].players[j].userId[2];
						
						sendRoomList[gi+3] = strlen(word) + '0';
						
						for(ji=0; ji<strlen(word); ji++)
							sendRoomList[gi+4+ji] = word[ji];
						gi = gi+4+ji;
					}
					
				}
			}
			
			sendRoomList[gi] = '\0';
			
			printf("active users sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, gi, 0) != gi)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		case RANKINGS:
			xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
			
			totalPlayerCount = xMainNode.nChildNode("player");
			
			sprintf(tempRanksL, "%d", totalPlayerCount);
			
			totalMessageSize = 2;
			totalMessageSize += strlen(tempRanksL);
			
			for(int i=0; i<totalPlayerCount; i++)
			{
				XMLNode player = xMainNode.getChildNode("player",&myIterator);
				
				word = (char *)player.getChildNode("username").getAttribute("v");
				room = (char *)player.getChildNode("points").getAttribute("v");
				
				totalMessageSize += 2+strlen(word)+strlen(room);
			}
			
			sendRoomList = (char *)malloc(sizeof(char)*(totalMessageSize+1));
			
			sendRoomList[0] = 'L';
			sendRoomList[1] = strlen(tempRanksL)+'0';
			
			for(ii=0;ii<strlen(tempRanksL);ii++)
				sendRoomList[2+ii] = tempRanksL[ii];
			
			gi = 2+ii;
			
			for(int i=1; i<totalPlayerCount+1; i++)
			{
				myIterator = 0;
				for(int j=0;j<totalPlayerCount;j++)
				{
					XMLNode player = xMainNode.getChildNode("player",&myIterator);
				
					char *rankL = (char *)player.getChildNode("rating").getAttribute("v");
					
					char *tempR = ChangeToUserId(i);
					
					if(strcmp(rankL, tempR) == 0)
					{
						word = (char *)player.getChildNode("username").getAttribute("v");
						room = (char *)player.getChildNode("points").getAttribute("v");
				
						sendRoomList[gi] = (strlen(word)-1)+'0';
				
						for(ii=0;ii<strlen(word);ii++)
							sendRoomList[gi+1+ii] = word[ii];
				
						gi = gi+1+ii;
				
						sendRoomList[gi] = strlen(room)+'0';
				
						for(ii=0;ii<strlen(room);ii++)
							sendRoomList[gi+1+ii] = room[ii];
				
						gi = gi+1+ii;
						
						break;
					}
					else
						continue;	
				}
			}
			
			sendRoomList[gi] = '\0';
			
			printf("rankings sent message : %s\n", sendRoomList);
			
			if (send(clientSock, sendRoomList, gi+1, 0) != gi+1)
				GiveErrorAndDie("send() hata verdi");
			
			break;
		default:
			printf("Yanlis kodlu mesaj geldi\n");	
			break;
	}
	close(clientSock);    /* Close client socket */
}

char* ServerConnection::ChangeToUserId(int n)
{
	char *temp = (char *)malloc(sizeof(char)*4);
	
	int araHesap, araHesap2;
	char number[2];
	
	if(n > 99)
	{
		araHesap = n % 100;
		araHesap = (n - araHesap)/100;
		sprintf(number, "%d", araHesap);
		temp[0] = number[0];
	}
	else
		temp[0] = '0';
	
	if(n > 9)
	{
		araHesap = n % 100;
		araHesap2 = (araHesap - (araHesap % 10))/10;
		sprintf(number, "%d", araHesap2);
		temp[1] = number[0];
	}
	else
		temp[1] = '0';
	
	araHesap = n % 10;
	sprintf(number, "%d", araHesap);
	temp[2] = number[0];
	
	temp[3] = '\0';
	
	return temp;
}

int compare(const void *a, const void *b)
{
	struct Player * aa;
	struct Player * bb;
	
	aa = (struct Player *)a;
	bb = (struct Player *)b;
	
	if(aa->points > bb->points)
		return -1;
	else if(aa->points < bb->points)
		return 1;
	
	return 0;
}

int compare2(const void *a, const void *b)
{
	struct PlayerRanking * aa;
	struct PlayerRanking * bb;
	
	aa = (struct PlayerRanking *)a;
	bb = (struct PlayerRanking *)b;
	
	if(aa->points > bb->points)
		return -1;
	else if(aa->points < bb->points)
		return 1;
	
	return 0;
}

void ServerConnection::CalculateResults(int roomIndex)
{
	char *sendMessage;
	int totalMessageSize=0;
	char number[2];
	
	if(rooms[roomIndex].isUniqueCounts)
	{
		
	}
	else
	{
		qsort(rooms[roomIndex].players, rooms[roomIndex].playerCount, sizeof(struct Player), compare);
	}
	
	printf("results are calculated\n");
}

int ServerConnection::ChangetoInt(char *temp)
{
	int i=0;
	
	while(temp[0] != '\0')
	{
		i = i*10 + temp[0] - '0';
		temp = temp+1;
	}
	
	return i;
}

void ServerConnection::CalculateAndSavePlayerRatings(int roomIndex)
{
	int rPlayerCount = rooms[roomIndex].lastPlayerCount;
	Player *rPlayers = rooms[roomIndex].lastPlayers;
	
	XMLNode xMainNode = XMLNode::openFileHelper("players.xml","PLAYERS");
	
	int totalPlayerCount = xMainNode.nChildNode("player");
	
	PlayerRanking *ranks = (PlayerRanking *)malloc(sizeof(PlayerRanking)*totalPlayerCount);
	
	int myIterator = 0;
	for(int i=0; i<totalPlayerCount; i++)
	{
		XMLNode player = xMainNode.getChildNode("player",&myIterator);
		
		char *user = (char *)player.getChildNode("id").getAttribute("v");
		char *points = (char *)player.getChildNode("points").getAttribute("v");
		char *gcount = (char *)player.getChildNode("gcount").getAttribute("v");
		
		ranks[i].userId = user;
		ranks[i].points = atoi(points);
		ranks[i].gameCount = atoi(gcount);
	
	}
	
	for(int i=0; i<rPlayerCount; i++)
	{
		for(int j=0; j<totalPlayerCount; j++)
		{
			if(strcmp(ranks[j].userId, rPlayers[i].userId) == 0)
			{
				ranks[j].gameCount++;
				ranks[j].points = ((ranks[j].points*ranks[j].gameCount)+rPlayers[i].points)/ranks[j].gameCount;
				break;
			}
		}
	}
	
	qsort(ranks, totalPlayerCount, sizeof(struct PlayerRanking), compare2);
	
	for(int i=1; i<totalPlayerCount+1; i++)
	{
		int myIterator = 0;
		for(int j=0; j<totalPlayerCount; j++)
		{
			XMLNode player = xMainNode.getChildNode("player",&myIterator);
			if(strcmp(player.getChildNode("id").getAttribute("v"), ranks[(i-1)].userId) == 0)
			{
				char *newrank = ChangeToUserId(i);
				
				player.getChildNode("rating").updateAttribute(newrank,NULL,"v");
				
				char tempP[10];
				sprintf(tempP, "%d", ranks[(i-1)].points);
				
				player.getChildNode("points").updateAttribute(tempP,NULL,"v");
				
				char tempP2[10];
				sprintf(tempP2, "%d", ranks[(i-1)].gameCount);
				
				player.getChildNode("gcount").updateAttribute(tempP2,NULL,"v");
			}
		}
	}
	
	char *tX = xMainNode.createXMLString(true);
				
	FILE * pFile;
	pFile = fopen ("players.xml","w");
				
	fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n",pFile);
	fputs (tX,pFile);
	fclose (pFile);
				
	free(tX);
	
	free(ranks);
	
	printf("player rankings updated\n");
}

void ServerConnection::CheckRoomsStatus()
{
	for(int i=0;i<roomCount;i++)
	{
		time_t seconds = time(NULL);
		if(!(rooms[i].gameStarted))
		{
			if((seconds - rooms[i].breaktime) > 15) // break bitti
			{
				rooms[i].gameStarted = true;
				rooms[i].starttime = time(NULL);
			
				printf("%s Odasinda oyun basladi\n", rooms[i].room);
			}
		}
		else
		{
			if((seconds - rooms[i].starttime) > rooms[i].time) // sure bitti
			{
				rooms[i].gameStarted = false;
				rooms[i].resultsCalculated = false;
				rooms[i].breaktime = time(NULL);
				printf("%s Odasinda oyun bitti\n", rooms[i].room);
				
				CalculateResults(i);			// sonuclari hesapla
				
				if(rooms[i].lastPlayers != NULL) 
				{
					for(int j=0;j<rooms[i].lastPlayerCount;j++)
					{
						free(rooms[i].lastPlayers[j].userId);
						
						for(int k=0;k<rooms[i].lastPlayers[j].wordCount;k++)
							free(rooms[i].lastPlayers[j].words[k]);
						free(rooms[i].lastPlayers[j].words);
					}
					free(rooms[i].lastPlayers);
				}
				rooms[i].lastPlayers = rooms[i].players;
				rooms[i].lastPlayerCount = rooms[i].playerCount;
				
				
				if(strcmp(rooms[i].room, "Official")==0)
					CalculateAndSavePlayerRatings(i);	// playerlarin rankingini guncelle
				
				rooms[i].resultsCalculated = true;
				rooms[i].players = NULL;		// playerlari resetle
				rooms[i].playerCount = 0;
				
				char *lastBoard = rooms[i].board;
				
				rooms[i].board = CreateRandomBoard(rooms[i].rowCount, rooms[i].columnCount); // yeni board olustur
				
				// log u yaz
				
				int totalXmlLength = 65;
				totalXmlLength += strlen(rooms[i].room);
				
				time_t rawtime;
				struct tm * timeinfo;
				time ( &rawtime );
				timeinfo = localtime ( &rawtime );
				totalXmlLength += strlen(asctime (timeinfo))-1;
				
				totalXmlLength += rooms[i].rowCount * rooms[i].columnCount;
				
				for(int j=0;j<rooms[i].lastPlayerCount;j++)
				{
					totalXmlLength += 59;
					char temp1[10];
					sprintf(temp1, "%d", rooms[i].lastPlayers[j].points);
					totalXmlLength += strlen(temp1);
					
					for(int k=0;k<rooms[i].lastPlayers[j].wordCount;k++)
					{
						totalXmlLength += 12;
						totalXmlLength += strlen(rooms[i].lastPlayers[j].words[k]);
					}
				}
				
				char *xmlString = (char *)malloc(sizeof(char)*(totalXmlLength+1));
				
				char *temp = "<room v=";
			
				int j1=0;
				for(j1=0; j1<strlen(temp); j1++)
					xmlString[j1] = temp[j1];
				xmlString[j1] = '\"';
				
				int j2=0;
				for(j2=0; j2<strlen(rooms[i].room); j2++)
					xmlString[j1+1+j2] = rooms[i].room[j2];
				xmlString[j1+1+j2] = '\"';
				
				char *temp2 = "><time v=";
				
				int j3=0;
				for(j3=0; j3<strlen(temp2); j3++)
					xmlString[j1+2+j2+j3] = temp2[j3];
				xmlString[j1+2+j2+j3] = '\"';
				
				char *temp3 = asctime (timeinfo);
				
				int j4=0;
				for(j4=0; j4<strlen(temp3)-1; j4++)
					xmlString[j1+3+j2+j3+j4] = temp3[j4];
				xmlString[j1+3+j2+j3+j4] = '\"';
				
				char *temp4 = "/><board v=";
				
				int j5=0;
				for(j5=0; j5<strlen(temp4); j5++)
					xmlString[j1+4+j2+j3+j4+j5] = temp4[j5];
				xmlString[j1+4+j2+j3+j4+j5] = '\"';
				
				int j6=0;
				for(j6=0; j6<strlen(lastBoard); j6++)
					xmlString[j1+5+j2+j3+j4+j5+j6] = lastBoard[j6];
				xmlString[j1+5+j2+j3+j4+j5+j6] = '\"';
				
				// last board i temizle
				
				char *temp5 = "/><players>";
				
				int j7=0;
				for(j7=0; j7<strlen(temp5); j7++)
					xmlString[j1+6+j2+j3+j4+j5+j6+j7] = temp5[j7];
				
				int t = j1+6+j2+j3+j4+j5+j6+j7;
				
				for(int m=0;m<rooms[i].lastPlayerCount;m++)
				{
					char *temp6 = "<player><id v=";
					int j8=0;
					for(j8=0; j8<strlen(temp6); j8++)
						xmlString[t+j8] = temp6[j8];
					xmlString[t+j8] = '\"';
					
					xmlString[t+j8+1] = rooms[i].lastPlayers[m].userId[0];
					xmlString[t+j8+2] = rooms[i].lastPlayers[m].userId[1];
					xmlString[t+j8+3] = rooms[i].lastPlayers[m].userId[2];
					xmlString[t+j8+4] = '\"';
					
					char *temp7 = "/><points v=";
					int j9=0;
					for(j9=0; j9<strlen(temp7); j9++)
						xmlString[t+j8+5+j9] = temp7[j9];
					xmlString[t+j8+5+j9] = '\"';
					
					char temp8[10];
					sprintf(temp8, "%d", rooms[i].lastPlayers[m].points);
					
					int j10=0;
					for(j10=0; j10<strlen(temp8); j10++)
						xmlString[t+j8+6+j9+j10] = temp8[j10];
					xmlString[t+j8+6+j9+j10] = '\"';
					
					char *temp9 = "/><words>";
				
					int j11=0;
					for(j11=0; j11<strlen(temp9); j11++)
						xmlString[t+j8+7+j9+j10+j11] = temp9[j11];
					
					int t2 = t+j8+7+j9+j10+j11;
					
					for(int p=0;p<rooms[i].lastPlayers[m].wordCount;p++)
					{
						char *temp10 = "<word v=";
						int j12=0;
						for(j12=0; j12<strlen(temp10); j12++)
							xmlString[t2+j12] = temp10[j12];
						xmlString[t2+j12] = '\"';
						
						int j13=0;
						for(j13=0; j13<strlen(rooms[i].lastPlayers[m].words[p]); j13++)
							xmlString[t2+j12+1+j13] = rooms[i].lastPlayers[m].words[p][j13];
						xmlString[t2+j12+1+j13] = '\"';
						
						xmlString[t2+j12+2+j13] = '/';
						xmlString[t2+j12+3+j13] = '>';
						
						t2 = t2+j12+4+j13;
					}
					
					char *temp11 = "</words></player>";
				
					int j14=0;
					for(j14=0; j14<strlen(temp11); j14++)
						xmlString[t2+j14] = temp11[j14];
					
					t = t2+j14;
				}
				
				char *temp12 = "</players></room>";
				
				int j15=0;
				for(j15=0; j15<strlen(temp12); j15++)
					xmlString[t+j15] = temp12[j15];
				xmlString[t+j15] = '\0';
				
				XMLNode xMainNode = XMLNode::openFileHelper("log.xml","BOGGLELOG");
				XMLNode xNode = XMLNode::parseString(xmlString);
				xMainNode.addChild(xNode,0);
			
				char *tX = xMainNode.createXMLString(true);
				
				FILE * pFile;
				pFile = fopen ("log.xml","w");
				
				fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n",pFile);
				fputs (tX,pFile);
				fclose (pFile);
				
				free(tX);
				printf("room log updated\n");
			}
		}
	}
}

void ServerConnection::GiveErrorAndDie(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}
