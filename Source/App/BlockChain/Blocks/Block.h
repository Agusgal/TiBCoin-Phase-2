#pragma once
#include "Transactions/Transaction.h"
#include "json.hpp"
#include <list>


const enum class BlockInfo
{
	SEE_MROOT,
	CALCULATE_MROOT,
	BLOCKID,
	PREVIOUS_BLOCKID,
	NTX,
	BLOCK_NUMBER,
	NONCE,
	VALIDATE_MROOT
};

using namespace std;
using json = nlohmann::json;

class BlockMicho {
public:
	//friend Merkle;
	BlockMicho();
	BlockMicho(const json& block);
	~BlockMicho();

	const string getData(const BlockInfo& data);


	void setBlockid(string blockID_);
	void setHeight(unsigned int height_);
	void setMerkleRoot(string merkleRoot_);
	void setNtx(unsigned int ntx_);
	void setNonce(unsigned int nonce_);
	void setPreviousBlockId(string previousBlockId_);
	void setTx(Transaction tx_);

	//friend class Merkle;
	std::string printTree(void);

private:
	string fliptree(string str);
	
	string getBlockid(void);
	unsigned int getHeight(void);
	string getMerkleRoot(void);
	unsigned int getNtx(void);
	unsigned int getNonce(void);
	string getPreviousBlockid(void);
	vector<Transaction>& getTx();

	vector<Transaction> Tx;
	unsigned int ntx;
	unsigned int height;
	unsigned int nonce;
	string blockid;
	string previousBlockId;
	string merkleRoot;

	std::list<std::string> nodes, ids;
	void parseIds(void);


	/*"""Hashing"""*/
	static unsigned int generateID(unsigned char* str);
	inline static const std::string hex2ASCII(unsigned int);
	inline static const std::string hash(const std::string&);

	/*Merkle Tree*/
	void buildTree();
	std::vector<std::string> tree;

	friend class Merkle;

	json jsonData;
	string calculatedMerkleRoot;
	string isValidMR;
	bool validated;

	
};


