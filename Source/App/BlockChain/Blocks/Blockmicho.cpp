#include "Blockmicho.h"
#include <string>


using namespace std;

//CONSTRUCTOR Y DESTRUCTOR
BlockMicho::BlockMicho()
{

}

BlockMicho::~BlockMicho()
{

}

BlockMicho::BlockMicho(const json& block)
{
	this->jsonData = block;
	this->validated = false;
	
	ntx = block["nTx"].get<unsigned int>();
	height = block["height"].get<unsigned int>();
	nonce = block["nonce"].get<unsigned int>();
	blockid = block["blockid"].get<string>();
	previousBlockId = block["previousblockid"].get<string>();
	merkleRoot = block["merkleroot"].get<string>();

	auto transactions = block["tx"];

	Transaction tempTransaction;

	for (auto& tx_ : transactions) {

		auto nTxin_ = tx_["nTxin"].get<unsigned int>();
		tempTransaction.setNtxin(nTxin_);

		auto nTxout_ = tx_["nTxout"].get<unsigned int>();
		tempTransaction.setNtxout(nTxout_);

		auto txid_ = tx_["txid"].get<string>();
		tempTransaction.setTxid(txid_);

		auto vin_ = tx_["vin"];

		for (auto& vin_json : vin_)
		{
			Vin tempvin;

			auto blockid_ = vin_json["blockid"].get<string>();
			tempvin.blockid = blockid_;

			auto intxid_ = vin_json["txid"].get<string>();
			tempvin.intxid = intxid_;

			auto signature_ = vin_json["signature"].get<string>();
			tempvin.signature = signature_;

			auto outputIndex_ = vin_json["outputIndex"].get<int>();
			tempvin.outputIndex = outputIndex_;

			tempTransaction.setVin(tempvin);
		}

		auto vout_ = tx_["vout"];

		for (auto& vout_json : vout_)
		{
			Vout tempvout;

			auto amount_ = vout_json["amount"].get<unsigned int>();
			tempvout.amount = amount_;

			auto publicid_ = vout_json["publicid"].get<string>();
			tempvout.publicid = publicid_;

			tempTransaction.setVout(tempvout);
		}

		Tx.push_back(tempTransaction);
	}
}


/*Transforms int into hex Coded ASCII.*/
inline const string BlockMicho::hexCodedASCII(unsigned int number)
{
	char res[9];
	sprintf_s(res, "%08X", number);

	return res;
}

/*"""""Hashes""""""*/
inline const string BlockMicho::hash(const string& code)
{
	return hexCodedASCII(generateID((unsigned char*)code.c_str()));
}


void BlockMicho::parseIds(void)
{
	this->ids.clear();

	unsigned int tempID;

	if (jsonData.is_null())
		return;

	string ss = jsonData.dump();

	string tx_id;
	bool mustAdd;
	
	/*Loops over every transaction*/
	for (const auto& TX : jsonData["tx"]) 
	{
		/*Loops through every element in jsonData['vin'].*/
		for (const auto& elementVin : TX["vin"])
		{
			/*Gets string from JSON.*/
			tx_id.append(elementVin["txid"].get<string>());
		}

		/*Hashes id and appends it to IDs.*/
		ids.push_back(hash(tx_id));

		tx_id.clear();
	}
}

void BlockMicho::buildTree()
{
	/*Parses from json to ids in Block*/
	parseIds();
	
	/*Copies IDs vector to temporary list 'nodes'.*/
	nodes.assign(ids.begin(), ids.end());

	std::list<std::string>::iterator itrTemp;

	/*While nodes list is not the Merkle Root...*/
	while (nodes.size() > 1) 
	{
		/*If node amount is uneven, it copies the last one to the back of the list.*/
		if (nodes.size() % 2)
		{
			nodes.push_back(nodes.back());
		}

		tree.insert(tree.end(), nodes.begin(), nodes.end());

		/*For every node in the list...*/
		for (auto i = nodes.begin(); i != nodes.end(); i++) 
		{
			/*Concats next node's content to the current node's content and hashes.*/
			*i = hash(*i + *std::next(i));

			/*Erases next node.*/
			nodes.erase(std::next(i));
		}
	}
	if (nodes.size()) 
	{
		tree.push_back(nodes.back());
		this->calculatedMerkleRoot = nodes.back();
		if (nodes.back() == jsonData["merkleroot"])
			this->isValidMR = "True";
		else
			this->isValidMR = "False";
	}
	this->validated = true;
}


