#pragma once
#include <iostream>
#include <string>
#include "../BlockChain/Blocks/json.hpp"
#include <curl\curl.h>
#include<vector>
//#include "Structs.h"

#define NOTFOUND -1

using namespace std;
using json = nlohmann::json;

typedef enum { GET, POST } method_n;
typedef enum { ERROR_FREE2, CURLINIT_ERROR, CURLPERFORM_ERROR, INVALID_DATA } errorCode_n;
//size_t myCallback(char* contents, size_t size, size_t nmemb, void* userp);


//typedef struct {
//	int TEMP_PUERTO;
//	int TEMP_ID;
//	int numberofConnections = 0;
//	vector<unsigned int> connections; //Vecinos dentro de la red con los que ya se conectó
//	bool checked = false;	//Indica si fue o no visitado
//
//}NodoSubconjunto;


class Client
{
public:
	//Client(string IP_, int port_);
	Client(const string &ip, const unsigned int ownPort, const unsigned int outPort);
	//Client();
	virtual ~Client(void);

	virtual bool performRequest(void);
	virtual const json& getAnswer(void);

	/*void useGETmethod(string path_);
	void usePOSTmethod(string path_, const json data);

	void setIP(string IP_);
	void setPort(unsigned int port_);
	void setURL(string URL_);
	void setHost(string host_);

	void setMethod(method_n method_);

	void setRunningStatus(int RunningStatus);
	void setErrorCode(errorCode_n errorCode_);
	void setErrorMsg(string errorMsg_);
	string getIP(void);
	unsigned int getPort(void);
	string getURL(void);
	string getHost(void);
	method_n getMethod(void);
	int getRunningStatus(void);
	string getReply(void);
	errorCode_n getErrorCode(void);
	string getErrorMsg(void);*/

protected:
	virtual void configureClient(void) = 0;
	CURL* easyHandler, * multiHandler;

	string ip, rawReply, url;
	json data;
	json answer;

	unsigned int ownPort;
	unsigned int outPort;
	int stillRunning;

	static size_t myCallback(char* contents, size_t size, size_t nmemb, void* userData);
	
	//CURLcode easyError;
	//CURLMcode multiError;
	//method_n method;
	//string reply, host, url;
	//string IP; //IP del vecino
	//unsigned int ID;
	//unsigned int port; //Puerto del vecino
	//unsigned int own_port; //Puerto propio
	//int stillRunning;
	/*errorCode_n errorCode;
	string errorMsg;
	json data_;
	string myjson;
	json parsedReply;*/
};
