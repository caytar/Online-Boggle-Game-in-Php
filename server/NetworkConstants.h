#define MAXPENDING		5		/* Maximum outstanding connection requests */
#define MESSAGEBUFFER		1024
#define ROOMCREATE		'A'		/* To create rooms */
#define WORDCONTROL		'B'		/* To control the words from dictionary */
#define STARTGAME		'C'		/* To start a new game in a room */
#define ROOMLIST		'D'		/* To list the existing rooms */
#define ENDGAME			'E'		/* To inform the client that the game finished in a room */
#define GAMERESULTS		'F'		/* To send the results to the clients */
#define LOGREQUEST		'G'		/* To send the log history to the user */
#define REGISTER		'H'		/* To register the user to the game */
#define LOGIN			'I'		/* To look the login of the user to the game */
#define LEARNNAME		'J'		/* To learn the name of the user */
#define ACTIVEUSERS		'K'		/* To learn the active users in the room */
#define RANKINGS		'L'		/* To send the all rankings to the user */


