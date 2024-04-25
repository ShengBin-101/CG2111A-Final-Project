#include "make_tls_client.h"
// define colors
#define WHITE 0
#define RED 1
#define GREEN 2
#define TICKS_THRESHOLD 0.5
// Network packet types
#include "netconstants.h"

// Packet types, error codes, etc.
#include "constants.h"
#include <queue>
#include <termios.h>

// added comment
// Tells us that the network is running.
static volatile int networkActive = 0;
bool manualMode = true;
void handleError(const char *buffer)
{
	switch (buffer[1])
	{
	case RESP_OK:
		printf("Command / Status OK\n");
		break;

	case RESP_BAD_PACKET:
		printf("BAD MAGIC NUMBER FROM ARDUINO\n");
		break;

	case RESP_BAD_CHECKSUM:
		printf("BAD CHECKSUM FROM ARDUINO\n");
		break;

	case RESP_BAD_COMMAND:
		printf("PI SENT BAD COMMAND TO ARDUINO\n");
		break;

	case RESP_BAD_RESPONSE:
		printf("PI GOT BAD RESPONSE FROM ARDUINO\n");
		break;

	default:
		printf("PI IS CONFUSED!\n");
	}
}

void handleStatus(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));

	printf("\n ------- ALEX STATUS REPORT ------- \n\n");
	printf("Left Forward Ticks:\t\t%d\n", data[0]);
	printf("Right Forward Ticks:\t\t%d\n", data[1]);
	printf("Left Reverse Ticks:\t\t%d\n", data[2]);
	printf("Right Reverse Ticks:\t\t%d\n", data[3]);
	printf("Left Forward Ticks Turns:\t%d\n", data[4]);
	printf("Right Forward Ticks Turns:\t%d\n", data[5]);
	printf("Left Reverse Ticks Turns:\t%d\n", data[6]);
	printf("Right Reverse Ticks Turns:\t%d\n", data[7]);
	printf("Forward Distance:\t\t%d\n", data[8]);
	printf("Reverse Distance:\t\t%d\n", data[9]);
	printf("\n---------------------------------------\n\n");
}

void handleUltrasonicMessage(const char *buffer)
{
	printf("Ultrasonic Sensor Distance Reading: %d\n", buffer[1]);
}

void handleColorMessage(const char *buffer)
{
	// check buffer[1] and determine color
	switch (buffer[1])
	{
	case WHITE:
		printf("Colour Detected: White\n");
		break;
	case RED:
		printf("Colour Detected: Red\n");
		break;
	case GREEN:
		printf("Colour Detected: Green\n");
		break;
	default:
		printf("Colour Detected: Unknown\n");
		break;
	}

	printf("Colour Detected ID: %s\n", &buffer[1]);
}

void handleBuzzerMessage()
{
	printf("Buzzer Activated\n");
}

void handleMessage(const char *buffer)
{
	printf("MESSAGE FROM ALEX: %s\n", &buffer[1]); // also used to indicate speed
}

void handleCommand(const char *buffer)
{
	// We don't do anything because we issue commands
	// but we don't get them. Put this here
	// for future expansion
}

void handleModeMessage(const char *buffer, bool *manualMode)
{
	printf("Manual Mode: %s\n", buffer[1] ? "ON" : "OFF");
	*manualMode = buffer[1];
}

void handleNetwork(const char *buffer, int len)
{
	// The first byte is the packet type
	int type = buffer[0];

	switch (type)
	{
	case NET_ERROR_PACKET:
		handleError(buffer);
		break;

	case NET_STATUS_PACKET:
		handleStatus(buffer);
		break;

	// we use message packet for the colour sensor and ultra sonic sensor
	case NET_ULTRASONIC_MESSAGE_PACKET:
		handleUltrasonicMessage(buffer);
		break;

	case NET_COMMAND_PACKET:
		handleCommand(buffer);
		break;

	case NET_COLOR_MESSAGE_PACKET:
		handleColorMessage(buffer);
		break;

	case NET_BUZZER_PACKET:
		handleBuzzerMessage();
		break;

	case NET_MESSAGE_PACKET:
		handleMessage(buffer);
		break;

	case NET_MANUALMODE_PACKET:
		handleModeMessage(buffer, &manualMode);
		break;
	}
}

