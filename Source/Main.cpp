#include <iostream>
#include "App/App.h"


int main()
{
	try 
	{
		App myApp;
		Events ev;

		while (myApp.isRunning()) 
		{
			ev = myApp.eventGenerator();
			
			myApp.dispatcher(ev);
		}
	}
	catch (std::exception& e) 
	{
		std::cout << e.what() << std::endl;
	}
}
