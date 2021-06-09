#include "NodeSPV.h"
#include "../../Client/AllClients.h"
#include <typeinfo>

namespace {
	const char* MERKLEPOST = "send_merkle_block";
}

/*SPV_Node constructor. Uses Node constructor.*/
NodeSPV::NodeSPV(boost::asio::io_context& io_context, const std::string& ip,
	const unsigned int port, const unsigned int identifier) : Node(io_context, ip, port, identifier)
{
}

/*GET callback for server.*/
const std::string NodeSPV::getResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	setConnectedClientID(nodeInfo);
	serverState = ConnectionState::FAILED;

	json result;
	result["status"] = false;

	/*Content error.*/
	result["result"] = 2;
	return headerFormat(result.dump());
}

/*POST callback for server.*/
const std::string NodeSPV::postResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo)
{
	setConnectedClientID(nodeInfo);
	serverState = ConnectionState::FAILED;
	json result;
	result["status"] = true;
	result["result"] = NULL;

	/*Checks if it's a POST for merkleblock.*/
	if (request.find(MERKLEPOST) != std::string::npos) 
	{
		int content = request.find/*_last_of*/("Content-Type");
		int data = request.find/*_last_of*/("Data=");
		if (content == std::string::npos || data == std::string::npos)
			result["status"] = false;
		else 
		{
			serverState = ConnectionState::OK;
		}
	}
	else 
	{
		result["status"] = false;
		result["result"] = 2;
	}

	return headerFormat(result.dump());
}

/*Destructor. Uses Node destructor.*/
NodeSPV::~NodeSPV() {}

void NodeSPV::postFilter(const unsigned int id, const std::string& key) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor...*/
		if (neighbors.find(id) != neighbors.end()) 
		{
			json tempData;

			tempData["key"] = key;

			client = new FilterClient(neighbors[id].ip, port + 1, neighbors[id].port, tempData);
			clientState = ConnectionState::PERFORMING;
		}
	}
};
void NodeSPV::transaction(const unsigned int id, const std::string& wallet, const unsigned int amount) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor...*/
		if (neighbors.find(id) != neighbors.end()) 
		{
			json tempData;

			tempData["nTxin"] = 0;
			tempData["nTxout"] = 1;
			tempData["txid"] = "ABCDE123";
			tempData["vin"] = json();

			json vout;
			vout["amount"] = amount;
			vout["publicid"] = wallet;

			tempData["vout"] = vout;

			client = new TransactionClient(neighbors[id].ip, port + 1, neighbors[id].port, tempData);
			clientState = ConnectionState::PERFORMING;
		}
	}
}
void NodeSPV::getBlockHeaders(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	/*If node is free...*/
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor and count isn't null...*/
		if (neighbors.find(id) != neighbors.end() && count) 
		{
			/*Sets new GETBlockClient.*/
			client = new GetHeaderClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count);

			/*Toggles state.*/
			clientState = ConnectionState::PERFORMING;
		}
	}
};

/*Performs client mode. */
void NodeSPV::perform()
{
	/*If node is in client mode...*/
	if (client) 
	{
		/*If request has ended...*/
		if (!client->performRequest()) 
		{
			if (typeid(*client) == typeid(GetHeaderClient)) 
			{
				const json& temp = client->getAnswer();
				if (temp["status"]) 
				{
					for (const auto& header : temp["result"])
						headers.push_back(header);
				}
			}
			/*Deletes client and set pointer to null.*/
			delete client;
			client = nullptr;
			clientState = ConnectionState::FINISHED;
		}
	}
}


std::vector<Actions> NodeSPV::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::S, "Post Transaction"));
	actionvector.push_back(Actions(ActionType::R, "Post merkleblock"));
	actionvector.push_back(Actions(ActionType::S, "Post Filter"));
	actionvector.push_back(Actions(ActionType::S, "Get Block headers"));

	return actionvector;
}