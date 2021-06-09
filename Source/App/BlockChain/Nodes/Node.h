#pragma once
#include "../../Client/Client.h"
#include "../../Server/Server.h"
#include "../../Actions/Actions.h"

#include <chrono>
#include <map>
#include <vector>

using namespace std;


const enum class ConnectionState : unsigned int {
	FREE,
	PERFORMING,
	OK,
	FAILED,
	FINISHED
};

const enum class ConnectionType : unsigned int {
	GETBLOCK,
	GETHEADER,
	POSTBLOCK,
	POSTMERKLE,
	POSTTRANS,
	POSTFILTER
};


typedef enum { FREE, CLIENT, SERVER } state_t;
typedef enum { ERROR_FREE, CLIENT_ERROR, SERVER_ERROR, BUSY_NODE, NOT_NEIGHBOUR } errorType_t;

struct Neighbour {
	string ip;
	unsigned int port;
};

//typedef struct registronodo_t
//{
//	string ip;
//	int puerto;
//	int id;
//	unsigned int type;
//	map<unsigned int, neighbour> nodosvecinos;
//	registronodo_t(int id_, std::string ip_, int puerto_, int type_) :id(id_), ip(ip_), puerto(puerto_), type(type_) {}
//	registronodo_t() {}

//}registronodo_t;

//typedef struct
//{
//	registronodo_t nodoemisor;
//	map<unsigned int, neighbour> nodosvecinospt;
//	string mensaje;
//	vector<string> vecinos;	
//	int selectedvecino;
//	vector<neighbour> vecinosvector;

//	neighbour vecino;
//	unsigned int mensaje;		
//	unsigned int coins_g;
//	string publickey_g;
//	string block_id;
//	int cant_blocks;
//	string ntx_;


//} brothersmsg_t;


class Node {
public:
	Node(boost::asio::io_context& io_context, const std::string& ip, const unsigned int port,
		const unsigned int identifier);
	virtual ~Node();

	virtual void newNeighbor(const unsigned int id, const std::string& ip, const unsigned int port);

	/*virtual methods*/
	virtual void perform(void) = 0;
	virtual const unsigned int getId(void);

	virtual void transaction(const unsigned int, const string& wallet, const unsigned int amount) = 0;
	virtual void postBlock(const unsigned int, const string &blockId) = 0;
	virtual void postMerkleBlock(const unsigned int, const string& blockId, const string& transId) = 0;

	virtual void postFilter(const unsigned int, const string& key) = 0;
	
	virtual void getBlocks(const unsigned int, const string& blockId, const unsigned int count) = 0;
	virtual void getBlockHeaders(const unsigned int, const string& blockId, const unsigned int count) = 0;

	virtual std::vector <Actions> getActions(void) = 0;

	virtual ConnectionState getClientState(void);
	virtual ConnectionState getServerState(void);

	virtual map <unsigned int, Neighbour> getNeighbours(void);

	virtual int getClientPort(void);

	/*string getIP(void);
	unsigned int getPort(void);
	unsigned int getID(void);
	state_t getState(void);
	errorType_t getErrorType(void);
	string getErrorMsg(void);
	map <unsigned int, Neighbour> getNeighbours(void);
	vector <string> getFilters(void);*/


	/*string parseResponse(string message);
	void setIP(string IP_);
	void setPort(unsigned int port_);
	void setID(unsigned int ID_);
	void setState(state_t state_);
	void setErrorType(errorType_t errorType_);
	void setErrorMsg(string errorMsg_);*/


protected:
	virtual std::string makeDaytimeString(bool);

	virtual const string getResponse(const string&, const boost::asio::ip::tcp::endpoint&) = 0;
	virtual const string postResponse(const string&, const boost::asio::ip::tcp::endpoint&) = 0;
	virtual const string errorResponse(void);
	

	virtual const string headerFormat(const string&);
	virtual void setConnectedClientID(const boost::asio::ip::tcp::endpoint&);


	string ip;
	string publicKey;
	unsigned int port;
	unsigned int id;
	int sentMessage;
	int receivedMessage;
	state_t state;


	ConnectionState clientState;
	ConnectionState serverState;
	int connectedClientId;
	Client* client;
	Server* server;
	
	errorType_t errorType;
	string errorMsg;

	map <unsigned int, Neighbour> neighbors;
	vector<string> FilterArray;
};
