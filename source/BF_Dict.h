#ifndef __BF_DICT_H__
#define __BF_DICT_H__

#include "BL_Tools.h"

//////////////////////////////////////////////
#define BF_DICT_VOLUMEDIALOG_TITLE	0

/// top menu //
#define BF_DICT_TOPMENU_LEFT				1
#define BF_DICT_TOPMENU_COMMON			2
#define BF_DICT_TOPMENU_RIGHT				3

// top menu Common//
#define BF_DICT_TOPMENU_COMMON_MAINSETUP			4
#define BF_DICT_TOPMENU_COMMON_EMPTY_TRASH		5
#define BF_DICT_TOPMENU_COMMON_COMPARE				6
//
#define BF_DICT_TOPMENU_COMMON_SAVE_SETUP			7
#define BF_DICT_TOPMENU_COMMON_LOAD_SETUP			8
#define BF_DICT_TOPMENU_COMMON_DEFAULT_SETUP	9
#define BF_DICT_TOPMENU_COMMON_SAVE_PALETTE		10
#define BF_DICT_TOPMENU_COMMON_LOAD_PALETTE		11
//
#define BF_DICT_TOPMENU_COMMON_SELECTDICT			42
//
#define BF_DICT_TOPMENU_COMMON_ABOUT					12
//
#define BF_DICT_TOPMENU_COMMON_QUIT						13

// common messages ///
#define BF_DICT_OK				14
#define BF_DICT_CANCEL		15
#define BF_DICT_YES				16
#define BF_DICT_NO				17
#define BF_DICT_FILE			53
#define BF_DICT_SEARCH		52 //Search
#define BF_DICT_NAME			58 //Name
#define BF_DICT_GOTO			61 //Go to
#define BF_DICT_DIR				62 //Dir
#define BF_DICT_TYPE			65 //Type
#define BF_DICT_VALUE			66 //Value
#define BF_DICT_ERROR			67 //Error
#define BF_DICT_CLOSE			70 //Close
#define BF_DICT_SIZE			76 //Size
#define BF_DICT_WARNING		102 //Warning
#define BF_DICT_RENAME		104 //Rename
#define BF_DICT_VIEW			106 //View
#define BF_DICT_EDIT			107 //Edit
#define BF_DICT_COPY			108 //Copy
#define BF_DICT_MOVE			109 //Move
#define BF_DICT_PATH			121 //Path
#define BF_DICT_COPYTO			134 //Copy to
#define BF_DICT_SPECIAL		135 //Special
#define BF_DICT_MOVETO			136 //Move to // full
#define BF_DICT_QUESTIONS	137 //Question // full
#define BF_DICT_TARGETFILE		187 //Target file
#define BF_DICT_SCRIPTNAME		188 //Script name
#define BF_DICT_INFOLDER			189 //In folder
#define BF_DICT_ARGS					190 //Arguments
#define BF_DICT_SKIP					215 //'Skip'
#define BF_DICT_TRYAGAIN			216 //'Try again'
#define BF_DICT_OWERWRITE		223 //'Owerwrite'
#define BF_DICT_CANCELALL		224 //'Cancel all'
#define BF_DICT_CREATE				231 //'Create'
#define BF_DICT_DELETE				232 //'Delete'
#define BF_DICT_OWERWRITEALL	258 //'Owerwrite all'
#define BF_DICT_SKIPALLERRORS	260 //'Skip all errors'
#define BF_DICT_APPEND				261 //'Append'



// vertical menu //
#define BF_DICT_MENU_SORTNAME									18	
#define BF_DICT_MENU_SORTSIZE									19
#define BF_DICT_MENU_SORTDATE									20
#define BF_DICT_MENU_SORTTYPE									21
#define BF_DICT_MENU_SORTEXT									22
//
#define BF_DICT_MENU_SHOWHIDE_ICONS						23
#define BF_DICT_MENU_FULLMODE									24
#define BF_DICT_MENU_COLSMODE									25
#define BF_DICT_MENU_PANELSETUP								26

