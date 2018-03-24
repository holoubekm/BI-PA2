#include <sstream>
#include <cstring>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "cnetwork.h"

/**
* Destructor
*/
CNetwork::~CNetwork()
{
    close(mClient);
    FD_CLR(mClient, &mSockets);
    close(mListener);
    FD_CLR(mListener, &mSockets);
}

/**
* Static method to check the correctness of the ipv4 string
* @param ip string to be parsed
* @return the parsing result
*/
bool CNetwork::ParseIP(string& ip)
{
	istringstream buffer(ip);
	int a = -1, b = -1, c = -1, d = -1;
	char dot;
	buffer >> a >> dot >> b >> dot >> c >> dot >> d;

	if(a == -1 || b == -1 || c == -1 || d == -1)
	{
		return false;
	}
	return true;
}

/**
* Enumerates network adapters and returns adresses they're bound with
* @return vector containing the string values
*/
vector<string> CNetwork::GetAddresses()
{
    vector<string> addresses;
    ifaddrs *interfaces = NULL, *cur = NULL;
    char buffer[INET6_ADDRSTRLEN];
  
    if(getifaddrs(&interfaces) == 0)
    {    
        for(cur = interfaces; cur != NULL; cur = cur->ifa_next)
        {
            if(cur->ifa_addr->sa_family == AF_INET)
            {
                inet_ntop(cur->ifa_addr->sa_family, GetAddress(cur->ifa_addr), buffer, sizeof(buffer));
                addresses.push_back(buffer);
            }
        }
    }
    return addresses;
}


/**
* Main server method, starts listening for the clien, blocking method
* @return denotes whether waiting was succesful
*/
bool CNetwork::AwaitConnection()
{
	int retvalue = 0;
	int reuse = 1;

	FD_ZERO(&mSockets);
	addrinfo sockets, *info, *cur;

	memset(&sockets, 0, sizeof sockets);
	sockets.ai_family = AF_UNSPEC;
	sockets.ai_socktype = SOCK_STREAM;
	sockets.ai_flags = AI_PASSIVE;
	if ((retvalue = getaddrinfo(NULL, PORT, &sockets, &info)) != 0) 
	{
		//cerr << "selectserver: " << gai_strerror(retvalue) << endl;
		return false;
	}

	for(cur = info; cur != NULL; cur = cur->ai_next) 
	{
    	mListener = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
		if (mListener < 0)
			continue;
		
		setsockopt(mListener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
		if (bind(mListener, cur->ai_addr, cur->ai_addrlen) < 0) 
		{
			close(mListener);
			continue;
		}
		break;
	}

	int flag = fcntl(mListener, F_GETFL, 0);
    if (fcntl(mListener, F_SETFL, flag | O_NONBLOCK) == -1)
    {
		//cerr << "a problem with socket mode setup" << endl;
    	return false;
    }

    if (cur == NULL) 
    {
		//cerr << "failed to bind the socket" << endl;
		return false;
	}
	freeaddrinfo(info);

    if (listen(mListener, 1) == -1) 
    {
        //perror("failed to setup listener socket");
    	return false;
    }
    FD_SET(mListener, &mSockets);

    if (select(((int)mListener) + 1, &mSockets, NULL, NULL, NULL) == -1) 
    {
        //perror("failed to select the right socket");
    	return false;
    }
	
	if(!WaitForClient())
	{
		//cerr << "Error encountered while waiting for client socket" << endl;
		return false;
	}
	FD_CLR(mListener, &mSockets);
	close(mListener);
	return true;
}


/**
* Main client method, connects to the server
* @param ip is the ipv4 address of the server
* @return points out whether the connection was succesful
*/
bool CNetwork::Connect(string ip)
{
	int retvalue = 0;

	FD_ZERO(&mSockets);
	addrinfo sockets, *info, *cur;

    memset(&sockets, 0, sizeof sockets);
    sockets.ai_family = AF_UNSPEC;
    sockets.ai_socktype = SOCK_STREAM;

    if ((retvalue = getaddrinfo(ip.c_str(), PORT, &sockets, &info)) != 0) {
        //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retvalue));
        return false;
    }

    for(cur = info; cur != NULL; cur = cur->ai_next) 
    {
        if ((mClient = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol)) == -1) {
            //perror("client: socket");
            continue;
        }

        if (connect(mClient, cur->ai_addr, cur->ai_addrlen) == -1) 
        {
            close(mClient);
            //perror("client: connect");
            continue;
        }

        break;
    }

    FD_SET(mClient, &mSockets);

    if (cur == NULL) 
    {
        //fprintf(stderr, "client: failed to connect\n");
        return false;
    }

    inet_ntop(cur->ai_family, GetAddress((sockaddr*)cur->ai_addr), mIPText, INET6_ADDRSTRLEN);
    freeaddrinfo(info); 

    int oldFlag = fcntl(mClient, F_GETFL, 0);
    if (fcntl(mClient, F_SETFL, oldFlag | O_NONBLOCK) == -1)
    {
      //cerr <<"Problém s nastavením reľimu soketu" << endl;
      return false;
    }
    return true;
}


