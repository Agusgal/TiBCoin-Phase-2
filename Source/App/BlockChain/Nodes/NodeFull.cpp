#include "NodeFull.h"

#include <typeinfo>
#include "../../Client/AllClients.h"


/*Text constants in requests.*/
/************************************************/
namespace {
	const char* BLOCKPOST = "send_block";
	const char* TRANSPOST = "send_tx";
	const char* FILTERPOST = "send_filter";
	const char* BLOCKSGET = "get_blocks";
	const char* HEADERGET = "get_block_header";
}
/************************************************/

const json error = { "error" };

/*Constructor. Uses Node constructor.*/
NodeFull::NodeFull(boost::asio::io_context& io_context, const std::string& ip,
	const unsigned int port, const unsigned int identifier)
	: Node(io_context, ip, port, identifier), blockChain("C:\\Users\\Agus\\source\\repos\\TiBCoin Phase 2\\Resources\\sample0.json")
{
}


/*GET performer for GET blocks request. */
void NodeFull::getBlocks(const unsigned int id, const std::string& blockID, const unsigned int count) 
{
	/*If node is free...*/
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor and count isn't null...*/
		if (neighbors.find(id) != neighbors.end() && count) 
		{
			/*Sets new GetBlockClient.*/
			client = new GetBlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockID, count);

			/*Toggles state.*/
			clientState = ConnectionState::PERFORMING;
		}
	}
}


/*POST connection for blocks.*/
void NodeFull::postBlock(const unsigned int id, const std::string& blockID) 
{
	/*If node is in client mode*/
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor and count isn't null...*/
		if (neighbors.find(id) != neighbors.end()) 
		{
			/*Sets new BlockClient for POST request.*/
			client = new BlockClient(neighbors[id].ip, port + 1, neighbors[id].port, blockChain.getBlock(0)/*getBlock(blockID)*/);
			clientState = ConnectionState::PERFORMING;
		}
	}
}

/*POST merkleblock connection.*/
void NodeFull::postMerkleBlock(const unsigned int id, const std::string& blockID, const std::string& transID) 
{
	if (clientState == ConnectionState::FREE && !client) 
	{
		/*If id is a neighbor...*/
		if (neighbors.find(id) != neighbors.end()) 
		{
			auto temp = getMerkleBlock(blockID, transID);
			client = new MerkleClient(neighbors[id].ip, port + 1, neighbors[id].port, temp);
			clientState = ConnectionState::PERFORMING;
		}
	}
}

/*Gets block from blockChain by ID.*/
const json& NodeFull::getBlock(const std::string& blockID) 
{
	/*Loops through every block in blockchain.*/
	for (unsigned int i = 0; i < blockChain.getBlockQuantity(); i++)
	{
		/*If it finds the correct ID, it returns that node.*/
		if (blockChain.getBlockInfo(i, BlockInfo::BLOCKID) == blockID)
		{
			return blockChain.getBlock(i);
		}
	}

	return error;
}

