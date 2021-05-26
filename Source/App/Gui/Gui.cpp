#include "../../../Lib/ImGui/imgui.h"
#include "../../../Lib/ImGui/imgui_impl_allegro5.h"
#include "../../../Lib/ImGui/imgui_stdlib.h"
#include "../../../Lib/ImGuiFileDialog/ImGuiFileDialog.h"

#include "GUI.h"
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <allegro5/allegro_primitives.h>
#include <functional>

/*GUI data.*/
/***************************************/
namespace data 
{
	const unsigned int width = 1200;
	const unsigned int height = 650;

	const char* fixedFormat = ".json";
	int notSelectedIndex = -1;
}
/***************************************/

Gui::Gui(void) :
	guiDisp(nullptr),
	guiQueue(nullptr),
	action(Events::NONE),
	force(true),
	state(States::INIT),
	action_msg("none."),
	index(data::notSelectedIndex),
	chainLength(0)
{
	initAllegro();
}


/*Initializes Allegro resources and throws different
messages in case of different errors.*/
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

	else if (!(guiDisp = al_create_display(data::width, data::height)))
		throw std::exception("Failed to create display.");

	else 
	{
		al_register_event_source(guiQueue, al_get_keyboard_event_source());
		al_register_event_source(guiQueue, al_get_mouse_event_source());
		al_register_event_source(guiQueue, al_get_display_event_source(guiDisp));

		initialImGuiSetup();
	}
}


//Set up for GUI with ImGUI.
void Gui::initialImGuiSetup(void) const 
{
	al_set_target_backbuffer(guiDisp);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer bindings
	ImGui_ImplAllegro5_Init(guiDisp);

	//Sets screen to black.
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

/*Checks if user pressed ESC or closed display.
Manages allegro events in that regard.*/
/*Return true if user exits, flase otehrwise.*/
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
			al_acknowledge_resize(guiDisp);
			ImGui_ImplAllegro5_CreateDeviceObjects();
		}
	}
	return result;
}

const Events Gui::checkForEvent(void)
{
	Events result = Events::NONE;

	al_set_target_backbuffer(guiDisp);

	//If user pressed ESC or closed display, returns Events::END.
	if (windowEvents())
	{
		result = Events::END_EV;
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
			

			/*If it's not the first run after update...*/
			if (state > States::INIT)
			{
				//ImGui::NewLine(); ImGui::NewLine();

				/*Shows files from path.*/
				if (showFile())
				{
					result = Events::NEW_FILE_EV;
					state = States::FILE_OK;
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



			/*If file was correctly loaded:*/
			if (state > States::WAITING)
			{
				/*Shows blocks in BlockChain.*/
				showBlocks();
			}

			/*If block was selected*/
			if (state > States::FILE_OK)
			{
				ImGui::NewLine();

				/*Shows actions to perform to a given block.*/
				showBlockchainMenu();
				ImGui::NewLine(); ImGui::NewLine();

				/*If an action has been selected...*/
				if (index != data::notSelectedIndex)
				{
					/*Shows result of action applied to block.*/
					ImGui::Text("Result: ");
					ImGui::NewLine();
					ImGui::Text(shower.c_str());
					ImGui::NewLine();
					result = action;
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::NewLine();

	/*Exit button.*/
	displayWidget("Exit", [&result]() {result = Events::END_EV; });

	ImGui::SameLine();

	ImGui::End();

	/*Rendering.*/
	render();
	
	
	return result;
}

void Gui::actionSolved(void) 
{ 
	action = Events::NONE; 
}


/*Shows Blockchain manipulation menu*/
inline void Gui::showBlockchainMenu()
{
	ImGui::Text("Select an option: ");

	/*Button callback for both buttons.*/
	const auto button_callback = [this](const Events eventId, const char* msg) 
	{
		action = eventId;
		action_msg = msg;
	};

	/*Creates buttons for different functionalities.*/
	displayWidget("Block ID", std::bind(button_callback, Events::BLOCKID_EV, "Block ID"));
	ImGui::SameLine();
	displayWidget("Previous ID", std::bind(button_callback, Events::PREVIOUS_BLOCKID_EV, "Previous ID"));
	ImGui::SameLine();
	displayWidget("nTx", std::bind(button_callback, Events::TXN_EV, "Number of transactions"));
	ImGui::SameLine();
	displayWidget("Block Number", std::bind(button_callback, Events::BLOCK_NUMBER_EV, "Block Number"));
	

	displayWidget("Nonce", std::bind(button_callback, Events::NONCE_EV, "Nonce"));
	ImGui::SameLine();
	displayWidget("Calculate MR", std::bind(button_callback, Events::SEE_MROOT_EV, "Merkle Root calculation"));
	ImGui::SameLine();
	displayWidget("Validate MR", std::bind(button_callback, Events::VALIDATE_MROOT_EV, "Merkle Root validation"));
	ImGui::SameLine();
	displayWidget("Print tree", std::bind(button_callback, Events::PRINT_TREE_EV, "Tree printing"));

	/*Message with selected option.*/
	ImGui::Text(("Selected: " + action_msg).c_str());
}

inline void Gui::fileDialog()
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
			//falta pasar a estado init
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
inline void Gui::render() const 
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
	
	bool checker;
	for (unsigned int i = 0; i < chainLength; i++) 
	{
		checker = (index == i);
		displayWidget(std::bind(ImGui::Checkbox, ("Block " + std::to_string(i)).c_str(), &checker),

			[this, i, &checker]() 
			{
				if (checker) { index = i; state = States::BLOCK_OK; }
				else setAllFalse(States::FILE_OK);
			});
		ImGui::SameLine();
	}
	ImGui::NewLine();
	ImGui::Text(("Selected Blok: Block  " + (index != data::notSelectedIndex ? std::to_string(index) : "none.")).c_str());
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

void Gui::setInfoShower(const std::string& shower) 
{ 
	this->shower = shower; 
}


/*Cleanup. Frees resources.*/
Gui::~Gui() 
{
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	if (guiQueue)
		al_destroy_event_queue(guiQueue);
	if (guiDisp)
		al_destroy_display(guiDisp);
}


inline void Gui::setAllFalse(const States& revert, bool alsoFile) {
	action = Events::NONE;
	index = data::notSelectedIndex;
	state = revert;
	shower = "";
	action_msg = "none.";
	if (alsoFile)
		selected = "";
}


/*Displays given widget and applies callback according to widget state.*/
template <class Widget, class F1, class F2>
inline auto Gui::displayWidget(const Widget& widget, const F1& f1, const F2& f2) -> decltype(f1())
{
	if (widget())
		return f1();
	return f2();
}

/*Specialization of displayWidget template.
As ImGui::Button is the most used widget, when the given 'widget'
is actually a const char*, then the widget will be ImGui::Button.*/
template <class F1, class F2>
inline auto Gui::displayWidget(const char* txt, const F1& f1, const F2& f2)->decltype(f1()) 
{
	if (ImGui::Button(txt, ImVec2(125, 25)))
		return f1();
	return f2();
}
