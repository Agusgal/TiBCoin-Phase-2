#include "Node.h"

using namespace std;

/*Constructor. Sets callbacks in server.*/
Node::Node(boost::asio::io_context& io_context, const std::string& ip, const unsigned int port, const unsigned int id)
	: ip(ip), server(nullptr), client(nullptr), clientState(ConnectionState::FREE), serverState(ConnectionState::FREE),
	port(port), id(id), connectedClientId(-1) 
{
	server = new Server(io_context,
		std::bind(&Node::getResponse, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Node::postResponse, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Node::errorResponse, this),
		port);
};

/*Desctructor. Frees resources.*/
Node::~Node() 
{
	if (client) 
	{
		delete client;
		client = nullptr;
	}
	if (server) 
	{
		delete server;
		server = nullptr;
	}
}


/*Adds new neighbor to 'neighbors' vector.*/
void Node::newNeighbor(const unsigned int id, const std::string& ip, const unsigned int port) 
{
	neighbors[id] = { ip, port };
}

/*Generates http response, according to validity of input.*/
const std::string Node::errorResponse() 
{
	json result;

	result["status"] = false;
	result["result"] = 1;

	return headerFormat(result.dump());
}

/*Returns daytime string. If plusThirty is true, it returns
current daytime + 30 seconds.*/
std::string Node::makeDaytimeString(bool plusThirty) 
{
	using namespace std::chrono;
	system_clock::time_point theTime = system_clock::now();

	if (plusThirty)
		theTime += seconds(30);

	time_t now = system_clock::to_time_t(theTime);

	return ctime(&now);
}


const std::string Node::headerFormat(const std::string& result) 
{
	return "HTTP/1.1 200 OK\r\nDate:" + makeDaytimeString(false) + "Location: " + "eda_coins" + "\r\nCache-Control: max-age=30\r\nExpires:" +
		makeDaytimeString(true) + "Content-Length:" + std::to_string(result.length()) +
		"\r\nContent-Type: " + "text/html" + "; charset=iso-8859-1\r\n\r\n" + result;
}


/*Getters*/
const unsigned int Node::getId() 
{ 
	return id; 
}

ConnectionState Node::getClientState(void) 
{
	if (clientState == ConnectionState::FINISHED) 
	{
		clientState = ConnectionState::FREE;
		return ConnectionState::FINISHED;
	}
	else
		return clientState;
}


ConnectionState Node::getServerState(void) 
{
	switch (serverState) 
	{
	case ConnectionState::OK:
		serverState = ConnectionState::FINISHED;
		return ConnectionState::OK;
	case ConnectionState::FAILED:
		serverState = ConnectionState::FINISHED;
		return ConnectionState::FAILED;
	case ConnectionState::FINISHED:
		serverState = ConnectionState::FREE;
		return ConnectionState::FINISHED;
	default:
		break;
	}
}


int Node::getClientPort(void) 
{
	int temp = connectedClientId;
	connectedClientId = -1;
	return temp;
}


void Node:: setConnectedClientID(const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	for (const auto& neighbor : neighbors) 
	{
		/*std::cout << nodeInfo.port() << std::endl;
		std::cout << neighbor.second.port + 1 << std::endl;
		std::cout << neighbor.second.ip << std::endl;
		std::cout << nodeInfo.address().to_string() << std::endl;*/
		if (neighbor.second.port + 1 == nodeInfo.port() && neighbor.second.ip == nodeInfo.address().to_string())
			connectedClientId = neighbor.first;
	}
}


map <unsigned int, Neighbour> Node::getNeighbours(void)
{
	return neighbors;
}