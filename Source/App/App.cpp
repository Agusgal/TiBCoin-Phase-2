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
		/*User exit.*/
	case Events::END_EV:
		running = false;
		break;
	case Events::NEW_FILE_EV:
		blockChain.loadBlockChain(gui->getFilename());
		gui->setChainLength(blockChain.getBlockAmount());
		break;
	case Events::BLOCKID_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCKID));
		break;
	case Events::SEE_MROOT_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::SEE_MROOT));
		break;
	case Events::PREVIOUS_BLOCKID_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::PREVIOUS_BLOCKID));
		break;
	case Events::NONCE_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NONCE));
		break;
	case Events::TXN_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NTX));
		break;
	case Events::BLOCK_NUMBER_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCK_NUMBER));
		break;
	case Events::VALIDATE_MROOT_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::VALIDATE_MROOT));
		break;
	case Events::PRINT_TREE_EV:
		gui->setInfoShower(blockChain.reprTree(gui->getBlockIndex()));
		break;
	case Events::BLOCK_SELECTED_EV:
	{
		string bId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCKID);
		string pbId = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::PREVIOUS_BLOCKID);
		string TxN = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NTX);
		string nonce = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::NONCE);
		string bNumber = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::BLOCK_NUMBER);
		string root = blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::SEE_MROOT);

		BlockShowData data = { bId, pbId, TxN, bNumber, nonce, root};
		gui->setBlockShownData(data);
		break;
	}
	case Events::CALC_MROOT_EV:
		gui->setInfoShower(blockChain.getBlockInfo(gui->getBlockIndex(), BlockInfo::CALCULATE_MROOT));
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


/*App destructor.*/
App::~App() 
{
	if (gui)
	{
		delete gui;
	}
}