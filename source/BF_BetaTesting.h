#define BF_BETATEST_OK				0
#define BF_BETATEST_ASK_KEY			1
#define BF_BETATEST_CHECKING_KEY	2
#define BF_BETATEST_QUIT			3

extern uint32 iBetaTestingStatus;
 
void
BF_BetaTesting_InitStatus();

void
BF_BetaTesting_CheckKey_Start(BView * po_ViewOwner);

void
BF_BetaTesting_CheckKey_Finish(char *pc_Key);