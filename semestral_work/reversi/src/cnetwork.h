#pragma once

#include <vector>
#include <netinet/in.h>

#include "types.h"

using namespace std;

struct sockaddr;
struct sockaddr_storage;

/**
* The networking bridge wrapper between two remote game instances.
* Build on the base of asynchronous methods, class needs to be repeatedly checked from the main loop
* In the case of a successful read, the callback is invoked as a response.
*
* @author Martin Holoubek
*/
class CNetwork
{
public:
	~CNetwork();
	bool AwaitConnection();
	bool Connect(string ip);
	static bool ParseIP(string& ip);
	static vector<string> GetAddresses();
public:
public:
	static void* GetAddress(sockaddr* sa);
	bool WaitForClient();
	bool Send(string data) const;
	NetworkStatus TryReceive(string& message);
private:
	int mListener;
	int mClient;
	fd_set mSockets;
	sockaddr_storage mIPClient;
	char mIPText[INET6_ADDRSTRLEN];
	char mBuffer[BUF_LEN];
};