// VERTICALMENU__FILES PANEL //
#define BF_DICT_MENU_SEARCH										27
#define BF_DICT_MENU_CHANGECASE								28
#define BF_DICT_MENU_TRACKER_ADDONS						29
#define BF_DICT_MENU_SHOWINFO									30
#define BF_DICT_MENU_SETOPPATH								31
#define BF_DICT_MENU_GOTO											32
#define BF_DICT_MENU_REFRESH									33
#define BF_DICT_MENU_SHOWATTRS								34
#define BF_DICT_MENU_MAKESHLINK								35

// PANEL //
#define BF_DICT_PANEL_COL_NAME								36
#define BF_DICT_PANEL_COL_TYPE								37
#define BF_DICT_PANEL_COL_SIZE								38
#define BF_DICT_PANEL_COL_DATE								39
//
#define BF_DICT_PANEL_BYTESIN									40
#define BF_DICT_PANEL_FILES										41
#define BF_DICT_PANEL_DIR											122 //<dir>
#define BF_DICT_PANEL_FILEEMPTYNAME						123 //Are you crazy? File can`t has empty name
#define BF_DICT_PANEL_SEL_INVERT								124 //Invert selection
#define BF_DICT_PANEL_SEL_SET									125 //Set selection
#define BF_DICT_PANEL_SEL_UNSET								126 //Unset selection
#define BF_DICT_PANEL_MASK											127 //Mask
#define BF_DICT_PANEL_FOLDERALSO								128 //Folder also
#define BF_DICT_PANEL_SETUP										129 //Panel setup
#define BF_DICT_PANEL_SHOWICONS								130 //Show icons
#define BF_DICT_PANEL_SHOWDATES								131 //Show dates
#define BF_DICT_PANEL_SHOWSIZES								132 //Show sizes
#define BF_DICT_PANEL_SHOWTYPES								133 //Show types
#define BF_DICT_PANEL_CREATEFOLDER_TITLE			180 //Create folder  // Dialog title
#define BF_DICT_PANEL_DELETEFILES							138 //Delete files ?  // Dialog line
#define BF_DICT_PANEL_PANELEMPTY								139 //Sorry, but panel is empty //Dialog line
#define BF_DICT_PANEL_FROM											140 //From
#define BF_DICT_PANEL_TO												141 //To


//

// SELECT_LANGUAGE //
#define BF_DICT_SELDICT_TITLE									43


// TEXT_VIWER //
#define BF_DICT_TEXTVIEWER_POS								44
#define BF_DICT_TEXTVIEWER_COL								45
#define BF_DICT_TEXTVIEWER_CP									46
#define BF_DICT_TEXTVIEWER_SHOWSPEC						47
#define BF_DICT_TEXTVIEWER_HIDESPEC						48
#define BF_DICT_TEXTVIEWER_NEXT								49	
#define BF_DICT_TEXTVIEWER_CLOSE							50
#define BF_DICT_TEXTVIEWER_CODEPAGE						51
#define BF_DICT_TEXTVIEWER_SEARCH_DLG					277 //"Search text"
#define BF_DICT_TEXTVIEWER_SEARCH_DLGTEXT			278 //"Text:"


// FILES SEARCH //
#define BF_DICT_FILESEARCH_FILENAME						54 // File name
#define BF_DICT_FILESEARCH_BYTEXT							55 
#define BF_DICT_FILESEARCH_FROMCURFOLDER			56 // from current folder
#define BF_DICT_FILESEARCH_ONVOLUMES					57 // on Volumes (use Ins for select)
#define BF_DICT_FILESEARCH_SIZETYPE						59 //Size/Type
#define BF_DICT_FILESEARCH_PROGRESS						60 // Search in progress...
#define BF_DICT_FILESEARCH_FOUNDED					  63 // Searching finished. Founded
#define BF_DICT_FILESEARCH_FILES					    64 // files  // for example : Founded 123 files )

