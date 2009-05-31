#include <Application.h>
#include <InterfaceKit.h>
#include <Window.h>
#include <String.h>

class BF_GUI_WinMain;

class BF_App : public BApplication {
public:
		bool 			bAskedQuit;  
		
						BF_App(const char* pc_AppFileName);
virtual	bool 			QuitRequested();
virtual	void			MessageReceived(BMessage * message);

		const BString	AppFileName();

private:
		BString			sAppFileName;
};

extern BF_App *poApp;


