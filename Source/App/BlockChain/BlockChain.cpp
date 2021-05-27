#include "BlockChain.h"
#include <fstream>

using namespace std;

BlockChain::BlockChain()
{

}

BlockChain::~BlockChain()
{

}

BlockChain::BlockChain(const string& filename)
{
	loadBlockChain(filename);
}

void BlockChain::loadBlockChain(const string& filename)
{
	Blocks.clear();

	fstream jsonFile(filename, ios::in);

	if (!jsonFile.is_open()) 
	{
		jsonFile.close();
		throw std::exception("Failed to open file!");
	}

	jsonObj = json::parse(jsonFile);

	jsonFile.close();

	/*We cerate a block for every json object.*/
	for (auto& j : jsonObj)
	{
		Blocks.push_back(BlockMicho(j));
	}
}

const string BlockChain::getBlockInfo(int id, const BlockInfo& member) 
{
	return Blocks[id].getData(member);
}

const unsigned int BlockChain::getBlockQuantity() 
{ 
	return Blocks.size(); 
}

const string BlockChain::drawTree(unsigned int id) 
{
	return Blocks[id].printTree();
};