// ATTR_DIALOG //
#define BF_DICT_ATTRDLG_OPENERROR							68 // can`t open file; error :
#define BF_DICT_ATTRDLG_TITLE									69 // File attributes

// MAIN SETUP //
#define BF_DICT_MAINSETUP											71// Main setup
#define BF_DICT_MAINSETUP_COLSFONTS						72// Colors and fonts
#define BF_DICT_MAINSETUP_FILES								73// File operations setup
#define BF_DICT_MAINSETUP_DEBUG								74// Debug setup

#define BF_DICT_MAINSETUP_ASKEXIT							83// Ask before exit
#define BF_DICT_MAINSETUP_VOLSHOWHOME					84// Vol.dialog: show home,desktop,boot
#define BF_DICT_MAINSETUP_VOLSTACK							85// Vol.dialog: show folder-stack
#define BF_DICT_MAINSETUP_VOLICONS							86// Vol.dialog: show icons
#define BF_DICT_MAINSETUP_PATHICON							87// Panel: show path_icon
#define BF_DICT_MAINSETUP_PANBACK							88// Panel: use backspace
#define BF_DICT_MAINSETUP_PANRET							  89// Panel: return to current folder
#define BF_DICT_MAINSETUP_CMDPATH							90// Command line : show path
#define BF_DICT_MAINSETUP_TRACKADDICONS				91// Tracker Add-Ons menu : show icons
#define BF_DICT_MAINSETUP_SEARCHVOLICONS				92// Search : setup : show volumes icons
#define BF_DICT_MAINSETUP_AUTOSAVE							93// Setup auto saving

// FONTS SETUP //
#define BF_DICT_FONTSETUP_ANTI								75// Anti aliasing
#define BF_DICT_FONTSETUP_FONTPANEL						78// Font : Panel
#define BF_DICT_FONTSETUP_FONTDIALOG					79// Font : Dialog
#define BF_DICT_FONTSETUP_FONTMONO						80// Font : Mono
#define BF_DICT_FONTSETUP_FONTCOMMAND					81// Font : Command prompt
#define BF_DICT_FONTSETUP_FLAGS								82// Flags

// SETUP>FILE OPERATIONS //
#define BF_DICT_SETUPFILES_SHOWDLG						94 // Taskdialog: show after new task
#define BF_DICT_SETUPFILES_COPYDES						95 // Deselect after coping
#define BF_DICT_SETUPFILES_MOVEDES						96 // Deselect after move
#define BF_DICT_SETUPFILES_ASKDEL							97 // Ask before files deleting
#define BF_DICT_SETUPFILES_ASKCOPY						98 // Ask before copy

// FILES PANEL //
#define BF_DICT_FILES_LOADING									99	//Loading
#define BF_DICT_FILES_FROM 										100 //' from '
#define BF_DICT_FILES_PATHHIST_EMPTY					101 //sorry, but path_history is empty
#define BF_DICT_FILES_MKLINK									103 //MkLink
#define BF_DICT_FILES_TOVOID									105 //To void
#define BF_DICT_FILES_MKDIR										110 //MkDir
#define BF_DICT_FILES_TOTRASH 								111	//To trash
#define BF_DICT_FILES_MKFOLDEMPTY							112	//I can`t make folder with empty name
#define BF_DICT_FILES_VOLREAD									113	//Are you crazy ? This volume is READ_ONLY !
#define BF_DICT_FILES_ERRMKDIR								114	//Error in MakeDir
#define BF_DICT_FILES_TOLOWER									115	//To lower
#define BF_DICT_FILES_TOUPPER									116	//To upper
#define BF_DICT_FILES_CAPITALISE							117	//Capitalise
#define BF_DICT_FILES_CAPITALISEEW						118 //Capitalise EW
#define BF_DICT_FILES_CHANGENAMES							119 //Change file names
#define BF_DICT_FILES_MKLINKS									120 //Make links to
#define BF_DICT_FILES_CANTRUN								185 //I can`t run this file
#define BF_DICT_FILES_MKSHLINK								186 //Make SH-link
#define BF_DICT_FILES_MKSHLINK_SETICONS			191 //Set icons from source file
#define BF_DICT_FILES_MKSH_ERROR1						192 //Error while folder opening
#define BF_DICT_FILES_MKSH_ERROR2						193 //Store folder not found
#define BF_DICT_FILES_MKSH_ERROR3						194 //This file exists
#define BF_DICT_FILES_MKSH_ERROR4						195 //Write permission request denied
#define BF_DICT_FILES_MKSH_ERROR5						196 //Error while file creating

