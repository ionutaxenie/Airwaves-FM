/*
 * SocketManager.h
 *
 *  Created on: Dec 1, 2014
 *      Author: ionut
 */

#ifndef SOCKETMANAGER_H_
#define SOCKETMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Enum that has all possible command for rtl_tcp
enum class Command
{
	SET_FREQUENCY = (char)0x01,
	SET_SAMPLING_RATE = (char)0x02,
	SET_TUNER_GAIN_MODE = (char)0x03,
	SET_TUNER_GAIN = (char)0x04,
	SET_FREQUENCY_CORRECTION = (char)0x05,
	SET_IF_STAGE_GAIN = (char)0x06,
	SET_TEST_MODE = (char)0x07,
	SET_AGC_MODE = (char)0x08,
	SET_DIRECT_SAMPLING = (char)0x09,
	SET_OFFSET_TUNING = (char)0x0a,
	SET_RTL_XTAL_FREQUENCY = (char)0x0b,
	SET_TUNER_XTAL_FREQUENCY = (char)0x0c,
	SET_TUNER_GAIN_BY_INDEX = (char)0x0d,
	CLOSE_RTL_TCP = (char)0x7e,
	SET_GAIN_BY_PERC = (char)0x7f
};

class SocketCommand
{
public:
	SocketCommand(Command command = Command::SET_AGC_MODE, unsigned int value = 0);
	Command GetCommand();
	unsigned int GetValue();
	void SetCommand(Command command);
	void SetValue(unsigned int value);
	static SocketCommand GetCloseCommand();
private:
	Command command;
	unsigned int value;
	static SocketCommand closeCommand;
};

class SocketManager {
public:
	// Constructors and destructor
	SocketManager(char * ip, int port);
	SocketManager();
	virtual ~SocketManager();

	// Various method used to interface with rtl_tcp
	int ConnectToSocket();
	int ReadFromSocket(unsigned char * message, int numberOfBytes);
	int DisconnectFromSocket();
	int ConnectToDevice(unsigned int frequency, char * gain, unsigned int sampleRate, bool supressOutput);
	int SendCommand(SocketCommand command);
	void DisconnectFromDevice();
	void Init(char * ip, int port);

private:

	// Private members that hold information for the socket
	int sockfd;
	int portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char ipAddress[30];

	int WriteToSocket(void * message, unsigned int size);
};

#endif /* SOCKETMANAGER_H_ */
