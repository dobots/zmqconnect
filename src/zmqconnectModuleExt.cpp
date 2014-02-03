/**
 * @file zmqconnectModuleExt.cpp
 * @brief zmqconnectModule extension
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2013 Your Name <your@email>
 *
 * @author	Your Name
 * @date	Current date
 * @company	Your Company
 * @project	Specific Software Project
 */

#include <zmqconnectModuleExt.h>

#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 1000;

zmqconnectModuleExt::zmqconnectModuleExt() {
	cmdc_socket = NULL;
	success = false;
	ack_received = false;
}

zmqconnectModuleExt::~zmqconnectModuleExt() {
	std::cout << "Deallocate object and close socket to name server" << endl;
}

/**
 * This example is from the web, but doesn't seem to work.
 */
void zmqconnectModuleExt::Tick() {
	std::cout << '[' << getpid() << "] Tick " << lifetime << endl;
//	zmqconnect::Tick(); // we don't need to receive commands for the connect binary for now

	// send this command
	std::stringstream smsg; smsg.clear(); smsg.str("");
	smsg << connect_source << "->" << connect_target;
	std::string msg = smsg.str();
	int len = msg.size(); // include null-terminator
//	std::cout << "Size " << connect_source.size() << " plus 2 plus " << connect_target.size() << " = " << msg.size() << std::endl;
	char send [len+1];
	memcpy(send+1, (void*)msg.c_str(), msg.size());
	send[0] = 0x01; // prepend command, 0x01 means "CONNECT"
	//send[len+1] = '\0';
	std::cout << "Send message " << std::string(send+1, len) << std::endl;

	// get first the pid to construct the name of the control port
	pns_record record;
	record.name = "/resolve" + connect_source;
//	record.name = connect_source;
	Resolve(record);

	record.name = "/resolve/" + record.pid + "/control";
	Resolve(record);

	// connect to this socket
	std::stringstream ss; ss.clear(); ss.str("");
	ss << "tcp://" << record.host << ":" << record.port;
	std::string sock = ss.str();
	cmdc_socket = new zmq::socket_t(*context, ZMQ_REQ);
	std::cout << "Connect to " << sock << std::endl;
	cmdc_socket->connect(sock.c_str());

	success = true; // previously cycle of send-receive initialized to success=true
	bool blocking = true;
	SendRequest(cmdc_socket, success, blocking, std::string(send, len+1));
	if (success) {
		std::cout << "Send successfully" << std::endl;
		ack_received = true;
	    ReceiveAck(cmdc_socket, ack_received, true);
	} else {
		std::cerr << "Error: Something went wrong..." << std::endl;
		sleep(1);
//		return false;
		return;
	}

	sleep(1);
//	return true;
	return;
}

bool zmqconnectModuleExt::Stop() {
	// we can quit when the connection is established
	if (ack_received) {
		std::cout << "Ack received, we quit" << endl;
		sleep(1);
		delete cmdc_socket;
		return true;
	}
	if (--lifetime) {
		delete cmdc_socket;
		return false;
	}
	return true;
}