/**
* Helper method to get the right ip address
* @param sa is the pointer to sockaddr struct
* @return pointer to the data
*/
void* CNetwork::GetAddress(sockaddr* sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
* Server's helper method to wait for a connection, blocking method
* @return the connection result
*/
bool CNetwork::WaitForClient()
{
	if (FD_ISSET(mListener, &mSockets)) 
	{
		socklen_t addrlen = sizeof(mIPClient);
		mClient = accept(mListener, (sockaddr*)&mIPClient, &addrlen);

		if (mClient == -1) 
		{
			//perror("accept");
			return false;
		} 
		else 
		{
			int flag = fcntl(mClient, F_GETFL, 0);
		    if (fcntl(mClient, F_SETFL, flag | O_NONBLOCK) == -1)
		    {
		    	//cerr << "an error encountered while setting up socket mode" << endl;
		      	return false;
		    }

            FD_SET(mClient, &mSockets);
            inet_ntop(mIPClient.ss_family, GetAddress((sockaddr*)&mIPClient), mIPText, INET6_ADDRSTRLEN);
       		return true;
        }
    }
    return false;
}


/**
* Method to send data to the remote client/server
* @param data string to be sent
* @return determines success of the method
*/
bool CNetwork::Send(string data) const
{
	size_t length = data.size();
	if(length > BUF_LEN)
	{
		//cerr << "overflow -> data length exceeded the buffer length" << endl;
		return false;
	}

	while(true)
	{
		if (FD_ISSET(mClient, &mSockets)) 
        {
            if (send(mClient, data.c_str(), length, 0) == -1) 
            {
            	//cerr << "desired data couln't be sent due to an unspecified networking problem" << endl;
                return false;
            }
            return true;
        }
	}
	return true;
}


/**
* Method check the stream for an input
* @param[out] message if method was succesful, data is stored here
* @return the networking status
*/
NetworkStatus CNetwork::TryReceive(string& message)
{
	int received = 0;
	if (FD_ISSET(mClient, &mSockets)) 
    {
        if ((received = recv(mClient, mBuffer, BUF_LEN, 0)) <= 0) 
        {
        	if(received != -1)
        	{
                if (received == 0)
                {
                    //cout << "socket hung up" << endl;
                    //deb << "Nastala chybka 1" << endl;
                    return mNetworkError;
                } 
                else 
                {
                    //perror("recv");
                    //deb << "Nastala chybka 2" << endl;
                    return mNetworkError;
                }
                close(mClient);
                FD_CLR(mClient, &mSockets);
            }
        } 
        else 
        {
        	message = string(mBuffer, received);
        	return mNetworkTrue;
        }
    }
    return mNetworkFalse;
}