void sendData(void *conn, const char *buffer, int len)
{
	int c;
	printf("\nSENDING %d BYTES DATA\n\n", len);
	if (networkActive)
	{
		/* TODO: Insert SSL write here to write buffer to network */
		c = sslWrite(conn, buffer, len);
		if (c < 0)
		{
			perror("Error writing to network: ");
		}
		/* END TODO */
		networkActive = (c > 0);
	}
}

void *readerThread(void *conn)
{
	char buffer[128];
	int len;

	while (networkActive)
	{
		/* TODO: Insert SSL read here into buffer */

		len = sslRead(conn, buffer, sizeof(buffer));

		printf("read %d bytes from server.\n", len);

		/* END TODO */

		networkActive = (len > 0);

		if (networkActive)
			handleNetwork(buffer, len);
	}

	printf("Exiting network listener thread\n");

	/* TODO: Stop the client loop and call EXIT_THREAD */
	stopClient();
	EXIT_THREAD(conn);
	/* END TODO */

	return NULL;
}

void flushInput()
{
	char c;

	while ((c = getchar()) != '\n' && c != EOF)
		;
}

void getParams(int32_t *params)
{
	printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
	printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
	scanf("%d %d", &params[0], &params[1]);
	flushInput();
}

char getch()
{
	struct termios term, oterm;
	char c = 0;

	tcgetattr(0, &oterm);				 // Obtain current terminal attributes
	memcpy(&term, &oterm, sizeof(term)); // Copy over into term
	term.c_lflag &= ~(ICANON | ECHO);	 // Disable ICANON and ECHO
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &term);  // Apply changes
	c = getchar();				   // Block function
	tcsetattr(0, TCSANOW, &oterm); // Revert changes
	return c;
}