// SETUP ///
#define BF_DICT_SETUP_ERR1												181 //can`t create file vars.settings,error : 
#define BF_DICT_SETUP_ERR2												174 //can`t make settings dir
#define BF_DICT_SETUP_ERR3												175 //can`t create setting_file,error : 
#define BF_DICT_SETUP_ERR4												176 //can`t load palete_file,error : 
#define BF_DICT_SETUP_ERR5												177 //can`t create palete_file,error : 

// SETUP>COLORS //
#define BF_DICT_SETUPCOLOR_PANELBACK							182 //Panel : Back
#define BF_DICT_SETUPCOLOR_PANELFRAME							142 //Panel : Frame
#define BF_DICT_SETUPCOLOR_PANELNODE							143 //Panel : Node
#define BF_DICT_SETUPCOLOR_PANELNODELINK					144 //Panel : Node : Link
#define BF_DICT_SETUPCOLOR_PANELNODEDIR 					145 //Panel : Node : Dir
#define BF_DICT_SETUPCOLOR_PANELNODEDIRLINK				146 //Panel : Node : Dir : Link
#define BF_DICT_SETUPCOLOR_PANELINFOTEXT 					147 //Panel : Info : Text
#define BF_DICT_SETUPCOLOR_PANELCURSORBACK			  148 //Panel : Cursor : Back
#define BF_DICT_SETUPCOLOR_PANELCURSORTEXT				149 //Panel : Cursor : Text
#define BF_DICT_SETUPCOLOR_PANELHEADERSTEXT				150 //Panel : Headers : Text
#define BF_DICT_SETUPCOLOR_PANELFOCUSPATHTEXT 		151 //Panel : FocusPath :  Text
#define BF_DICT_SETUPCOLOR_PANELFOCUSPATHBACK			152 //Panel : FocusPath :  Back
#define BF_DICT_SETUPCOLOR_PANELPATHTEXT					153 //Panel : Path : Text
#define BF_DICT_SETUPCOLOR_PANELPATHBACK					178 //Panel : Path : Back
#define BF_DICT_SETUPCOLOR_PANELNODESELECTING			154 //Panel : Node : Selecting
#define BF_DICT_SETUPCOLOR_PANELNODEFOCUSSELECTING 155 // Panel : Node : Focus&Selecting
#define BF_DICT_SETUPCOLOR_DIALOGFOCUSEDBORDER   	156 // Dialog : FocusedBorder
#define BF_DICT_SETUPCOLOR_DIALOGUNFOCUSEDBAR			157 // Dialog : UnfocusedBorder
#define BF_DICT_SETUPCOLOR_DIALOGBACK							158 // Dialog : Back
#define BF_DICT_SETUPCOLOR_DIALOGTEXT							159 // Dialog : Text
#define BF_DICT_SETUPCOLOR_DIALOGMENU							160 // Dialog : Menu : Item selected
#define BF_DICT_SETUPCOLOR_DIALOGEDITTEXT					161 // Dialog : Edit : Text
#define BF_DICT_SETUPCOLOR_DIALOGEDITBACK					162 // Dialog : Edit : Back
#define BF_DICT_SETUPCOLOR_DIALOGBUTTONFOCUS			163 // "Dialog : Button : Focus
#define BF_DICT_SETUPCOLOR_DIALOGBUTTONTEXT				164 // Dialog : Button : Text
#define BF_DICT_SETUPCOLOR_DIALOGPROGRESSBARE			165 // Dialog : ProgressBar : Empty
#define BF_DICT_SETUPCOLOR_DIALOGPROGRESSBARF			166 // "Dialog : ProgressBar : Filled"
#define BF_DICT_SETUPCOLOR_DIALOGRED						  167 // Red
#define BF_DICT_SETUPCOLOR_COMMANDLINE_BACK				168 // CommandLine : Back
#define BF_DICT_SETUPCOLOR_COMMANDLINE_TEXT				169 // CommandLine : Text
#define BF_DICT_SETUPCOLOR_TEXTVIEWER1						170 // TextViewer : Status bar : text
#define BF_DICT_SETUPCOLOR_TEXTVIEWER2						171 //TextViewer : Status bar : back
#define BF_DICT_SETUPCOLOR_TEXTVIEWER3						172 // TextViewer : Text
#define BF_DICT_SETUPCOLOR_TEXTVIEWER4						173 // TextViewer : Background : back

