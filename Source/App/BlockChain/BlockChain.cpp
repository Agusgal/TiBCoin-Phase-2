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

	/*Attempts to open file.*/
	fstream jsonFile(filename, ios::in);

	if (!jsonFile.is_open()) {
		jsonFile.close();
		throw std::exception("Failed to open file.");
	}

	/*Parses file input.*/
	JSON = json::parse(jsonFile);

	jsonFile.close();

	/*For every block in the blockChain...*/
	for (auto& j : JSON)
	{
		Blocks.push_back(BlockMicho(j));
	}
}

const string BlockChain::getBlockInfo(int id, const BlockInfo& member) 
{
	return Blocks[id].getData(member);
}

const unsigned int BlockChain::getBlockAmount() 
{ 
	return Blocks.size(); 
}

const string BlockChain::reprTree(unsigned int id) 
{
	return Blocks[id].printTree();
};