void *writerThread(void *conn)
{
	int quit = 0;
	clock_t old = clock();

	while (!quit)
	{
		char buffer[10] = {};
		buffer[0] = NET_COMMAND_PACKET;
		if (manualMode)
		{
			char ch;
			printf("Command (f=forward, b=reverse, l=turn left, r=turn right, s=stop, c=clear stats, g=get stats q=exit)\n");
			printf("(y=color, u=ultrasonic, i=start_tone, o=red_found, p=green_found, [=end_tone, ]=tokyo_drift)\n");
			// printf("Command (f=forward, b=reverse, l=turn left, r=turn right, s=stop, c=clear stats, g=get stats q=exit)\nAddit, m=Toggle Manual Mode\n");
			scanf("%c", &ch);

			// Purge extraneous characters from input stream
			flushInput();

			int32_t params[2];

			switch (ch)
			{
			case 'f':
			case 'F':
			case 'b':
			case 'B':
			case 'l':
			case 'L':
			case 'r':
			case 'R':
				getParams(params);
				buffer[1] = ch;
				memcpy(&buffer[2], params, sizeof(params));
				sendData(conn, buffer, sizeof(buffer));
				break;
			case 's':
			case 'S':
			case 'c':
			case 'C':
			case 'g':
			case 'G':
				params[0] = 0;
				params[1] = 0;
				memcpy(&buffer[2], params, sizeof(params));
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			case 'q':
			case 'Q':
				quit = 1;
				break;
			// case to activate colour sensor
			case 'y':
			case 'Y':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active ultra sonic sensor
			case 'u':
			case 'U':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active buzzer_1 (start_tone)
			case 'i':
			case 'I':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active buzzer_2 (low_hp)
			case 'o':
			case 'O':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active buzzer_3 (caught)
			case 'p':
			case 'P':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active buzzer_4 (celebrate)
			case '[':
			case '{':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to active buzzer_5 (tokyo_drift)
			case ']':
			case '}':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
			// case to activate stable mode (stable_mode == 0 means robot moves slowly)
			case 'm':
			case 'M':
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
			    sleep(2);
				break;

			default:
				printf("BAD COMMAND\n");
			}
		}
		else
		{
			printf("===============FREE TELE-OP===============\n");
			printf("WASD - movements,\ny - color\nu - ultrasound,\n");
			printf("i - start_tone\no - red_found\np - green_found\n[ - end_tone\n] - tokyo_drift\n");
			printf("==========================================\n");

			
			// char ch;
			// scanf("%c", &ch);

			// Purge extraneous characters from input stream
			// flushInput();

			char ch = getch();
			// flushInput();
			switch (ch)
			{
			case 'w':
				buffer[1] = 'f';
				break;
			case 'a':
				buffer[1] = 'l';
				break;
			case 's':
				buffer[1] = 'b';
				break;
			case 'd':
				buffer[1] = 'r';
				break;
			case 'm':
				buffer[1] = ch; // Sending 'm' should also flip something in Arduino
				break;
			// Color Sensor
			case 'y':
			case 'Y':
			    buffer[1] = ch;
				break;
			// Ultrasound
			case 'U':
			case 'u':
				buffer[1] = ch;
				break;
			// Buzzer_1
			case 'i':
			case 'I':
				buffer[1] = ch;
				break;
			// Buzzer_2
			case 'o':
			case 'O':
				buffer[1] = ch;
				break;
			// Buzzer_3
			case 'p':
			case 'P':
				buffer[1] = ch;
				break;
			// Buzzer_4
			case '[':
			case '{':
				buffer[1] = ch;
				break;
			// Buzzer_5
			case ']':
			case '}':
				buffer[1] = ch;
				break;

			case ' ':
			default: // default stop
				buffer[1] = 's';
				break;
			}
			// SENDING MESSAGE ONLY IF ELAPSED TIME IS REASONABLE
			if (((double)(clock() - old) / double(CLOCKS_PER_SEC)) > TICKS_THRESHOLD)
			{
				sendData(conn, buffer, sizeof(buffer));
				sleep(0.7);
			}
			old = clock();
			
		}
	}

	printf("Exiting keyboard thread\n");

	/* TODO: Stop the client loop and call EXIT_THREAD */
	stopClient();
	EXIT_THREAD(conn);
	/* END TODO */

	return NULL;
}

/* TODO: #define filenames for the client private key, certificatea,
   CA filename, etc. that you need to create a client */
#define SERVER_NAME "172.31.198.100" // IP address of the server also defined by user input, this is just here for fun
#define CA_CERT_FNAME "signing.pem"
#define PORT_NUM 5001 // PORT NAME is defined by user input, this is just here for fun
#define CLIENT_CERT_FNAME "laptop.crt"
#define CLIENT_KEY_FNAME "laptop.key"
#define SERVER_NAME_ON_CERT "alex"

/* END TODO */
void connectToServer(const char *serverName, int portNum)
{
	/* TODO: Create a new client */
	createClient(serverName, portNum, 1, CA_CERT_FNAME, SERVER_NAME_ON_CERT, 1, CLIENT_CERT_FNAME, CLIENT_KEY_FNAME, readerThread, writerThread);

	/* END TODO */
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		fprintf(stderr, "\n\n%s <IP address> <Port Number>\n\n", av[0]);
		exit(-1);
	}

	networkActive = 1;
	connectToServer(av[1], atoi(av[2]));

	/* TODO: Add in while loop to prevent main from exiting while the
	client loop is running */

	while (client_is_running())
		;

	/* END TODO */
	printf("\nMAIN exiting\n\n");
}
