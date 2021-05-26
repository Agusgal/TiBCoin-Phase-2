#pragma once
#include "Blocks/Blockmicho.h"

using json = nlohmann::json;
using namespace std;

class BlockChain
{
public:
	BlockChain();
	~BlockChain();
	BlockChain(const string&);

	void loadBlockChain(const string& filename);
	const string getBlockInfo(int id, const BlockInfo&);
	const unsigned int getBlockAmount();
	const string reprTree(unsigned int id);

private:
	vector <BlockMicho> Blocks;
	json JSON;
};