#pragma once
#include "Gui/Gui.h"
#include "BlockChain/BlockChain.h"


class App 
{

public:
	App(void);
	~App(void);

	bool isRunning(void);

	void dispatcher(const Events&);
	const Events eventGenerator();

private:
	/*Prevents from using copy constructor.*/
	App(const App&);
	
	BlockChain blockChain;

	Gui* gui;

	bool running;
};