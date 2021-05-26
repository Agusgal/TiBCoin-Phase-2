#pragma once
#include "Transaction.h"
//#include "Merkle.h"
//#include <nlohmann/json.hpp>
//#include "json.hpp"


using namespace std;
using json = nlohmann::json;

class Block {
public:
	Block();
	Block(const json& block);
	~Block();

	string getBlockid(void);
	unsigned int getHeight(void);
	string getMerkleRoot(void);
	unsigned int getNtx(void);
	unsigned int getNonce(void);
	string getPreviousBlockid(void);
	vector<Transaction>& getTx();


	void setBlockid(string blockID_);
	void setHeight(unsigned int height_);
	void setMerkleRoot(string merkleRoot_);
	void setNtx(unsigned int ntx_);
	void setNonce(unsigned int nonce_);
	void setPreviousBlockId(string previousBlockId_);
	void setTx(Transaction tx_);

	//friend class Merkle;

private:
	vector<Transaction> Tx;
	unsigned int ntx;
	unsigned int height;
	unsigned int nonce;
	string blockid;
	string previousBlockId;
	string merkleRoot;
	json block_json;
	unsigned int generateID(unsigned char* str);
	//vector de transacciones
};


