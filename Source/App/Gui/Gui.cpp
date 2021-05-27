#include "../../../Lib/ImGui/imgui.h"
#include "../../../Lib/ImGui/imgui_impl_allegro5.h"
#include "../../../Lib/ImGui/imgui_stdlib.h"
#include "../../../Lib/ImGuiFileDialog/ImGuiFileDialog.h"

#include "GUI.h"
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <allegro5/allegro_primitives.h>
#include <functional>

/*Namespace with Gui Config Data.*/
namespace data 
{
	const unsigned int width = 1200;
	const unsigned int height = 650;

	int notSelectedIndex = -1;
}

/*Constructor*/
Gui::Gui(void) :
	guiDisplay(nullptr),
	guiQueue(nullptr),
	event(Events::NO_EV),
	state(States::INIT),
	actionMsg("None."),
	index(data::notSelectedIndex),
	chainLength(0)
{
	blockData = {"", "", "", "", "", ""};
	firstUpdate = false;
	initAllegro();
}


/*Initializes Allegro resources and throws different
messages in case of diverse errors.*/
void Gui::initAllegro()
{
	/*Initializes Allegro resources.*/
	if (!al_init())
		throw std::exception("Failed to initialize Allegro.");

	else if (!al_init_primitives_addon())
		throw std::exception("Failed to initialize primitives addon.");

	else if (!(al_install_keyboard()))
		throw std::exception("Failed to initialize keyboard addon.");

	else if (!(al_install_mouse()))
		throw std::exception("Failed to initialize mouse addon.");

	else if (!(guiQueue = al_create_event_queue()))
		throw std::exception("Failed to create event queue.");

	else if (!(guiDisplay = al_create_display(data::width, data::height)))
		throw std::exception("Failed to create display.");

	else 
	{
		al_register_event_source(guiQueue, al_get_keyboard_event_source());
		al_register_event_source(guiQueue, al_get_mouse_event_source());
		al_register_event_source(guiQueue, al_get_display_event_source(guiDisplay));

		initialImGuiSetup();
	}
}


