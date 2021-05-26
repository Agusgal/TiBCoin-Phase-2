#include "Block.h"
#include <string>


using namespace std;

//CONSTRUCTOR Y DESTRUCTOR
Block::Block()
{

}

Block::~Block()
{

}

Block::Block(json block)
{

	ntx = block["nTx"].get<unsigned int>();
	height = block["height"].get<unsigned int>();
	nonce = block["nonce"].get<unsigned int>();
	blockid = block["blockid"].get<string>();
	previousBlockId = block["previousblockid"].get<string>();

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


//GETTERS
unsigned int Block::getHeight(void) {
	return height;
}

string Block::getMerkleRoot(void) {
	return merkleRoot;
}

unsigned int Block::getNtx(void) {
	return ntx;
}

string Block::getBlockid(void) {
	return blockid;
}

unsigned int Block::getNonce(void) {
	return nonce;
}

string Block::getPreviousBlockid(void) {
	return previousBlockId;
}

vector<Transaction>& Block::getTx() {
	return Tx;
}

//SETTERS
void Block::setBlockid(string blockId_) {
	blockid = blockId_;
}

void Block::setHeight(unsigned int height_) {
	height = height_;
}

void Block::setMerkleRoot(string merkleRoot_) {
	merkleRoot = merkleRoot_;
}

void Block::setNtx(unsigned int ntx_) {
	ntx = ntx_;
}

void Block::setNonce(unsigned int nonce_) {
	nonce = nonce_;
}

void Block::setPreviousBlockId(string previousBlockId_) {
	previousBlockId = previousBlockId_;
}

void Block::setTx(Transaction tx_) {
	Tx.push_back(tx_);
}


//
unsigned int Block::generateID(unsigned char* str)
{
	unsigned int ID = 0;
	int c;
	while (c = *str++)
		ID = c + (ID << 6) + (ID << 16) - ID;
	return ID;
}


