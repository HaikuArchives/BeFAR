## BeOS Generic Makefile v2.2 ##

## Fill in this file to specify the project being created, and the referenced
## makefile-engine will do all of the hard work for you.  This handles both
## Intel and PowerPC builds of the BeOS and Haiku.

## Application Specific Settings ---------------------------------------------

# specify the name of the binary
NAME=BeFAR

# specify the type of binary
#	APP:	Application
#	SHARED:	Shared library or add-on
#	STATIC:	Static library archive
#	DRIVER: Kernel Driver
TYPE=APP

#	add support for new Pe and Eddie features
#	to fill in generic makefile

#%{
# @src->@ 

#	specify the source files to use
#	full paths or paths relative to the makefile can be included
# 	all files, regardless of directory, will have their object
#	files created in the common object directory.
#	Note that this means this makefile will not work correctly
#	if two source files with the same name (source.c or source.cpp)
#	are included from different directories.  Also note that spaces
#	in folder names do not work well with this makefile.
SRCS= 	./_BazaLib/BL_File.cpp\
		./_BazaLib/BL_Tools.cpp\
		BF_Dict.cpp\
		BF_GUI_Func.cpp\
		BF_GUI_NodePanel_Tools.cpp\
		BF_GUI_NodePanel.cpp\
		BF_GUI_NodePanel_MoveDraw.cpp\
		BF_GUI_KeysMenu.cpp\
		BF_GUI_Setup.cpp\
		BF_GUI_SetupDialog.cpp\
		BF_GUI_CmdLine.cpp\
		BF_GUI_WinMain.cpp\
		BF_GUI_WinMain_Tools.cpp\
		BF_GUI_OperRoster.cpp\
		BF_Node.cpp\
		BF_Roster.cpp\
		Main.cpp\
		BF_GUI_TopMenu.cpp\
		./_BF_GUI/BF_GUI_Edit.cpp\
		BF_GUI_DlgViews.cpp\
		BF_GUI_DlgViews_Menu.cpp\
		BF_GUI_Misc.cpp\
		./_FtpPanel/BF_GUI_FilesPanel_Action_MakeSH.cpp\
		./_FtpPanel/BF_GUI_FilesPanel_InfoPanel.cpp\
		./_FilesPanel/BF_GUI_FilesPanel.cpp\
		./_FtpPanel/BF_GUI_FilesPanel_TrackerAddOns.cpp\
		./_FtpPanel/BF_GUI_FilesPanel_Tasks.cpp\
		BF_GUI_AttrDialog.cpp\
		./_Search/BF_GUI_FilesPanel_Search.cpp\
		./_FtpPanel/BF_GUI_FilesPanel_Monitor.cpp\
		./_TextViewer/BF_GUI_TextViewer.cpp\
		./_FtpPanel/BF_GUI_FtpPanel.cpp\
		./_FtpPanel/BF_GUI_FtpPanel_Tasks.cpp\
		./_FtpPanel/BF_GUI_FtpPanel_Threads.cpp\
		./_FtpPanel/FtpClient.cpp\
		./_ArcPanel/TextFile.cpp\
		./_ArcPanel/BF_GUI_ArcPanel.cpp\
		./_ImageViewer/BF_GUI_ImagePanel.cpp\
		BF_BetaTesting.cpp
		

#	specify the definition files for resources to use.
#	full path or a relative path to the file can be used.
RDEFS= 	BeFar.rdef\
		BeFarIcons.rdef

#	alternatively you can specify compiled resource files to use.
#	full path or a relative path to the resource file can be used.
#RSRCS= BeFarIcons.rsrc


# @<-src@ 
#%}

#	end support for Pe and Eddie

#	specify additional libraries to link against
#	there are two acceptable forms of library specifications
#	-	if your library follows the naming pattern of:
#		libXXX.so or libXXX.a you can simply specify XXX
#		library: libbe.so entry: be
#		
#	- 	if your library does not follow the standard library
#		naming scheme you need to specify the path to the library
#		and it's name
#		library: my_lib.a entry: my_lib.a or path/my_lib.a
LIBS=translation textencoding root be stdc++ bnetapi 

#	specify additional paths to directories following the standard
#	libXXX.so or libXXX.a naming scheme.  You can specify full paths
#	or paths relative to the makefile.  The paths included may not
#	be recursive, so include all of the paths where libraries can
#	be found.  Directories where source files are found are
#	automatically included.
LIBPATHS= 

#	additional paths to look for system headers
#	thes use the form: #include <header>
#	source file directories are NOT auto-included here
SYSTEM_INCLUDE_PATHS = 

#	additional paths to look for local headers
#	thes use the form: #include "header"
#	source file directories are automatically included
LOCAL_INCLUDE_PATHS = 

#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= 

#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES= 

#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS = 

#	specify whether image symbols will be created
#	so that stack crawls in the debugger are meaningful
#	if TRUE symbols will be created
SYMBOLS = 

#	specify debug settings
#	if TRUE will allow application to be run from a source-level
#	debugger.  Note that this will disable all optimzation.
DEBUGGER = 

#	specify additional compiler flags for all files
COMPILER_FLAGS =

#	specify additional linker flags
LINKER_FLAGS =

#	specify the version of this particular item
#	(for example, -app 3 4 0 d 0 -short 340 -long "340 "`echo -n -e '\302\251'`"1999 GNU GPL") 
#	This may also be specified in a resource.
APP_VERSION = 

#	(for TYPE == DRIVER only) Specify desired location of driver in the /dev
#	hierarchy. Used by the driverinstall rule. E.g., DRIVER_PATH = video/usb will
#	instruct the driverinstall rule to place a symlink to your driver's binary in
#	~/add-ons/kernel/drivers/dev/video/usb, so that your driver will appear at
#	/dev/video/usb when loaded. Default is "misc".
DRIVER_PATH = 

## include the makefile-engine
include $(BUILDHOME)/etc/makefile-engine

DICTS=	BeFar_Dict_Eng.dict \
		BeFar_Dict_French.dict \
		BeFar_Dict_Russia.dict \
		BeFar_Dict_Hun.dict \
		BeFar_Dict_Spa.dict

$(OBJ_DIR)/$(DICTS):
	for i in $(DICTS); do \
		cp $$i $(OBJ_DIR)/ ; \
	done 

default: $(OBJ_DIR)/$(DICTS)