void NodeFull::transaction(const unsigned int id, const std::string& wallet, const unsigned int amount) 
{
	if (clientState == ConnectionState::FREE && !client) {
		/*If id is a neighbor...*/
		if (neighbors.find(id) != neighbors.end()) {
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

const json NodeFull::getMerkleBlock(const std::string& blockID, const std::string& transID) 
{
	//int k = blockChain.getBlockIndex(blockID);
	unsigned int k = 0;
	auto tree = blockChain.getTree(k);

	/*for (unsigned int i = 0; i < tree.size(); i++) {
		if (tree[i] == transID)
			k = i;
	}*/
	json result;
	int size = static_cast<int>(log2(tree.size() + 1));
	std::vector<std::string> merklePath;
	while (k < (tree.size() - 1)) 
	{
		if (k % 2) merklePath.push_back(tree[--k]);
		else merklePath.push_back(tree[k + 1]);

		k = static_cast<unsigned int>(k / 2 + pow(2, size - 1));
	}

	result["blockid"] = blockID;
	result["tx"] = blockChain.getBlock(0)["tx"];
	result["txPos"] = 0;
	result["merklePath"] = merklePath;

	return result;
}

/*Destructor. Uses Node destructor.*/
NodeFull::~NodeFull() {}


/*GET callback for server.*/
const std::string NodeFull::getResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	json result;
	setConnectedClientID(nodeInfo);
	result["status"] = true;
	int block;
	serverState = ConnectionState::FAILED;
	/*Checks for correct data input (one of the strings must be in the request).*/
	if ((block = request.find(BLOCKSGET)) || request.find(HEADERGET)) 
	{
		/*Checks for correct syntax within data input.*/

		int pos_id = request.find("block_id=");
		int pos_count = request.find("count=");

		if (pos_id != std::string::npos && pos_count != std::string::npos) 
		{
			json response;

			/*Parses input for id.*/
			std::string id = request.substr(pos_id + 9, request.find_last_of("&") - pos_id - 9);

			/*Parses input for count.*/
			int count = std::stoi(request.substr(pos_count + 6, request.find("HTTP") - pos_count - 6));

			/*Sets block's position in blockchain.*/
			int abs = blockChain.getBlockIndex(id);

			/*Goes to next block.*/
			if (!(++abs)) 
			{
				result["status"] = false;
				result["result"] = 2;
			}

			else 
			{
				/*Loops through blockchain ('count' blocks or until end of blockchain).*/
				while (abs < blockChain.getBlockQuantity() && count) 
				{
					/*If it's a POST block...*/
					if (block != std::string::npos) 
					{
						/*Attaches full block to response.*/
						response.push_back(blockChain.getBlock(abs));
					}
					/*Otherwise...*/
					else 
					{
						/*Attaches header to response.*/
						response.push_back(blockChain.getHeader(abs));
					}
					count--;
					serverState = ConnectionState::OK;
				}

				/*Appends response to result.*/
				result["result"] = response;
			}
		}
		/*Format error.*/
		else 
		{
			result["status"] = false;
			result["result"] = 1;
		}
	}
	/*Content error.*/
	else
	{
		result["status"] = false;
		result["result"] = 2;
	}

	return headerFormat(result.dump());
}

/*POST callback for server.*/
const std::string NodeFull::postResponse(const std::string& request, const boost::asio::ip::tcp::endpoint& nodeInfo) 
{
	setConnectedClientID(nodeInfo);

	serverState = ConnectionState::FAILED;

	json result;
	result["status"] = true;
	result["result"] = NULL;

	/*If it's POST block...*/
	if (request.find(BLOCKPOST) != std::string::npos) 
	{
		/*Adds block to blockchain.*/
		int content = request.find/*_last_of*/("Content-Type");
		int data = request.find/*_last_of*/("Data=");
		if (content == std::string::npos || data == std::string::npos)
			result["status"] = false;
		else 
		{
			blockChain.addBlock(json::parse(request.substr(data + 5, content - data - 5)));
			serverState = ConnectionState::OK;
		}
	}

	/*If it's a transaction...*/
	else if (request.find(TRANSPOST) != std::string::npos) 
	{
		serverState = ConnectionState::OK;
	}

	/*If it's a filter...*/
	else if (request.find(FILTERPOST) != std::string::npos) 
	{
		serverState = ConnectionState::OK;
	}

	return headerFormat(result.dump());
}


/*Performs client mode. */
void NodeFull::perform() 
{
	/*If node is in client mode...*/
	if (client) 
	{
		/*If request has ended...*/
		if (!client->performRequest())
		{
			if (typeid(*client) == typeid(GetBlockClient)) 
			{
				const json& temp = client->getAnswer();
				if (temp["status"]) 
				{
					for (const auto& block : temp["result"]) 
					{
						blockChain.addBlock(block);
					}
				}
			}
			/*Deletes client and set pointer to null.*/
			delete client;
			client = nullptr;
			clientState = ConnectionState::FINISHED;
		}
	}
}

std::vector <Actions> NodeFull::getActions(void)
{
	std::vector<Actions> actionvector;

	actionvector.push_back(Actions(ActionType::SR, "Post Block"));
	actionvector.push_back(Actions(ActionType::SR, "Post Transaction"));
	actionvector.push_back(Actions(ActionType::S, "Post merkleblock"));
	actionvector.push_back(Actions(ActionType::R, "Post Filter"));
	actionvector.push_back(Actions(ActionType::R, "Get Block headers"));
	actionvector.push_back(Actions(ActionType::SR, "Get Blocks"));

	return actionvector;
}