#pragma once
#include <string>
#include <allegro5/allegro.h>
#include <vector>


/*Gui event codes.*/
const enum class Events:int 
{
	NO_EV = 0,
	END_EV,
	VALIDATE_MROOT_EV,
	ALL_MERKLE_EV,
	NEW_FILE_EV,
	PRINT_TREE_EV,
	CALC_MROOT_EV,
	FIRST_UPDATE_EV
};


const enum class States
{
	INIT = 0,
	WAITING,
	FILE_OK,
	BLOCK_OK
};


struct BlockShowData
{
	std::string blockId;
	std::string previousBlockId;
	std::string TxN;
	std::string blockNumber;
	std::string nonce;
	std::string mkRoot;
};


class Gui 
{
public:

	Gui();
	~Gui();

	const Events checkForEvent(void);

	const unsigned int getBlockIndex() const;

	const std::string& getFilename(void);

	void setResultMsg(const std::string&);
	void setBlockShownData(BlockShowData);

	void setChainLength(unsigned int);

private:
	
	/*Initial setup.*/
	
	void initAllegro();
	void initialImGuiSetup(void) const;
	

	/*Window displayers.*/
	void prepareNewWindow() const;
	void fileDialog();
	bool showFile();

	inline void showBlockchainMenu();
	void showBlocks();
	void showBlockInfo();


	/*This methods are wrappers to display different imgui widgets*/
	template <class Widget, class F1, class F2 = void(*)(void)>
	inline auto displayWidget(const Widget&, const F1& f1, const F2 & = []() {}) -> decltype(f1());


	template <class F1, class F2 = void(*)(void)>
	inline auto displayWidget(const char*, const F1& f1, const F2 & = []() {})->decltype(f1());

	inline void renderScreen() const;
	inline void setAllFalse(const States&);


	/*Exit and resize events.*/
	bool windowEvents(void);


	/*Allegro data members.*/
	ALLEGRO_DISPLAY* guiDisplay;
	ALLEGRO_EVENT_QUEUE* guiQueue;
	ALLEGRO_EVENT guiEvent;

	/*Flag data members.*/
	unsigned int chainLength;
	std::string actionMsg, resultMsg;
	Events event;
	States state;

	/*Data members modifiable by user.*/
	std::string path, selected;
	std::string filePath;
	std::string filename;
	std::string fileNamePath;
	unsigned int index;

	/*Misc.*/
	BlockShowData blockData;
	bool firstUpdate;
};