#include "Client.h"
#include <iostream>




Client::Client(const std::string& ip, const unsigned int ownPort, const unsigned int outPort) : ip(ip), ownPort(ownPort),
outPort(outPort), multiHandler(nullptr), easyHandler(nullptr) 
{
	if (ip.length() && ownPort && outPort)
	{
		stillRunning = 1;
	}
	else
		throw std::exception("Wrong input in client.");

	url = ip + '/' + "eda_coins";
};

//Performs request.
bool Client::performRequest(void) 
{
	bool stillOn = true;
	static bool step = false;
	try {
		if (ip.length() && ownPort && outPort) 
		{
			if (!step) 
			{
				//Sets easy and multi modes with error checker.
				if (!(easyHandler = curl_easy_init()))
				{
					throw std::exception("Failed to initialize easy handler.");
				}
				
				if (!(multiHandler = curl_multi_init()))
				{
					throw std::exception("Failed to initialize multi handler.");
				}

				//If it's the first time in this run, it sets the request parameters.
				configureClient();
				step = true;
			}

			//Should be an if. Performs one request and checks for errors.
			if (stillRunning) 
			{
				if (curl_multi_perform(multiHandler, &stillRunning) != CURLM_OK) 
				{
					curl_easy_cleanup(easyHandler);
					curl_multi_cleanup(multiHandler);
					easyHandler = nullptr;
					multiHandler = nullptr;
					throw std::exception("Failed to connect.");
				}
			}
			else 
			{
				//Cleans used variables.
				curl_easy_cleanup(easyHandler);
				curl_multi_cleanup(multiHandler);

				//Resets step to false.
				step = false;

				//Resets stillRunning to 1;
				stillRunning = 1;

				//Parses answer.
				try 
				{
					answer = json::parse(rawReply);
				}
				catch (std::exception&) 
				{
					throw std::exception("Invalid Data. Wrong JSON format.");
				}
				std::cout << "Received: " << answer << std::endl;

				//Sets result to 'FALSE', to end loop.
				stillOn = false;
			}
		}
		else
		{
			throw std::exception("Invalid data.");
		}
	}
	catch (std::exception& e) 
	{
		answer = e.what();
		stillOn = false;
		step = false;
		stillRunning = 1;
		std::cout << "Received: " << answer << std::endl;
	}
	return stillOn;
}

size_t Client::myCallback(char* ptr, size_t size, size_t nmemb, void* userData) 
{
	std::string* userDataPtr = (std::string*)userData;

	userDataPtr->append(ptr, size * nmemb);

	return size * nmemb;
}

const json& Client::getAnswer() 
{
	return answer;
}

Client::~Client() {}