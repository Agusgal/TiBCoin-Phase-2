#include "App.h"

//App Constructor
App::App(void) : running(true)
{
	gui = new Gui;
}

//Dispatcher that decides based on Gui events.
void App::dispatcher(const Events& event) 
{
	switch (event)
	{
	case Events::END_EV:
		running = false;
		break;
	case Events::NEW_FILE_EV:
		blockChain.loadBlockChain(gui->getFilename());
		gui->setChainLength(blockChain.getBlockQuantity());
		break;
	case Events::FIRST_UPDATE_EV:
		updateGuiBlockData();
		break;
	case Events::VALIDATE_MROOT_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::VALIDATE_MROOT));
		break;
	case Events::PRINT_TREE_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.drawTree(gui->getBlockIndex()));
		break;
	case Events::CALC_MROOT_EV:
		updateGuiBlockData();
		gui->setResultMsg(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::CALCULATE_MROOT));
		break;
	default:
		break;
	}
}


/*Generates event from GUI.*/
const Events App::eventGenerator() 
{ 
	return gui->checkForEvent();
}


bool App::isRunning(void) 
{
	return running; 
}

void App::updateGuiBlockData()
{
	string bId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCKID);
	string pbId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::PREVIOUS_BLOCKID);
	string TxN = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NTX);
	string nonce = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NONCE);
	string bNumber = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCK_NUMBER);
	string root = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::SEE_MROOT);

	BlockShowData data = { bId, pbId, TxN, bNumber, nonce, root };
	gui->setBlockShownData(data);
}


/*App destructor.*/
App::~App() 
{
	if (gui)
	{
		delete gui;
	}
}