#define BF_DICT_PATH_EMPTY												183 // command`s history is empty
#define BF_DICT_PATH_CANNOTRUN										184 // can`t run this command

// INFO_PANEL ///
#define BF_DICT_INFOPANEL_TITLE									198 //File information
#define BF_DICT_INFOPANEL_SIZE										199 //'Size: '
#define BF_DICT_INFOPANEL_DATE										200 //'Date: '
#define BF_DICT_INFOPANEL_PERMS									201 //'Perms: '
#define BF_DICT_INFOPANEL_MIME										202 //'MIME: '
#define BF_DICT_INFOPANEL_CAPACITY								202 //'Capacity: '
#define BF_DICT_INFOPANEL_FREEBYTES							203 //'Free bytes : '
#define BF_DICT_INFOPANEL_REMOVEABLE							204 //'Removeable '
#define BF_DICT_READONLY													205 //'Read only'
#define BF_DICT_CANTLOADINFO										 	206 //'Can`t load info'


// TASKS //
#define BF_DICT_TASKS_READONLYVOL								207 //'Are you crazy ? This volume is READ_ONLY !'
#define BF_DICT_TASKS_ONENAMEMANYFILES						208	//'Sorry, but I can`t give one name to many files '
#define BF_DICT_TASKS_DESTFOLDERNAMEWRONG				209	//'Are you crazy ? Name of dest.folder is wrong'
#define BF_DICT_TASKS_CREATEITNOW								210	//'Create it now!'
#define BF_DICT_TASKS_DESTFOLDERNOTEXIST					211	//"Dest.folder is not existing"
#define BF_DICT_TASKS_CANTMAKEFOLDER							212	//'Sorry,i can`t make folder!'
#define BF_DICT_TASKS_DESTVOLREADONLY						213	//'Are you crazy ? Destination volume is READ_ONLY !
#define BF_DICT_TASKS_CANTMAKETRASH							214	//'can`t make Trash_Dir '
#define BF_DICT_TASKS_SYSFOLDERDELETE						217	//'This is a system folder. Do you really want to delete it ?'
#define BF_DICT_TASKS_COPYINGFILES								218	//'copying files'
#define BF_DICT_TASKS_COPYINGFILESFROM						219	//'copying files from /'
#define BF_DICT_TASKS_COPYINGFILESTO							220	//' to /'
#define BF_DICT_TASKS_DESTFILE										221	//'Dest.file '
#define BF_DICT_TASKS_DESTFILEEXIST							222	//' is existing'
#define BF_DICT_TASKS_CANTMOVE										225	//' Can`t move file '
#define BF_DICT_TASKS_CANTMAKERENAME							226	//'Sorry, but I can`t make links and rename them at one time.'
#define BF_DICT_TASKS_CANTMAKEINFOLDER						227	//'Sorry, but I can`t make links in folder'
#define BF_DICT_TASKS_CANTMAKELINKONENAME				227	//'Sorry, but I can`t make all links with one name'
#define BF_DICT_TASKS_EXISTSSKIPASK							228	//' exists. Skip it ?'

