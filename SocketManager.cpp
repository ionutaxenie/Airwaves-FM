/*
 * SocketManager.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: ionut
 */

#include "SocketManager.h"

// Empty constructor
SocketManager::SocketManager()
{
}

// Empty destructor
SocketManager::~SocketManager()
{
}

// Constructor that does socket initialization
SocketManager::SocketManager(char * ip, int port)
{
	Init(ip, port);
}

// Method that initializes the socket
void SocketManager::Init(char * ip, int port)
{
	strcpy(ipAddress, ip);
	portno = port;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		printf("ERROR opening socket\n");
	server = gethostbyname(ipAddress);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
	}
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)server->h_addr,
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);
	serv_addr.sin_port = htons(portno);
}

// Connect to the socket
int SocketManager::ConnectToSocket()
{
	int res;
	res = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (res < 0)
	{
		printf("Could not connect to the socket!\n");
	}

	return res;
}

// Method that writes to socket
// Returns the number of bytes successfully written on SUCCES or -1 on FAILURE
int SocketManager::WriteToSocket(void * message, unsigned int size)
{
	return send(sockfd, message, size, 0);
}

// Method that reads from socket
// Returns the number of bytes successfully read on SUCCES or -1 on FAILURE
int SocketManager::ReadFromSocket(unsigned char * message, int numberOfBytes)
{
	return recv(sockfd, message, numberOfBytes, MSG_WAITALL);
}

// Method that disconnects from socket
// Returns 0 on SUCCESS or -1 on FAILURE
int SocketManager::DisconnectFromSocket()
{
	int res;
	SocketCommand closeCommand = SocketCommand::GetCloseCommand();
	res = SendCommand(closeCommand);
	if (res < 0)
	{
		return -1;
	}
	return close(sockfd);
}

// Method that starts the rtl_tcp program and connects to the socket
int SocketManager::ConnectToDevice(unsigned int frequency, char * gain, unsigned int sampleRate, bool supressOutput)
{
	char command[256];

	// Prepare the system command that will start rtl_tcp
	sprintf(command, "rtl_tcp -a %s -p %i -f %u -g %s -s %u", ipAddress, portno, frequency, gain, sampleRate);

	// Decide if rtl_tcp will show output
	if (supressOutput)
	{
		sprintf(command, "%s > /dev/null 2>&1 &", command);
	}
	else
	{
		sprintf(command, "%s &", command);
	}
	printf ("%s\n", command);

	// Start the rtl_tcp program
	if (system(command) < 0)
	{
		printf("Unable to start RTL TCP daemon!\n");
		return -1;
	}

	sleep(1);

	return ConnectToSocket();
}

// Method that kills the rtl_tcp program and breaks the connection
void SocketManager::DisconnectFromDevice()
{
	system("pkill -9 rtl_tcp");
}

// Method for sending a command to the rtl_tcp client via socket
int SocketManager::SendCommand(SocketCommand command)
{
	int res;
	Command cmd = command.GetCommand();
	switch(cmd)
	{
	case Command::CLOSE_RTL_TCP:
		res = WriteToSocket( &cmd, 1);
		if (res < 0)
		{
			printf("Could not close RTL_TCP!\n");
			return -1;
		}
		break;
	default:
		res = WriteToSocket( &cmd, 1);
		if (res < 0)
		{
			printf("Unable to send command %c\n", static_cast<char>(cmd));
			return -1;
		}
		unsigned int value = command.GetValue();
		unsigned int val = htonl(value);
		res = WriteToSocket( &val, 4);
		if (res < 0)
		{
			printf("Unable to send value %d for command %c\n", value, static_cast<char>(cmd));
		}
		break;
	}

	return 0;
}

SocketCommand SocketCommand::closeCommand(Command::CLOSE_RTL_TCP);

SocketCommand::SocketCommand(Command command, unsigned int value) :
		command(command),
		value(value)
{
}

Command SocketCommand::GetCommand()
{
	return command;
}

unsigned int SocketCommand::GetValue()
{
	return value;
}

void SocketCommand::SetCommand(Command command)
{
	this->command = command;
}

void SocketCommand::SetValue(unsigned int value)
{
	this->value = value;
}

SocketCommand SocketCommand::GetCloseCommand()
{
	return closeCommand;
}