/*Prints tree.*/
std::string BlockMicho::printTree(void) {
	if (!tree.size())
	{
		buildTree();
	}

	int levels = log2(tree.size() + 1);

	std::string result;

	/*Character between words. */
	const char* spacing = " ";
	int words_in_row, init, middle, abs_pos = 0;
	const int length = tree[0].length();

	/*Total number of positions in square row.*/
	const int row = (pow(2, levels) - 1) * length;

	/*Loops from higher to lower level.*/
	for (int i = levels; i > 0; i--) {
		/*Sets number of words in current row.*/
		words_in_row = pow(2, i - 1);

		/*Sets initial amount of characters before first word.*/
		init = (pow(2, levels - i) - 1) * length;

		/*Sets number of characters between words.*/
		if (words_in_row - 1)
			middle = (row - 2 * init - words_in_row * length) / (words_in_row - 1);
		else
			middle = 0;

		/*Adds 'init' amount of initial spacings.*/
		for (int j = 0; j < init; j++)
			result.append(spacing);

		/*Adds words plus characters between next word.*/
		for (int j = abs_pos; j < abs_pos + words_in_row; j++) {
			result.append(tree[j]);
			for (int k = 0; k < middle; k++)
				result.append(spacing);
		}

		/*Updates absolute position within tree.*/
		abs_pos += words_in_row;

		/*Goes down one level.*/
		result.append("\n\n");
	}

	return result;
}



//GETTERS
unsigned int BlockMicho::getHeight(void) {
	return height;
}

string BlockMicho::getMerkleRoot(void) {
	return merkleRoot;
}

unsigned int BlockMicho::getNtx(void) {
	return ntx;
}

string BlockMicho::getBlockid(void) {
	return blockid;
}

unsigned int BlockMicho::getNonce(void) {
	return nonce;
}

string BlockMicho::getPreviousBlockid(void) {
	return previousBlockId;
}

vector<Transaction>& BlockMicho::getTx() {
	return Tx;
}

//SETTERS
void BlockMicho::setBlockid(string blockId_) {
	blockid = blockId_;
}

void BlockMicho::setHeight(unsigned int height_) {
	height = height_;
}

void BlockMicho::setMerkleRoot(string merkleRoot_) {
	merkleRoot = merkleRoot_;
}

void BlockMicho::setNtx(unsigned int ntx_) {
	ntx = ntx_;
}

void BlockMicho::setNonce(unsigned int nonce_) {
	nonce = nonce_;
}

void BlockMicho::setPreviousBlockId(string previousBlockId_) {
	previousBlockId = previousBlockId_;
}

void BlockMicho::setTx(Transaction tx_) {
	Tx.push_back(tx_);
}


unsigned int BlockMicho::generateID(unsigned char* str)
{
	unsigned int ID = 0;
	int c;
	while (c = *str++)
		ID = c + (ID << 6) + (ID << 16) - ID;
	return ID;
}


const string BlockMicho::getData(const BlockInfo& data) 
{
	switch (data) 
	{
	case BlockInfo::BLOCKID:
		return getBlockid();
	case BlockInfo::BLOCK_NUMBER:
		return to_string(getHeight());
	case BlockInfo::CALCULATE_MROOT:
		if (!this->validated)
		{
			buildTree();
		}
		return this->calculatedMerkleRoot;
	case BlockInfo::SEE_MROOT:
		return getMerkleRoot();
	case BlockInfo::VALIDATE_MROOT:
		if (!this->validated)
		{
			buildTree();
		}
		return this->isValidMR;
	case BlockInfo::NTX:
		return to_string(getNtx());
	case BlockInfo::NONCE:
		return to_string(getNonce());
	case BlockInfo::PREVIOUS_BLOCKID:
		return getPreviousBlockid();
	}
}