// TRACKER ADDONS DIALOG //
#define BF_DICT_TRADDS_NONE											229 //'can`t find any tracker add-ons'

// FTP_PANEL //
#define BF_DICT_FTP_DISCONNECT										233 //'Disconnect'
#define BF_DICT_FTP_MAKEDIR											234 //'MakeDir'
#define BF_DICT_FTP_CONNECTED										235 //'Ftp:Connected'
#define BF_DICT_FTP_ERROR												236 //'ftp error'
#define BF_DICT_FTP_EDITBKM											237 //'Edit bookmark'
#define BF_DICT_FTP_BKM_TITLE										238 //'Title:'
#define BF_DICT_FTP_BKM_HOST											239 //'Host:'
#define BF_DICT_FTP_BKM_USERNAME									240 //'User name:'
#define BF_DICT_FTP_BKM_PWD											241 //'Password:'
#define BF_DICT_FTP_BKM_FOLDER										242 //'Folder:'
#define BF_DICT_FTP_PANELTITLE										243 //"Ftp bookmarks"
#define BF_DICT_FTP_CANTMAKESETDIR								244 //"can`t make settings dir"
#define BF_DICT_FTP_CANTMAKESETFILE							245 //"can`t make setting_file "
#define BF_DICT_FTP_CONNECTING										246 //"Ftp:Connecting..."
#define BF_DICT_FTP_DIRCHANGING									247 //"Ftp:Dir changing..."
#define BF_DICT_FTP_NOTREADY											248 //"ftp_panel not ready"
#define BF_DICT_FTP_DIRRELOADING									249 //"Ftp:Dir reloading..."
#define BF_DICT_FTP_BKM_ASKDEL										250 //"Delete this bookmark ?"
#define BF_DICT_FTP_RELOADFOLDER									251 //"Reload folder"
#define BF_DICT_FTP_TASKDOWNLOAD									252 //"download from ftp"
#define BF_DICT_FTP_TASKDEL											253 //"delete file(s) from ftp"
#define BF_DICT_FTP_ERRGETNAME										254 //"Can`t get name of current dir"
#define BF_DICT_FTP_ERRCONNECT										255 //"Couldn't connect"

// OPERROSTER //
#define BF_DICT_ROSTER_OFF												256	//" of "
#define BF_DICT_ROSTER_INFO												257	//"Info"
#define BF_DICT_ROSTER_COPYASK										262	//"Copy question"
#define BF_DICT_ROSTER_FILE												263	//"file "
#define BF_DICT_ROSTER_EXISTS											264	//" exists"
#define BF_DICT_ROSTER_PREPARING									265	//"preparing to process"
#define BF_DICT_ROSTER_PAUSED											266	//" PAUSED "
#define BF_DICT_ROSTER_TOTAL											267	//"total"
#define BF_DICT_ROSTER_TITLE											268	//"File operations"
#define BF_DICT_ROSTER_PAUSERUN										269	//"Pause/Run"
#define BF_DICT_ROSTER_MINIMISE										270	//"Minimize[Esc]"
#define BF_DICT_ROSTER_OPERPROGRESS								276 //"operations progress"

#define BF_DICT_PAL_LOAD													271 //Load palete
#define BF_DICT_PAL_SAVE													272 //Save palete
#define BF_DICT_PAL_FILENAME											273 //'File name:'

/////////////////////////////////////////////////
#define BF_DICT_COUNT 280


void 				BF_Dict_Load(const char *pc_DictName);
const char* BF_DictAt(uint32 i_Index);
const char* BF_DictCurrentFile();

#endif // __BF_DICT_H__