//Set up for GUI with ImGUI.
void Gui::initialImGuiSetup(void) const 
{
	al_set_target_backbuffer(guiDisplay);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer bindings
	ImGui_ImplAllegro5_Init(guiDisplay);

	//Sets screen to black.
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

/*Checks if user pressed ESC or closed display.
Manages allegro events in that regard.*/
/*Return true if user exits, false otehrwise.*/
bool Gui::windowEvents(void)
{
	bool result = false;

	while ((al_get_next_event(guiQueue, &guiEvent)))
	{
		ImGui_ImplAllegro5_ProcessEvent(&guiEvent);

		/*If the display has been closed or if the user has pressed ESC, return true. */
		if (guiEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE || (guiEvent.type == ALLEGRO_EVENT_KEY_DOWN &&
			guiEvent.keyboard.keycode == ALLEGRO_KEY_ESCAPE))
		{
			result = true;
		}
		//If the display has been resized, it tells ImGUI to take care of it.
		else if (guiEvent.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
		{
			ImGui_ImplAllegro5_InvalidateDeviceObjects();
			al_acknowledge_resize(guiDisplay);
			ImGui_ImplAllegro5_CreateDeviceObjects();
		}
	}
	return result;
}

const Events Gui::checkForEvent(void)
{
	Events out = Events::NO_EV;

	al_set_target_backbuffer(guiDisplay);

	//If user pressed ESC or closed display, returns Events::END.
	if (windowEvents())
	{
		out = Events::END_EV;
	}
	else
	{
		/*Sets new ImGui window.*/
		prepareNewWindow();

		//Child 1
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::BeginChild("ChildL", ImVec2(data::width * 0.5f, data::height), true, window_flags);
			

			if (firstUpdate)
			{
				event = Events::FIRST_UPDATE_EV;
				firstUpdate = false;
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File System"))
				{
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			
			/*Custom file Dialog for loading json files.*/
			fileDialog();
			

			/*If it's not the first run shows file.*/
			if (state > States::INIT)
			{
				/*Shows files from filename path.*/
				if (showFile())
				{
					out = Events::NEW_FILE_EV;
					state = States::FILE_OK;
					firstUpdate = true;
				};
			}

			ImGui::EndChild();
		}
		
		ImGui::SameLine(); //Sameline to stack horizontally
		//Child 2
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			window_flags |= ImGuiWindowFlags_MenuBar;
			window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
			
			ImGui::BeginChild("ChildR", ImVec2(0, data::height), true, window_flags);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("BlockChain Explorer"))
				{
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}


			/*If file was correctly loaded we show the blockchain blocks.*/
			if (state > States::WAITING)
			{
				showBlocks();
			}

			/*If block was selected we show block info.*/
			if (state > States::FILE_OK)
			{
				ImGui::NewLine();

				showBlockInfo();

				/*Shows actions to perform to a given block.*/
				ImGui::NewLine();
				showBlockchainMenu();
				ImGui::NewLine(); ImGui::NewLine();

				/*If an action has been selected we print result.*/
				if (index != data::notSelectedIndex)
				{
					ImGui::Text("Result: ");
					ImGui::NewLine();
					ImGui::Text(resultMsg.c_str());
					ImGui::NewLine();
					out = event;
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::NewLine();

	/*Exit button.*/
	displayWidget("Exit", [&out]() {out = Events::END_EV; });

	ImGui::SameLine();

	ImGui::End();

	/*Rendering.*/
	renderScreen();
	
	return out;
}


/*Shows Blockchain manipulation menu*/
void Gui::showBlockchainMenu()
{
	ImGui::Text("Select an option: ");

	/*Button callback for both buttons(uses wrappers).*/
	const auto button_callback = [this](const Events eventId, const char* msg) 
	{
		event = eventId;
		actionMsg = msg;
	};

	/*Creates buttons for different functionalities.*/
	displayWidget("Calculate", std::bind(button_callback, Events::CALC_MROOT_EV, "Merkle Root calculation."));
	ImGui::SameLine();
	displayWidget("Validate MR", std::bind(button_callback, Events::VALIDATE_MROOT_EV, "Merkle Root validation."));
	ImGui::SameLine();
	displayWidget("Print tree", std::bind(button_callback, Events::PRINT_TREE_EV, "Tree printing."));

	/*Message with selected option.*/
	ImGui::Text(("Selected: " + actionMsg).c_str());
}

void Gui::fileDialog()
{
	// open Dialog Simple
	if (ImGui::Button("Open File Dialog", ImVec2(data::width/2 - 3, 40)))
		//ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a Directory", nullptr, ".");
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp,.json", ".");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		bool format = false;
		
		std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName(); //entire path with filename
		int last = filePathName.length();
		std::string subString = filePathName.substr(last - 4, last + 3);

		if (subString == "json")
		{
			format = true;
		}
		
		// if file format (json) is ok and user clicks ok... (otherwise path doesnt get laoded into member)
		if (ImGuiFileDialog::Instance()->IsOk() && format)
		{
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			
			int pathlen = filePath.length();
			std::string fileName = filePathName.substr(pathlen + 1, filePathName.length() - 1);

			this->filename = fileName;
			this->filePath = filePath;
			this->fileNamePath = filePathName;

			state = States::WAITING;
			path = filePathName;
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}



bool Gui::showFile()
{
	bool result = false;
	
	ImGui::NewLine(); 
	ImGui::NewLine();

	ImGui::Text("Current loaded file: ");
	ImGui::SameLine();

	/*Shows path.*/
	ImGui::TextWrapped(filename.c_str());
	ImGui::SameLine();

	if (ImGui::Button("Show Info", ImVec2(100, 35)))
	{
		setAllFalse(States::WAITING);
		result = true;
	}

	return result;
}

/*Prepare for new window*/
inline void Gui::prepareNewWindow() const
{
	//Sets new ImGUI frame.
	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();

	//Sets new window positioned at (0,0).
	ImGui::SetNextWindowSize(ImVec2(data::width, data::height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::Begin("EDA TP Bitcoin");
}

/*Rendering.*/
inline void Gui::renderScreen() const 
{
	ImGui::Render();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());

	al_flip_display();
}

/*Shows every block in the blockchain as a checkbox*/
void Gui::showBlocks(void)
{
	ImGui::Text("Select a Block: ");
	
	bool checker = true;
	for (unsigned int i = 0; i < chainLength; i++) 
	{
		checker = (index == i);
		displayWidget(std::bind(ImGui::Checkbox, ("Block " + std::to_string(i)).c_str(), &checker),

			[this, i, &checker]() 
			{
				if (checker) 
				{ 
					index = i; 
					state = States::BLOCK_OK;
				}
				else setAllFalse(States::FILE_OK);
			});
		ImGui::SameLine();
	}
	ImGui::NewLine();
	ImGui::Text(("Selected Block: Block  " + (index != data::notSelectedIndex ? std::to_string(index) : "none.")).c_str());
}

void Gui::showBlockInfo(void)
{
	ImGui::Text(("Selected Block ID: " + blockData.blockId).c_str());
	ImGui::Text(("Selected Block Previous ID: " + blockData.previousBlockId).c_str());
	ImGui::Text(("Selected Block Transaction count: " + blockData.TxN).c_str());
	ImGui::Text(("Selected Block Number: " + blockData.blockNumber).c_str());
	ImGui::Text(("Selected Block Nonce: " + blockData.nonce).c_str());
	ImGui::Text(("Selected Block MekRoot (as in provided file): " + blockData.mkRoot).c_str());
}

void Gui::setBlockShownData(BlockShowData data)
{
	this->blockData = data;
}

/*Getters.*/
const std::string& Gui::getFilename(void) 
{ 
	return fileNamePath; 
}

const unsigned int Gui::getBlockIndex(void) const 
{ 
	return index; 
}

/*Setters.*/
void Gui::setChainLength(unsigned int chainLength) 
{ 
	this->chainLength = chainLength; 
}

void Gui::setResultMsg(const std::string& shower) 
{ 
	this->resultMsg = shower; 
}


/*Cleanup. Frees resources.*/
Gui::~Gui() 
{
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	if (guiQueue)
		al_destroy_event_queue(guiQueue);
	if (guiDisplay)
		al_destroy_display(guiDisplay);
}


inline void Gui::setAllFalse(const States& revert) 
{
	event = Events::NO_EV;
	index = data::notSelectedIndex;
	state = revert;
	resultMsg = "";
	actionMsg = "none.";
}


/*Displays given widget and applies callback according to widget state.*/
template <class Widget, class F1, class F2>
inline auto Gui::displayWidget(const Widget& widget, const F1& f1, const F2& f2) -> decltype(f1())
{
	if (widget())
		return f1();
	return f2();
}

/*Specialization of displayWidget template.*/
template <class F1, class F2>
inline auto Gui::displayWidget(const char* txt, const F1& f1, const F2& f2)->decltype(f1()) 
{
	if (ImGui::Button(txt, ImVec2(125, 25)))
		return f1();
	return f2();
}
