#include <boost/asio.hpp>
#include <string>
#include <functional>

#include <iostream>
#include <boost\bind.hpp>
#include <chrono>
#include <fstream>
#include "../BlockChain/Blocks/json.hpp"
#include <boost/lexical_cast.hpp>
#include <boost\asio\ip\address.hpp>
#include <queue>

using namespace std;
//using boost::asio::ip::tcp;
//using json = nlohmann::json;

#define MAXSIZE 999999

namespace {
	using Response = function<const string(const string&, const boost::asio::ip::tcp::endpoint&)>;
	using errorResp = function<const string(void)>;
}

const enum class ConnectionTypes
{
	NONE = 0,
	POST,
	GET
};

struct Connection {
	Connection(boost::asio::io_context& io_context) : socket(io_context) {}
	boost::asio::ip::tcp::socket socket;
	char reader[MAXSIZE];
	std::string response;
	std::list<Connection>::iterator pos;
};

class Server
{
public:
	Server(boost::asio::io_context&, const Response&, const Response&, const errorResp&, unsigned int);
	virtual ~Server();

protected:
	void newConnector(void);

	/*const enum class ConnectionTypes 
	{
		NONE = 0,
		POST,
		GET
	};*/
	/*struct Connection {
		Connection(boost::asio::io_context& io_context) : socket(io_context) {}
		boost::asio::ip::tcp::socket socket;
		char reader[MAXSIZE];
		std::string response;
		std::list<Connection>::iterator pos;
	};*/

	/*Connection methods.*/
	/*******************************************/
	void asyncConnection(Connection&);
	void closeConnection(Connection&);

	void answer(Connection&, const std::string&);
	Response getResponse;
	Response postResponse;
	errorResp errorResponse;
	/*******************************************/

	/*Callbacks and callback-related.*/
	/*********************************************************************************/
	void connectionCallback(Connection&, const boost::system::error_code& error);
	void messageCallback(Connection&, const boost::system::error_code& error, size_t bytes_sent);
	void inputValidation(Connection&, const boost::system::error_code& error, size_t bytes);
	/*********************************************************************************/

	/*Boost::asio data members.*/
	/****************************************/
	boost::asio::io_context& io_context;
	boost::asio::ip::tcp::acceptor acceptor;
	std::list<Connection> sockets;
	/****************************************/

	/*Connection data members.*/
	/*********************************************/
	size_t size;
	std::string host;
	ConnectionTypes state;
	unsigned int port;
	/*********************************************/
};






//typedef function<json(string)> scallback;


//enum class CONNECTIONS { GET_BLOCK_HEADER, GETBLOCK, POST, NO };

//class Sockets {
//public:
//	Sockets(boost::asio::io_context& io_context) : socket(io_context) {}
//	boost::asio::ip::tcp::socket socket;
//	CONNECTIONS type;
//};


//class Server{
//public:
//	Server(boost::asio::io_context& io_context_, string IP, scallback p, unsigned int port_);
//	void waitForConnection(void);
//	~Server();
//
//private:
//	void closeConnection(void);
//	void parse(const boost::system::error_code& error, size_t bytes_sent);
//	void answer();
//	void generateTextResponse(void);
//	void response_sent_cb(const boost::system::error_code& error, size_t bytes_sent);
//	void connectionCallback(const boost::system::error_code& error);
//	void messageCallback(const boost::system::error_code& error, size_t bytes_sent);
//	scallback call;
//	string makeDaytimeString(int secs);
//
//	boost::asio::io_context& io_context;
//	boost::asio::ip::tcp::acceptor acceptor;
//	boost::asio::ip::tcp::socket socket;
//
//	string response;
//	string nodeIP;
//	list< char[MAXSIZE]> ClientInput;
//	string blockId;
//	unsigned int count;
//	unsigned int state;
//
//	json result;
//
//	int port;
//	size_t FileLen;
//	string msg;
//	string RealTime;
//	string ServerOutput;
//	string ClientInputStr;
//
//	list<boost::asio::ip::tcp::socket*> socketQ;
//
//};
