// Began using this as a default Script for testing with 8.1.0.2 Dated 5/7/2014

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/*

Customer:					JBT FoodTech
Location:					Madera, CA
Machine Type:				Template
Commissioned By:
:


Revisions after Startup:

1.  Date of Change:			9/11/2014
	Change made by:			Ripley
	Description of Change:	Built for 8.1.0.3 Release

2.  Date of Change:			5/14/2015
	Change made by:			Ripley
	Description of Change:	Built for 9.1.0.1 Release

	a. Added PID Ramp Test

3.  Date of Change:			1/26/2016
	Change made by:			Lizeth Kotheimer
	Description of Change:	Built for 9.1.0.1 Release

	a. Added Trending Data Test.

4.  Date of Change:			1/29/2016
	Change made by:			RIP
	Description of Change:	Built for 10.1.0.1 Release

	a. Added Trending Data Test.

5.  Date of Change:			6/13/2019
	Change made by:			Maria Milian
	Description of Change:	Built for 12.1.0.1 Release

6.  Date of Change:			1/7/2020
	Change made by:			Rick Ripley
	Description of Change:	Rebuilt for 12.1.0.2 Release

7.  Date of Change:			3/24/2020
	Change made by:			Randy Hunsaker
	Description of Change:	Updated with TCIO bindings
*/
//////////////////	//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pinesdisplay.h"
#include "pinesio.h"
#include "Timers.h"
#include "TCIO.h"
//#include "wrecord.h"


IUnknown* pIUnknownDisplay = NULL;
IUnknown* pIUnknownIO = NULL;
IUnknown* pIUnknownDIO = NULL;
IUnknown* pIUnknownPID = NULL;
IUnknown* pIUnknownPT = NULL;
IUnknown* pIUnknownPV = NULL;
IUnknown* pIUnknownTaskloop = NULL;
IUnknown* pIUnknownProfiler = NULL;
IUnknown* pIUnknownNumerical = NULL;
IUnknown* pIUnknownBall = NULL;
IUnknown* pIUnknownTunacal = NULL;

CString strVal;

LPCLSID pclsidDisplay = NULL;
LPCLSID pclsidAIO = NULL;
LPCLSID pclsidDIO = NULL;
LPCLSID pclsidPID = NULL;
LPCLSID pclsidPT = NULL;
LPCLSID pclsidPV = NULL;
LPCLSID pclsidTaskloop = NULL;
LPCLSID pclsidNumerical = NULL;
LPCLSID pclsidBall = NULL;
LPCLSID pclsidTunacal = NULL;
LPCLSID pclsidProfiler = NULL;


LPDISPATCH pDispatchDisplay = NULL;
LPDISPATCH pDispatchIO = NULL;
LPDISPATCH pDispatchDIO = NULL;
LPDISPATCH pDispatchPID = NULL;
LPDISPATCH pDispatchPT = NULL;
LPDISPATCH pDispatchPV = NULL;
LPDISPATCH pDispatchTaskloop = NULL;
LPDISPATCH pDispatchNumerical = NULL;
LPDISPATCH pDispatchBall = NULL;
LPDISPATCH pDispatchTunacal = NULL;
LPDISPATCH pDispatchProfiler = NULL;


IPSLDisplay Display;
IAnalogIO AnalogIO;
IDigitalIO DigitalIO;
IPIDCommands PID;
IProcessVariable PV;
ITaskLoop TaskLoop;
INumericalProcess Numerical;
IBallProcess Ball;
ITunacalProcess Tuna;
IProfiler Profiler;
TCIO Tcio;

CTimers PinesTime1;
CTimers PinesTime2;
CTimers PinesTime3;
CTimers PinesTime4;

CTimers PinesTime7;		//this timer is used for the analog ramp period and rate tests. and timer tests
CTimers PinesTime8;		//this is the timer used in get enter key to also display operator entry delay.
CTimers PinesTime100;	//this timer is for the trending data log to appear every couple minutes
IEncoder Encoder;


#define	True 1
#define	False 0
#define Onn 1
#define Off 0

double mem = 0;

/* ================  Declaring Functions used by the main script  =============== */


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Global Declarations
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void MainScript();						// my function tests function
void MyInit();
void ControllerCheck();				//spawned task to check controller
void FnTest();
void ShootSomeLogs();
void PIDTest();
void TaskLoopTest();

bool CheckKeyboard(CString);
bool CheckDigitalOut(int);
bool CheckDigitalIn(int);
bool CheckAnalogOut(int);
bool CheckAnalogIn(int, float, float);
bool MiscKeyboardChecks();
bool AlarmTestFns();
bool CheckSecurity();

void TestBallFns();
void TestGenFns();
void TestTunacalFns();
void TestNumericalFns();
void TestTimers();


bool bFirstTime = false;
bool bKillDoLampsLooping = false;
bool bKillTaskLoopTest = false;
bool ControllerPassFail(bool);
void InitObjects();
void TurnAllDigitalOutputs(int, bool);
void TurnAllAnalogOutputs(int, float);
void DisplayInitialProcessScreen();
void DL(CString, CString, CString, long, double, CString);
void DM(CString, CString, CString, long, double, CString);
CString DE(CString, CString, CString, long, double, CString);	//this returns a string of the string entered in the get entry box
void Pause();
void PrintDateLog();
void PrintTimeLog();
void WaitForEnterKey(bool);
void PidTestStart(int, float);
void PidTestStop(int);
bool SecurityCheck(bool);
void PrintProdType(int iProdNumfn);
void PrintMemoryInfo();
void CheckTimeDiffLampsLoop();
void DOLampsLooping();


//global variables

bool bFnRunning = false;
bool bF8Running = false;
bool BKillTheTask = false;
bool brecipevalidflag = false;
bool bOkFlag = false;
bool bOkFlag2 = false;
bool ControllerPassOrFailFlag = true, bEndTest = false;
long lOverRideChoice = 0;
long lNewCycleLength = 0;
long lNewCycleNumber = 0;
long lBallRecipeNo = 92;
long lNumericalRecipeNo = 15;

CString csVar1;
CString	strChangeCycleEntry = "";
CString	strNewCycleLength = "";
CString	strChangeCycleNumber = "";
CString	strNewCycleNumber = "";

float flIT = 0.0;
int iProductNumber = 0;
int iTaskLoopTestCount = 0;
int icyclecounter = 0;
bool bHideDisplays = false;
int iDisp = 0;
long lcount = 0;
long lcount1 = 0;

typedef enum Override
{
	MainStartLBL = 1,
	ControllerTestLBL = 2,
	FnTestLBL = 3,
	FnTestLBL2 = 4,
	TaskLoopLBL = 5,
	RecipeSEL = 6,
	PIDTestLBL = 7,
	LampsTimeTestLBL = 8,
} Override;

Override OverrideSelection;

#define GoTo(OverrideSelection) {switch (OverrideSelection) {\
					case RecipeSEL:						goto RecipeSEL;\

enum LogStatus
{
	NORMAL = 1,
	DEVIATION = 2,
	LOGERROR = 3,
	ABORT = 4,
	OVERRIDE = 5,
	MANUAL = 6
};

// MDM add IO for testing 
/*==================== Definition of Analog Input (AI) Channels ====================*/
#define AI_ROT_POS					9		/*AI 9 */		//FROM DEVICENET ENCODER
#define AI_ROT_SPD					10		/*AI 10 */

#define AI_RTD1						1		/*AI 1 */
#define AI_PREHT_RTD				2		/*AI 2 */
#define AI_INCOMING_WTR_TEMP		3		/*AI 3 */	//Not used on North American Pilot
#define AI_PRESSURE					4		/*AI 4 */	
#define AI_LEVEL					5		/*AI 5 */
//#define SPARE						6		/*AI 6 */
//#define SPARE						7		/*AI 7 */
#define AI_FLOW						8		/*AI 8 */

/*==================== Definition of Analog Output (AO) Channels ==================*/
#define AO_STEAM					1		/*AO 1 */
#define AO_VENT						2		/*AO 2 */
#define AO_AIR						3		/*AO 3 */
#define AO_WATER					4		/*AO 4 */
#define AO_DRAIN					5		/*AO 5 */
#define	AO_SPEED_CHART				6		/*AO 6 */	
#define AO_ROT_DRIVE				7		/*AO 7 */
#define AO_SPLT_RNG					8		/*AO 8 */

/*==================== Definition of Digital Input (DI) Channels ==================*/
#define DI_ESTOP_PB					1		/*DI 1 */
#define DI_DOOR_OPEN				2		/*DI 2 */
#define DI_DOOR_PIN_EXT				3		/*DI 3 */
#define DI_ZERO_PRS					4		/*DI 4 */
#define DI_CIRC_PUMP_OL				5		/*DI 5 */
//#define SPARE						6		/*DI 6 */
#define DI_AIR_OK					7		/*DI 7 */
#define DI_STEAM_OK					8		/*DI 8 */
#define DI_WATER_OK					9		/*DI 9 */
#define DI_JOG_CCW_SW				10		/*DI 10*/
#define DI_JOG_CW_SW				11		/*DI 11*/
//#define SPARE						12		/*DI 12*/
#define DI_ROT_DRV_OK				13		/*DI 13*/	
#define DI_ROT_OL_OK				14		/*DI 14*/
#define UPS_ALARM_OK				15		/*DI 15*/	
#define ALARM_ACK_PB				16		/*DI 16*/

/*=================== Definition of Digital Output (DO) Channels ==================*/

#define DO_FILL_VLV					1		/*DO 1 */
#define DO_ROT_DRV_STOP				2		/*DO 2 */
#define DO_HT_EXCH					3		/*DO 3 */
#define DO_LWR_RECIRC_VLV			4		/*DO 4 */	//Defaults to Upr circulation (if OFF)
#define LOGTECAlarm					5		/*DO 5 */
#define ALARM_ACK_LIGHT				6		/*DO 6 */
#define DO_ROT_RUN_CW				7		/*DO 7 */
#define DO_ROT_RUN_CCW				8		/*DO 8 */	
#define DO_ROT_JOG_CW				9		/*DO 9 */
#define DO_ROT_JOG_CCW				10		/*DO 10 */	
#define DO_PREHT_VLV				11		/*DO 11 */
#define ESTOP_FDBACK				12		/*DO 12 */	
#define DO_ROT_BRAKE_REL			13		/*DO 13 */
#define DO_PUMP						14		/*DO 14 */
#define DO_SFTY_RLY_RST				15		/*DO 15 */
#define DO_BLEEDER					16		/*DO 16 */


LogStatus	SendLogStatus;
//=====================================================================
//==================== Main Program Script begins =====================
//=====================================================================
void StartScript()
{
	/* Initiate the Objects Do not delete this */
	InitObjects(); //located in the header wrecord.h
	MyInit();
	Profiler.Init();

StartLabel:

	Display.OverrideText(1, L"Go To Start", MainStartLBL);
	Display.OverrideText(2, L"Go To Controller Test", ControllerTestLBL);
	Display.OverrideText(3, L"Go To Function Test", FnTestLBL);
	Display.OverrideText(4, L"Go To Logs Test", FnTestLBL2);
	Display.OverrideText(5, L"Go to TaskLoopTest", TaskLoopLBL);
	Display.OverrideText(6, L"Go To Trending Data Test", RecipeSEL);
	Display.OverrideText(7, L"Go To PID Test", PIDTestLBL);
	Display.OverrideText(8, L"Go to Lamps Looping Test", LampsTimeTestLBL);
	SendLogStatus = MANUAL;


	//Display.OverrideEnable();

	MainScript();
	Pause();
	int q = 0;
	q = (int)"l";
	goto StartLabel;

}	/* End of Main Program Script */


void MyInit()
{
	csVar1 = Display.GetProgramName();
	DisplayInitialProcessScreen();
	Display.Display(16, L" Cycle Number ", L"CycleNo");
	Display.Display(15, L"Phase Time", L"PhaseTime");
	Display.Display(14, L"Phase", L"PhaseName");
	Display.Display(13, L"Prog", csVar1);
	//	Display.WriteCurrentCycleNumber(1);
	Display.SetAlarmChannel(32);




};
//################################################################################################################
//################################################################################################################
//################################################################################################################
//################################################################################################################
//Main Program start.
void MainScript()
{

MainStartLBL:

	Display.KillEntry();
	CString strVarCycle = L"";
	bEndTest = false;

	//formats the cycle number to be printed
	strVarCycle.Format(L"%-d", Display.ReadCurrentCycleNumber());
	//	Display.Log(L"Cycle Number",strVarCycle,SendLogStatus);

	Display.Phase(L"Start");

	Display.OverrideEnable();

	Display.Display(20, L"JBT FoodTech");
	Display.Display(21, L"LOG-TEC Momentum System");
	Display.Display(22, L"OverRide Testing Area");
	Display.Display(23, L"");
	Display.DisplayStatus(L"Please Select an OVERRIDE...");
	Display.Entry(0, 0, TRUE);

	do
	{
		if (Display.IsOverRideScreenLaunched() == TRUE)
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{
				/*if(lOverRideChoice == PIDTestLBL)
				{
					Display.Log(L"REBOOT Called",L"");
//					Sleep(250);
					Display.EndControl();
					// ****** REBOOT NOW!!! SHOULD NOT GET PAST THIS!
					 __asm
					{
						cli                     ; Disable Interrupts
						mov     al, 0FEh        ; Reset CPU
						out     064h, al
						jmp     near $          ; Should not get here
					}
					// ****** DONE SHOULD NOT GET HERE!
				}
				else
				{*/
				BKillTheTask = true; //means for task to start killing itself
				Display.Log(L"prog start over ride", L"", SendLogStatus);
				while (bFnRunning) //waits for task to die.
					Sleep(250);
				//	Display.Log(L"bFnRunning=false now",L"",SendLogStatus);
				TaskLoop.KillTask(1);
				//	Display.Log(L"Spawntask 1 Terminated.","",SendLogStatus);  /
				Display.KillEntry();
				goto OverRideArea;
				//	}
			}
		}
		Sleep(1000);


		lcount++;
		lcount1 = lcount - 1;
		//Display.DisplayVariable(6, L"Counter1", L"LONG", (long)& lcount);
		//Display.DisplayVariable(7, L"Counter1", L"LONG", (long)& lcount1);

		/*Sleep(20000);

		if (bHideDisplays)
		{
			bHideDisplays = false;
			//show the displays
			DisplayInitialProcessScreen();
		}
		else
		{
			bHideDisplays = true;
			for (iDisp = 1; iDisp < 13; iDisp++)
				Display.HideDisplay(iDisp);
		}*/


	} while (true);
	//	}while (!Display.IsEnterKeyPressed());

	// RIP Added Lamps Looper Time test *****************************************

LampsTimeTestLBL: // Override #8

	Display.KillEntry();

	SendLogStatus = OVERRIDE;
	bF8Running = true;
	Display.Log(L"Begin Digital Lamps Timer Test", strVarCycle, SendLogStatus);
	//Sleep(250);

	TaskLoop.SpawnTask((LONG)ControllerCheck, 3);	//performs the checks on the controller.
	Sleep(250);

	//TaskLoop.SpawnTask((LONG) CheckTimeDiffLampsLoop, 6 );
//TaskLoop.SetThreadPriority(6, 251);
	Sleep(250);
	TaskLoop.SpawnTask((LONG)DOLampsLooping, 7);
	//TaskLoop.SetThreadPriority(7, 251);

	do
	{

		if (Display.IsOverRideScreenLaunched() == TRUE)
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{

				BKillTheTask = true; //means for task to start killing itself
				while (bF8Running) //waits for task to die.
					Display.KillEntry();
				goto OverRideArea;
			}
		}
		Sleep(1000);
		Display.Log(L"Testing...1..2..3...");

	} while (bF8Running);

	TaskLoop.KillTask(6);
	TaskLoop.KillTask(7);


	//////////////////////////////////////////////////////////////////////////////////////////////////
							/*PID Test Label*/
	//////////////////////////////////////////////////////////////////////////////////////////////////
PIDTestLBL:

	Display.KillEntry();
	bFnRunning = true;
	Display.Log(L"Begin PID Test");

	TaskLoop.SpawnTask((LONG)PIDTest, 7);
	Display.OverrideDisable();

	do
	{
		Sleep(1000);
	} while (bFnRunning);

	TaskLoop.KillTask(1);




	//////////////////////////////////////////////////////////////////////////////////////////////////
							/*Controller Test Label*/
	//////////////////////////////////////////////////////////////////////////////////////////////////
ControllerTestLBL:

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Logging Phase
	Display.KillEntry();
	Display.Phase(L"Logging");

	SendLogStatus = DEVIATION;

	bFnRunning = true;


	Display.OverrideText(1, L"Go To Start", MainStartLBL);
	Display.OverrideText(2, L"Go To ControllerTest", ControllerTestLBL);
	Display.OverrideText(3, L"Go To Function Test", FnTestLBL);
	Display.OverrideText(4, L"Go To Logs Test", FnTestLBL2);
	Display.OverrideText(5, L"Go To Task Loop Test", TaskLoopLBL);
	Display.OverrideText(6, L"", -1);
	Display.OverrideText(7, L"Go To PID Test", PIDTestLBL);
	Display.OverrideText(8, L"", -1);
	SendLogStatus = MANUAL;

	TaskLoop.SpawnTask((LONG)TestTimers, 3);
	//TaskLoop.SpawnTask((LONG)ControllerCheck, 3);	//performs the checks on the controller. //MDM uncomment this line

	do
	{
		if (Display.IsOverRideScreenLaunched())
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{

				BKillTheTask = true; //means for task to start killing itself
				while (bFnRunning) //waits for task to die.
					Sleep(250);
				TaskLoop.KillTask(1);
				Display.KillEntry();
				Display.Log(L"got here 1");
				goto OverRideArea;
			}
		}
		Sleep(1000);
		Display.Log(L"got here 2");
	} while (bFnRunning);

	TaskLoop.KillTask(1);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


FnTestLBL:
	Display.KillEntry();
	SendLogStatus = LOGERROR;
	bFnRunning = true;
	ControllerPassOrFailFlag = true; //sets flag to true to run test.
	TaskLoop.SpawnTask((LONG)FnTest, 1);	//performs the checks on the controller.
	do
	{
		if (Display.IsOverRideScreenLaunched() == TRUE)
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{

				BKillTheTask = true; //means for task to start killing itself
				while (bFnRunning) //waits for task to die.
					Sleep(250);
				TaskLoop.KillTask(1);
				Display.KillEntry();
				goto OverRideArea;
			}
		}
		Sleep(250);

	} while (bFnRunning);

	TaskLoop.KillTask(1);



	PidTestStart(8, 50);				//starts 8 PIDS with a setpoint of 50.
//	DM("8 pids with setpoint","of 50 started","",NULL,NULL,"press enter to stop them");



FnTestLBL2:

	Display.KillEntry();

	SendLogStatus = OVERRIDE;
	bFnRunning = true;
	strVarCycle.Format(L"%-d", Display.ReadCurrentCycleNumber());
	Display.Log(L"Cycle Number", strVarCycle, SendLogStatus);
	TaskLoop.SpawnTask((LONG)ShootSomeLogs, 1);	//performs the checks on the controller.
	do
	{

		if (Display.IsOverRideScreenLaunched() == TRUE)
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{

				BKillTheTask = true; //means for task to start killing itself
				while (bFnRunning) //waits for task to die.
					Sleep(250);
				TaskLoop.KillTask(1);
				Display.KillEntry();
				goto OverRideArea;
			}
		}
		Sleep(250);

	} while (bFnRunning);

	TaskLoop.KillTask(1);

	//goto FnTestLBL2; //does shoot some logs indefinitly for the 2 day stability test.

	Display.KillEntry();

	PidTestStop(8);					// stops 8 pids

	TurnAllAnalogOutputs(8, 0);		//turns 8 analog outputs to 0% off.
	TurnAllDigitalOutputs(32, 0);	//turns 32 digitial outputs to off

	Display.Phase(L"");
	PrintDateLog();
	PrintTimeLog();
	Display.EndControl();


	BKillTheTask = false;
	ControllerPassOrFailFlag = true;

	if (!bEndTest) goto FnTestLBL;
	else goto MainStartLBL;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
							/*Trending Data Test Label*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
RecipeSEL:

	Display.KillEntry();

	Display.Log(L"Trending Data Test is Running");
	Sleep(1000);

	while (!brecipevalidflag)
	{
		Display.ResetProduct();
		Display.Display(0, L"Please Select the Correct Recipe From the List");
		iProductNumber = Display.GetProduct();
		while (!Display.IsEnterKeyPressed())
		{
			Sleep(300);
		}

		if (iProductNumber <= 0)
		{
			brecipevalidflag = false;
			//Display current selection on the controller screen for verification
			//======================================================================
			Display.KillEntry();
			Display.Display(20, L"-INVALID RECIPE-");
			Display.Display(21, L" Recipe Number");
			Display.Display(22, L"Is Not Allowed");
			Display.Display(23, L"On This Machine");

			Display.DisplayStatus(L"Press Enter To Select New Recipe...");
			Display.Entry(0, 0);

			while (!Display.IsEnterKeyPressed())
			{
				Sleep(300);
			}
		}
		else
		{
			brecipevalidflag = true;
		}

		Sleep(250);
	}

	PV.SetProduct(Display.GetProduct(0, 1));
	Sleep(1000);
	Profiler.StartProfiler();
	Sleep(1000);
	PinesTime100.StartTimer();
	PinesTime100.ZeroTime();
	TaskLoop.SpawnTask((LONG)DOLampsLooping, 7);

	do
	{
		//Display current selection on the controller screen for verification
		//======================================================================
		Display.KillEntry();
		Display.Display(20, L"Enter Test Cycle Time");
		Display.Display(21, L" in Seconds");
		Display.Display(22, L"");
		Display.Display(23, L"");

		Display.DisplayStatus(L"waiting...");
		Display.Entry(3, 0);

		while (!Display.IsEnterKeyPressed())
		{
			Sleep(300);
		}

		lNewCycleLength = (long)Display.GetEnteredData(3);
		strNewCycleLength.Format(L"%-d", lNewCycleLength);

		Display.Display(21, L"Is this New Cycle Time correct?");
		Display.Display(22, strNewCycleLength);
		Display.Display(23, L"");
		Display.DisplayStatus(L"Please Enter Y or N...");
		Display.Entry(2, 0);
		while (!Display.IsEnterKeyPressed())
			Sleep(250);

		strChangeCycleEntry = Display.GetEnteredString(2);

		if ((strChangeCycleEntry.CompareNoCase(L"Y") == 0) && (lNewCycleLength > 0))
		{
			bOkFlag = true;
		}
		Sleep(250);

	} while (!bOkFlag);

	do
	{
		//Display current selection on the controller screen for verification
		//======================================================================
		Display.KillEntry();
		Display.Display(20, L"Enter Number of");
		Display.Display(21, L" test cycle loops");
		Display.Display(22, L" (Enter '0' for Endless)");
		Display.Display(23, L"");

		Display.DisplayStatus(L"waiting...");
		Display.Entry(3, 0);

		while (!Display.IsEnterKeyPressed())
		{
			Sleep(300);
		}

		lNewCycleNumber = (long)Display.GetEnteredData(3);
		strNewCycleNumber.Format(L"%-d", lNewCycleNumber);

		Display.Display(21, L"Is Cycle Loop Count correct?");
		Display.Display(22, strNewCycleNumber);
		Display.Display(23, L"");
		Display.DisplayStatus(L"Please Enter Y or N...");
		Display.Entry(2, 0);
		while (!Display.IsEnterKeyPressed())
			Sleep(250);

		strChangeCycleEntry = Display.GetEnteredString(2);

		if ((strChangeCycleEntry.CompareNoCase(L"Y") == 0))
		{
			bOkFlag2 = true;
		}
		Sleep(250);

	} while (!bOkFlag2);


	// We now have the questions answered  
	// so now loop using these values
	//Display.Phase(L"Trending");
	csVar1.Format(L"Loop#%d", icyclecounter);
	Display.Phase(csVar1);

	if (lNewCycleNumber > 0L)
	{
		csVar1.Format(L"BEGIN:Trend Test Looping %d times", lNewCycleNumber);
	}
	else
	{
		csVar1.Format(L"BEGIN:Trend Test Looping Endlessly...");
	}

	Display.Log(csVar1);

	do
	{
		if (Display.IsOverRideScreenLaunched() == TRUE)
		{
			if ((lOverRideChoice = Display.GetOverRideKey()) != -1)
			{

				BKillTheTask = true; //means for task to start killing itself
				while (bF8Running) //waits for task to die.
					Display.KillEntry();
				goto OverRideArea;
			}
		}


		if (PinesTime100.GetElapsedTime() > lNewCycleLength)
		{
			Profiler.StopProfiler();
			Sleep(3000);
			Display.EndControl();
			csVar1.Format(L"Trending Loop Cycle %d", icyclecounter++);
			Display.Log(csVar1);

		}

		if (PinesTime100.GetElapsedTime() > (lNewCycleLength + 1))
		{
			// Display.Log(L"New Trending Cycle");
			PinesTime100.ZeroTime();
			csVar1.Format(L"Loop#%d", icyclecounter);
			Display.Phase(csVar1);
			PV.SetProduct(Display.GetProduct(0, 1));
			Sleep(1000);
			Profiler.StartProfiler();
		}

		Sleep(1000); // Check every second

	} while ((icyclecounter <= lNewCycleNumber) || (lNewCycleNumber == 0));

	csVar1.Format(L"Finished %d Trend Data Test Cycles", icyclecounter);
	Display.Log(csVar1);


	TaskLoop.KillTask(6);
	TaskLoop.KillTask(7);
	Profiler.StopProfiler();
	Sleep(5000);
	Display.EndControl();

	goto MainStartLBL;


OverRideArea:

	switch (lOverRideChoice)
	{
	case 1:
		//	Display.Log(L"goto MainStartLBL;",L"1");
		goto MainStartLBL;
	case 2:
		//	Display.Log(L"goto ControllerTestLBL;",L"2");
		goto ControllerTestLBL;
	case 3:
		//	Display.Log(L"goto FnTestLBL;",L"3");
		goto FnTestLBL;
	case 4:
	{
		bEndTest = true;
		//	Display.Log(L"goto TaskLoopLBL;",L"4");
		goto FnTestLBL2;
	}
	case 5:
		//	Display.Log(L"goto FnTestLBL2;",L"5");
		goto TaskLoopLBL;
	case 6:
		//	Display.Log(L"goto MainStartLBL;",L"6");
		goto RecipeSEL;
	case 7:
		//	Display.Log(L"goto MainStartLBL;",L"7");
		goto PIDTestLBL; //MainStartLBL;
	case 8:
		//	Display.Log(L"goto MainStartLBL;",L"8");
		goto LampsTimeTestLBL;
	default:
		//	Display.Log(L"DEFAULT: goto MainStartLBL;",L"Default");
		goto MainStartLBL;

	}//end of override selection

	DM("Program Terminated", "Abnormally Via", "Incorrect OverRide", NULL, NULL, "Press enter to restart");
	//////////////////////////////////////////////////////////////////////////////////////////////////
							/*TaskLoop Test Label*/
	//////////////////////////////////////////////////////////////////////////////////////////////////
TaskLoopLBL:

	Display.KillEntry();
	bFnRunning = true;
	bKillTaskLoopTest = false;
	bKillDoLampsLooping = false;
	Display.Log(L"Begin Task Loop Test");
	do
	{
		TaskLoop.SpawnTask((LONG)TaskLoopTest, 5);
		TaskLoop.SpawnTask((LONG)DOLampsLooping, 7);
		iTaskLoopTestCount++;
		Display.OverrideDisable();

		while (!bKillTaskLoopTest && !bKillDoLampsLooping)
		{
			Sleep(500);
		}
		Sleep(1000);
		bKillTaskLoopTest = false;
		bKillDoLampsLooping = false;
	} while (bFnRunning);

	TaskLoop.KillTask(5);
	TaskLoop.KillTask(7);



};//end of MainScript();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////	
void TaskLoopTest()
{
	CString strNumberTaskLoops = L"";
	int iDO_Light_1 = 0;
	Display.Log(L"Begin Light Counting");

	do
	{
		if (DigitalIO.ReadDigitalOut(1))
		{
			iDO_Light_1++;
		}
		Sleep(500);

	} while (iDO_Light_1 < 10);

	strNumberTaskLoops.Format(L"%d", iTaskLoopTestCount);

	Display.Log(L"Number of times Task Loop has Launched", strNumberTaskLoops);
	bKillTaskLoopTest = true;
	bKillDoLampsLooping = true;

	TaskLoop.KillTask(5);
	TaskLoop.KillTask(7);

}

//////////////////////////////////////////////////////////////////////////////////////////////		
//////////////////////////////////////////////////////////////////////////////////////////////		
//////////////////////////////////////////////////////////////////////////////////////////////	

void PIDTest()
{
	int		a = 0,
		b = 0;

	long	TimeElapsed;

	bool	bReadyForProcessFlag = false,
		bPositionProcessFlag = false,
		bAlreadyDoneItOnceFlag = false,
		bCheckElapsedTimeFlag = false,

		bReadyForProcess2Flag = false,
		bPositionProcess2Flag = false,
		bAlreadyDoneItOnce2Flag = false,
		bCheckElapsedTime2Flag = false,

		bReadyForProcess3Flag = false,
		bPositionProcess3Flag = false,
		bAlreadyDoneItOnce3Flag = false,
		bCheckElapsedTime3Flag = false,

		bReadyForProcess4Flag = false,
		bPositionProcess4Flag = false,
		bAlreadyDoneItOnce4Flag = false,
		bCheckElapsedTime4Flag = false,

		bReadyForProcess5Flag = false,
		bPositionProcess5Flag = false,
		bAlreadyDoneItOnce5Flag = false,
		bCheckElapsedTime5Flag = false,

		bReadyForProcess6Flag = false,
		bPositionProcess6Flag = false,
		bAlreadyDoneItOnce6Flag = false,
		bCheckElapsedTime6Flag = false,

		bReadyForProcess7Flag = false,
		bPositionProcess7Flag = false,
		bAlreadyDoneItOnce7Flag = false,
		bCheckElapsedTime7Flag = false,

		bReadyForProcess8Flag = false,
		bPositionProcess8Flag = false,
		bAlreadyDoneItOnce8Flag = false,
		bCheckElapsedTime8Flag = false,

		bDoneOnce = false;
	CString csDisplayTimeElapsed = L"";
	Display.OverrideDisable();

	while (bFnRunning)
	{
		////////////////////////
		bReadyForProcessFlag = false;
		bPositionProcessFlag = false;
		bAlreadyDoneItOnceFlag = false;
		bCheckElapsedTimeFlag = false;
		bReadyForProcess2Flag = false;
		bPositionProcess2Flag = false;
		bAlreadyDoneItOnce2Flag = false;
		bCheckElapsedTime2Flag = false;

		bReadyForProcess3Flag = false;
		bPositionProcess3Flag = false;
		bAlreadyDoneItOnce3Flag = false;
		bCheckElapsedTime3Flag = false;

		bReadyForProcess4Flag = false;
		bPositionProcess4Flag = false;
		bAlreadyDoneItOnce4Flag = false;
		bCheckElapsedTime4Flag = false;

		bReadyForProcess5Flag = false;
		bPositionProcess5Flag = false;
		bAlreadyDoneItOnce5Flag = false;
		bCheckElapsedTime5Flag = false;

		bReadyForProcess6Flag = false;
		bPositionProcess6Flag = false;
		bAlreadyDoneItOnce6Flag = false;
		bCheckElapsedTime6Flag = false;

		bReadyForProcess7Flag = false;
		bPositionProcess7Flag = false;
		bAlreadyDoneItOnce7Flag = false;
		bCheckElapsedTime7Flag = false;

		bReadyForProcess8Flag = false;
		bPositionProcess8Flag = false;
		bAlreadyDoneItOnce8Flag = false;
		bCheckElapsedTime8Flag = false;
		bDoneOnce = false;
		////////////////////////
		Display.Log(L"Ramp up");


		for (a = 1; a < 9; a++)
		{
			PID.RampSetpointPeriod(a, 80, 60);
			PID.PIDStart(a, a, 0);
		}

		a = 0;

		PinesTime2.StartTimer();
		PinesTime2.ZeroTime();

		while (PinesTime2.GetElapsedTime() < 120)
		{
			// PID 1

			if (PID.GetPIDSetPoint(1) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcessFlag)
			{
				Display.Alarm(2, 1, 16, L"PID 1 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcessFlag = true;

			}

			if (PID.GetPIDSetPoint(1) > 80 && !bAlreadyDoneItOnceFlag)
			{
				Display.Alarm(2, 1, 16, L"PID 1 went past 80!", SendLogStatus);
				bAlreadyDoneItOnceFlag = true;

			}
			if (PID.GetPIDSetPoint(1) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTimeFlag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 1 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTimeFlag = true;
			}

			// PID 2

			if (PID.GetPIDSetPoint(2) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess2Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 2 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess2Flag = true;

			}

			if (PID.GetPIDSetPoint(2) > 80 && !bAlreadyDoneItOnce2Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 2 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce2Flag = true;

			}
			if (PID.GetPIDSetPoint(2) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime2Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 2 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime2Flag = true;
			}

			// PID 3

			if (PID.GetPIDSetPoint(3) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess3Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 3 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess3Flag = true;

			}
			if (PID.GetPIDSetPoint(3) > 80 && !bAlreadyDoneItOnce3Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 3 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce3Flag = true;

			}
			if (PID.GetPIDSetPoint(3) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime3Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 3 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime3Flag = true;
			}

			// PID 4

			if (PID.GetPIDSetPoint(4) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess4Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 4 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess4Flag = true;

			}
			if (PID.GetPIDSetPoint(4) > 80 && !bAlreadyDoneItOnce4Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 4 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce4Flag = true;

			}
			if (PID.GetPIDSetPoint(4) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime4Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 4 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime4Flag = true;
			}

			// PID 5

			if (PID.GetPIDSetPoint(5) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess5Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 5 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess5Flag = true;

			}
			if (PID.GetPIDSetPoint(5) > 80 && !bAlreadyDoneItOnce5Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 5 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce5Flag = true;

			}
			if (PID.GetPIDSetPoint(5) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime5Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 5 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime5Flag = true;
			}

			// PID 6

			if (PID.GetPIDSetPoint(6) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess6Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 6 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess6Flag = true;

			}
			if (PID.GetPIDSetPoint(6) > 80 && !bAlreadyDoneItOnce6Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 6 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce6Flag = true;

			}
			if (PID.GetPIDSetPoint(6) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime6Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 6 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime6Flag = true;
			}
			// PID 7

			if (PID.GetPIDSetPoint(7) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess7Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 7 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess7Flag = true;

			}
			if (PID.GetPIDSetPoint(7) > 80 && !bAlreadyDoneItOnce7Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 7 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce7Flag = true;

			}
			if (PID.GetPIDSetPoint(7) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime7Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 7 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime7Flag = true;
			}
			// PID 8

			if (PID.GetPIDSetPoint(8) < 80 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess8Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 8 took longer than 60 sec to reach 80!", SendLogStatus);
				bReadyForProcess8Flag = true;

			}
			if (PID.GetPIDSetPoint(8) > 80 && !bAlreadyDoneItOnce8Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 8 went past 80!", SendLogStatus);
				bAlreadyDoneItOnce8Flag = true;

			}
			if (PID.GetPIDSetPoint(8) == 80 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime8Flag)
			{
				a++;
				//Display.Alarm(2, 1, 16 , L"PID 8 Setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime8Flag = true;
			}

			if (!bDoneOnce && a == 8)
			{
				bDoneOnce = true;
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				Display.Log(L"Time Elapsed to meet 80 set point :", csDisplayTimeElapsed);
			}


			Sleep(75);
		}

		bReadyForProcessFlag = false;
		bPositionProcessFlag = false;
		bAlreadyDoneItOnceFlag = false;
		bCheckElapsedTimeFlag = false;
		bReadyForProcess2Flag = false;
		bPositionProcess2Flag = false;
		bAlreadyDoneItOnce2Flag = false;
		bCheckElapsedTime2Flag = false;

		bReadyForProcess3Flag = false;
		bPositionProcess3Flag = false;
		bAlreadyDoneItOnce3Flag = false;
		bCheckElapsedTime3Flag = false;

		bReadyForProcess4Flag = false;
		bPositionProcess4Flag = false;
		bAlreadyDoneItOnce4Flag = false;
		bCheckElapsedTime4Flag = false;

		bReadyForProcess5Flag = false;
		bPositionProcess5Flag = false;
		bAlreadyDoneItOnce5Flag = false;
		bCheckElapsedTime5Flag = false;

		bReadyForProcess6Flag = false;
		bPositionProcess6Flag = false;
		bAlreadyDoneItOnce6Flag = false;
		bCheckElapsedTime6Flag = false;

		bReadyForProcess7Flag = false;
		bPositionProcess7Flag = false;
		bAlreadyDoneItOnce7Flag = false;
		bCheckElapsedTime7Flag = false;

		bReadyForProcess8Flag = false;
		bPositionProcess8Flag = false;
		bAlreadyDoneItOnce8Flag = false;
		bCheckElapsedTime8Flag = false;
		bDoneOnce = false;

		PinesTime2.ZeroTime();
		Display.Log(L"Ramp down");


		for (b = 1; b < 9; b++)
			PID.RampSetpointPeriod(b, 10, 60);

		b = 0;


		while (PinesTime2.GetElapsedTime() < 120)
		{
			// PID 1

			if (PID.GetPIDSetPoint(1) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcessFlag)
			{
				Display.Alarm(2, 1, 16, L"PID 1 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcessFlag = true;

			}

			if (PID.GetPIDSetPoint(1) < 10 && !bAlreadyDoneItOnceFlag)
			{
				Display.Alarm(2, 1, 16, L"PID 1 went below 10!", SendLogStatus);
				bAlreadyDoneItOnceFlag = true;

			}

			if (PID.GetPIDSetPoint(1) == 10 && PinesTime2.GetElapsedTime() > 60 && !bCheckElapsedTimeFlag)
			{
				b++;
				//Display.Alarm(2, 1, 16 , L"PID Setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				Display.Log(L"Time Elapsed for PID 1 to meet set point :", csDisplayTimeElapsed);
				bCheckElapsedTimeFlag = true;
			}

			// PID 2

			if (PID.GetPIDSetPoint(2) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess2Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 2 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess2Flag = true;

			}

			if (PID.GetPIDSetPoint(2) < 10 && !bAlreadyDoneItOnce2Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 2 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce2Flag = true;

			}

			if (PID.GetPIDSetPoint(2) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime2Flag)
			{

				b++;
				//Display.Alarm(2, 1, 16 , L"PID 2 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime2Flag = true;
			}



			// PID 3

			if (PID.GetPIDSetPoint(3) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess3Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 3 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess3Flag = true;

			}

			if (PID.GetPIDSetPoint(3) < 10 && !bAlreadyDoneItOnce3Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 3 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce3Flag = true;

			}

			if (PID.GetPIDSetPoint(3) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime3Flag)
			{

				b++;
				//Display.Alarm(2, 1, 16 , L"PID 3 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime3Flag = true;
			}


			// PID 4

			if (PID.GetPIDSetPoint(4) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess4Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 4 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess4Flag = true;

			}

			if (PID.GetPIDSetPoint(4) < 10 && !bAlreadyDoneItOnce4Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 4 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce4Flag = true;

			}

			if (PID.GetPIDSetPoint(4) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime4Flag)
			{
				b++;
				//Display.Alarm(2, 1, 16 , L"PID 4 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime4Flag = true;
			}



			// PID 5

			if (PID.GetPIDSetPoint(5) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess5Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 5 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess5Flag = true;

			}

			if (PID.GetPIDSetPoint(5) < 10 && !bAlreadyDoneItOnce5Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 5 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce5Flag = true;

			}

			if (PID.GetPIDSetPoint(5) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime5Flag)
			{
				b++;
				//Display.Alarm(2, 1, 16 , L"PID 5 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime5Flag = true;
			}

			// PID 6

			if (PID.GetPIDSetPoint(6) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess6Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 6 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess6Flag = true;

			}

			if (PID.GetPIDSetPoint(6) < 10 && !bAlreadyDoneItOnce6Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 6 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce6Flag = true;

			}

			if (PID.GetPIDSetPoint(6) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime6Flag)
			{

				b++;
				//Display.Alarm(2, 1, 16 , L"PID 6 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime6Flag = true;
			}


			// PID 7

			if (PID.GetPIDSetPoint(7) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess7Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 7 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess7Flag = true;

			}

			if (PID.GetPIDSetPoint(7) < 10 && !bAlreadyDoneItOnce7Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 7 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce7Flag = true;

			}

			if (PID.GetPIDSetPoint(7) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime7Flag)
			{
				b++;
				//Display.Alarm(2, 1, 16 , L"PID 7 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime7Flag = true;
			}


			// PID 8

			if (PID.GetPIDSetPoint(8) > 10 && PinesTime2.GetElapsedTime() > 60 && !bReadyForProcess8Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 8 took longer than 60 sec to reach 10!", SendLogStatus);
				bReadyForProcess8Flag = true;

			}

			if (PID.GetPIDSetPoint(8) < 10 && !bAlreadyDoneItOnce8Flag)
			{
				Display.Alarm(2, 1, 16, L"PID 8 went below 10!", SendLogStatus);
				bAlreadyDoneItOnce8Flag = true;

			}

			if (PID.GetPIDSetPoint(8) == 10 && PinesTime2.GetElapsedTime() >= 60 && !bCheckElapsedTime8Flag)
			{
				b++;
				//Display.Alarm(2, 1, 16 , L"PID 8 setpoint was reached", SendLogStatus);
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				//	Display.Log(L"Time Elapsed to meet set point :",csDisplayTimeElapsed);
				bCheckElapsedTime8Flag = true;
			}

			if (!bDoneOnce && b == 8)
			{
				bDoneOnce = true;
				TimeElapsed = PinesTime2.GetElapsedTime();
				csDisplayTimeElapsed.Format(L"%-d", TimeElapsed);
				Display.Log(L"Time Elapsed to meet 10 set point :", csDisplayTimeElapsed);

			}

			Sleep(75);
		}
	}

	PID.PIDStop(1, TRUE);
	PID.PIDStop(2, TRUE);
	bReadyForProcessFlag = false;
	bPositionProcessFlag = false;
	bAlreadyDoneItOnceFlag = false;
	//Display.Log(L"Start Label");


}
//////////////////////////////////////////////////////////////////////////////////////////////		
//////////////////////////////////////////////////////////////////////////////////////////////		
//////////////////////////////////////////////////////////////////////////////////////////////		


void ControllerCheck()
{
	Display.Log(L"Controller Check Started", L"", SendLogStatus);
	bFnRunning = true;
	Display.OverrideDisable();

	CString strVarEntry = L"",
		strYNVar = L"";
	bool	bLoopFlag = true;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Serial Number Prompt

		/*do
		{

			strVarEntry=DE("","","Please Enter the Controllers Serial Number",NULL,NULL,"Please enter serial Number then press enter.");
			int iSerialNum=(int)Display.GetEnteredData(3); //gets the serial number.
			strYNVar=DE("","","Is This Serial Number Correct?",iSerialNum,NULL,"Please enter Y/N then press enter.");
			if	(!(strYNVar.CompareNoCase(L"y")))
				bLoopFlag=false;	//means they entered y if anything other then yes then prompt for serial number again.

		}while(bLoopFlag);

	Display.Log(L"Controllers Serial Number",strVarEntry,SendLogStatus);

	//	TestTimers();

	//	ControllerPassFail(AlarmTestFns());

		ControllerPassFail(CheckSecurity());

			//this does the keyboard check using the character array.
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Keboard Check
		const int istrArraySize=4;
		CString strArray[istrArraySize]={	L"1234567890",
											L"qwertyuiop",
											L"asdfghjkl",
											L"zxcvbnm"};
	ControllerPassOrFailFlag=true;
		for (int i=0;i<= (istrArraySize-1);i++) //the reason for the arraysize minus 1 is for array starting at 0
			ControllerPassFail(CheckKeyboard(strArray[i]));

		ControllerPassFail(MiscKeyboardChecks()); */ // MDM commented out for IO testing


		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Bobs IO Check
	Display.Phase(L"Dig Out");
	ControllerPassFail(CheckDigitalOut(32));		//tests 32 digital outputs.

	Display.Phase(L"Dig In");
	ControllerPassFail(CheckDigitalIn(16));			//tests 16 digital inputs.

	Display.Phase(L"Ana Out");
	ControllerPassFail(CheckAnalogOut(8));			//tests 8 analog outputs.

	Display.Phase(L"Ana In");
	ControllerPassFail(CheckAnalogIn(12, 1, 100));	//test 12 analog Inputs for a valid range of 1 to 100.

	strVarEntry = DE("Please Press \"Caps\" and", "Enter Your Initials ", "in Capital Letters", NULL, NULL, "Please enter Initials then press enter.");
	Display.Log(L"Testers Initials", strVarEntry, SendLogStatus);


	/*if (ControllerPassFail(true))
	{
		Display.Log(L"Controller PASSED",L"PASSED",SendLogStatus);
	}else
	{
		Display.Log(L"Controller FAILED",L"FAILED",SendLogStatus);
	}*/ //MDM commented out for testing

	bFnRunning = false;
	Display.Log(L"Controller Check Ended", L"", SendLogStatus);
	Display.OverrideEnable();

	//Display.EndControl();
}

void FnTest()
{
	bFnRunning = true;
	int i_Prod = 0;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Function Checks

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% these functions are used to return values for all the recipe types.
	Display.Phase(L"Fn Tests");

	PrintProdType(Display.GetProduct(0, lBallRecipeNo));

	if (lBallRecipeNo == 94 || lBallRecipeNo == 95) flIT = 20;
	else flIT = 15;
	TestBallFns();
	Display.EndControl();

	if (lBallRecipeNo >= 95) lBallRecipeNo = 92;
	else lBallRecipeNo = lBallRecipeNo + 1;

	if (BKillTheTask)goto FnTestEndLbl;	//these mean if an over ride is used they will goto the end of this function.
	PrintProdType(Display.GetProduct(0, 1));
	TestGenFns();
	Display.EndControl();

	//as 9/28/2010
		/*SendLogStatus=ABORT;
	if (BKillTheTask)goto FnTestEndLbl;
		PrintProdType(Display.GetProduct(0,35));
		TestTunacalFns();
		Display.EndControl();

	SendLogStatus=DEVIATION;
	if (BKillTheTask)goto FnTestEndLbl;

		i_Prod=Display.GetProduct(0,lNumericalRecipeNo);
		PrintProdType(i_Prod);
		PV.SetProduct(i_Prod);
		TestNumericalFns();
		if(lNumericalRecipeNo == 15) lNumericalRecipeNo = 60;
		else
		{
			if(lNumericalRecipeNo == 60) lNumericalRecipeNo = 61;
			else lNumericalRecipeNo = 15;
		}
		Display.EndControl();

	*/
	SendLogStatus = NORMAL;


FnTestEndLbl:						//if an over ride is used the function comes to here.

	bFnRunning = false;

} //end of function FnTest();

void ShootSomeLogs()
{
	bool	bDoOnceFlag = false, bDoOnceFlag1 = false, bDoOnceFlag2 = false, bDoOnceFlag3 = false;
	int		iChannelNumber = 8;
	short int	iLogNumber = 0;
	short int	iCurrentPLCVariable = 0;
	float		flCurrentPLCVariable = 0.0;
	CString strVar10, strVar11, strVar12, strVar13;
	//	SYSTEMTIME objSystemTime;


	bFnRunning = true;
	long	lLogNumber = 0;



	CString strVarEntry = L"";
	Display.Phase(L"Start");


	PV.SetProduct(Display.GetProduct(0, 1));
	Profiler.StartProfiler();
	Display.SetCookTemp(L"250.5°F");
	Display.SetCookTime(L"5:00 Min");
	Display.SetInitialTemp((float)130.95);

	if (BKillTheTask)goto ShootSomeLogsEndLbl;
	while (lLogNumber < 49)
	{
		lLogNumber++;
		strVar10.Format(L"%-d", lLogNumber);
		Display.Log(L"Shooting Log Indexed ", strVar10, SendLogStatus);


		if (lLogNumber < 9 && !bDoOnceFlag)
		{
			Display.Phase(L"StartProcess");
			bDoOnceFlag = true;
		}

		if (lLogNumber >= 10 && lLogNumber <= 25 && !bDoOnceFlag1)
		{
			Display.Phase(L"Second Phase");
			bDoOnceFlag1 = true;
		}

		if (lLogNumber >= 26 && lLogNumber <= 40 && !bDoOnceFlag2)
		{
			Display.Phase(L"Third Phase");
			bDoOnceFlag2 = true;
		}

		if (lLogNumber >= 41 && lLogNumber <= 49 && !bDoOnceFlag3)
		{
			Display.Phase(L"End Phase");
			bDoOnceFlag3 = true;
		}

		if (BKillTheTask)goto ShootSomeLogsEndLbl;

		Sleep(1000);
	}


ShootSomeLogsEndLbl:

	//as 9/28/2010	PrintDateLog();
		//as 9/28/2010PrintTimeLog();
	Profiler.StopProfiler();


	lLogNumber++;
	strVar10.Format(L"%-d", lLogNumber);
	Display.Log(L"Task Shoot Logs Complete", strVar10, SendLogStatus);
	//as 9/28/2010	Display.EndControl();
	bFnRunning = false;

}	//end of ShootSomeLogs


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^CheckKeyboard
bool CheckKeyboard(CString strChkStr)
{
	CString	strTempKeybdChk = L" ";
	bool	bPassFlag = true;

	strTempKeybdChk = DE("Please Enter the following Keys", "to be checked, in the order specified",
		strChkStr, NULL, NULL, "Please enter " + strChkStr + " then press enter.");
	if (strTempKeybdChk.CompareNoCase(strChkStr))
	{
		SendLogStatus = LOGERROR;
		Display.Log(strChkStr + L" Failed", strTempKeybdChk, SendLogStatus);
		bPassFlag = false;
		SendLogStatus = NORMAL;

	}
	else
	{
		Display.Log(strChkStr + L" Passed", strTempKeybdChk, SendLogStatus);
	}

	return bPassFlag;

}//end of fn CheckKeyboard();

bool MiscKeyboardChecks()
{
	bool bPassFlag = true,
		bLoopFlag = true;
	CString strYNVar = L" ";
	do {
		DM("Hit F2 ", "To see if EventLog", "Screen pops up", 0, 0, "Hit F2 then escape to return to this screen. Then Enter.");
		DM("Hit F4 ", "And force Some IO", "", 0, 0, "Hit F4 then escape to return to this screen. Then Enter.");
		DM("Hit F5 ", "And Calibrate Something", "", 0, 0, "Hit F5 then escape to return to this screen. Then Enter.");
		DM("Hit F6 and scroll through ", "the past logs", "use \"Page\"(Page Up) and \"Cntrl\"(Page Down) ", 0, 0, "Hit F6 then escape to return to this screen. Then Enter.");
		DM("Hit \"HELP\" ", "And read controller information", "", 0, 0, "Hit \"HELP\" then escape to return to this screen. Then Enter.");

		strYNVar = DE("Did the following keys work ok?", "Tab, Page,Cntrl, Arrow Keys", "F2, F4, F5, F6, Escape, Clear(backspace) and DEL?", NULL, NULL, "Please enter Y/N then press enter. Then Enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;
		DM("Hit \"F4\" ", "And remove All", "Forced IO on the controller.", 0, 0, "Hit F4 and remove all Forcing then escape to return to this screen. Then Enter.");
		DM("Hit \"F5\" ", "And remove All", "Calibrations on the controller.", 0, 0, "Hit F5 and remove all calibrations then escape to return to this screen. Then Enter.");

		strYNVar = DE("", "", "Re-Test Misc Keyboard commands?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
		{
			bLoopFlag = false;	//means they didnt entered y as in they do not wish to retest the keyboard

		}
		else
		{
			bPassFlag = true; //resets the flag because digital inputs are going to be retested.
		}

	} while (bLoopFlag);

	if (bPassFlag)
		Display.Log(L"Misc Keyboard Test Passed", L"Passed");
	else
		Display.Log(L"Misc Keyboard Test Failed", L"Failed");

	return bPassFlag;
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^CheckDigitalOut
bool CheckDigitalOut(int iTotDigOut = 32)	//default to 32 for 1 bobs io card with 2 simulators.
{
	long lDigTempVar = 0;
	bool bPassFlag = true,
		bLoopFlag = true;
	int i;
	iTotDigOut = 24;
	CString strYNVar = L" ";
	do {
		TurnAllDigitalOutputs(iTotDigOut, 0); //turns digital outputs off to start test.

		for (int i = 1; i <= iTotDigOut; i++)
		{
			lDigTempVar = DigitalIO.ReadDigitalOut(i); //reads each digital output and check to see if one is still on.
			if (lDigTempVar == 1)
			{
				DL("Digital Output ", "is Forced On", "", NULL, i, "");
				bPassFlag = false;
			}
		}



		TurnAllDigitalOutputs(iTotDigOut, 1); //turns digital outputs On to see if any forced off.


		for (i = 1; i <= iTotDigOut; i++)
		{
			lDigTempVar = DigitalIO.ReadDigitalOut(i); //reads each digital output and check to see if one is still Off.
			if (lDigTempVar == 0)
			{
				DL("Digital Output ", "is Forced Off", "", NULL, i, "");
				bPassFlag = false;
			}
		}

		strYNVar = DE("", "", "Are All Digital Outputs On?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;


		TurnAllDigitalOutputs(iTotDigOut, 0);
		strYNVar = DE("", "", "Are All Digital Outputs Off?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;

		strYNVar = DE("", "", "Re-Test Digital Outputs?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
		{
			bLoopFlag = false;	//means they entered y

		}
		else
		{
			bPassFlag = true; //resets the flag because digital inputs are going to be retested.
		}

	} while (bLoopFlag);

	if (bPassFlag)
		Display.Log(L"Digitial Ouput Test Passed", L"Passed");
	else
		Display.Log(L"Digitial Ouput Test Failed", L"Failed");

	return bPassFlag; //a return of true means all digital outputs validated, a false means one failed or was forced or an alarm was sounding.
}



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^CheckDigitalIn
bool CheckDigitalIn(int iTotDigIn = 16)//default to 16 for 1 bobs io card with 2 simulators.
{
	long lDigTempVar = 0;
	bool bPassFlag = true,
		bLoopFlag = true;
	int i;
	CString strYNVar = L" ";

	do
	{
		DM("Please Turn All Digital inputs On", "", "", NULL, NULL, "Press enter when ready to test.");

		for (int i = 1; i <= iTotDigIn; i++)
		{
			lDigTempVar = DigitalIO.ReadDigitalIn(i);
			if (lDigTempVar == 0)
			{
				DL("Digital Input ", "is Off", "", NULL, i, "");
				bPassFlag = false; //means one of the digital inputs was off

			}
		}

		DM("Please Turn All Digital inputs Off", "", "", NULL, NULL, "");


		for (i = 1; i <= iTotDigIn; i++)
		{
			lDigTempVar = DigitalIO.ReadDigitalIn(i);
			if (lDigTempVar == 1)
			{
				DL("Digital Input ", "is On", "", NULL, i, "");
				bPassFlag = false; //means one of the digital inputs was on

			}

		}


		strYNVar = DE("", "", "Re-Test Digitial Inputs?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
		{
			bLoopFlag = false;

		}
		else
		{
			bPassFlag = true; //resets the flag because digital inputs are going to be retested.
		}
	} while (bLoopFlag);

	if (bPassFlag)
		Display.Log(L"Digitial Input Test Passed", L"Passed");
	else
		Display.Log(L"Digitial Input Test Failed", L"Failed");
	return bPassFlag; //a return of true means all digital inputs validated a false means one failed.
}


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^CheckAnalogOut
bool CheckAnalogOut(int iTotAnaOut = 8)	//default to 8 for 1 bobs io card with 2 simulators.
{
	bool bPassFlag = true,
		bLoopFlag = true;
	int i;
	float flAnaTempVar = 0;
	CString strYNVar = L" ";
	do
	{
		PidTestStop(iTotAnaOut);	//stops any pids that might be running on the analog outputs.
		TurnAllAnalogOutputs(iTotAnaOut, 0); //turns off all analog outputs to be tested.
		//Sleep(100); //LK

		//checks to make sure controller can turn the analog outputs to less then 1%
		for (int i = 1; i <= iTotAnaOut; i++)
		{
			flAnaTempVar = AnalogIO.ReadAnalogOut(i);
			if (flAnaTempVar > 1)
			{
				DL("Analog Ouput", "is > 1", "", NULL, i, "");
				bPassFlag = false; //means one of the Analog Outputs was higher then 1%
				//Sleep(100); //LK

			}
		}

		//checks to make sure controller can turn the analog outputs to greater then 99%
		TurnAllAnalogOutputs(iTotAnaOut, 100); //turns all analog outputs to 100% for the test.
		for (i = 1; i <= iTotAnaOut; i++)
		{
			flAnaTempVar = AnalogIO.ReadAnalogOut(i);
			if (flAnaTempVar < 99)
			{
				DL("Digital Input ", "is < 99", "", NULL, i, "");
				bPassFlag = false; //means one of the Analog Outputs was less then 99%
				//Sleep(100);LK
			}
		}

		//tests the analog outputs at 0%
		TurnAllAnalogOutputs(iTotAnaOut, 0);
		strYNVar = DE("", "", "Are All Analog Outputs at 0%?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;


		//tests the analog outputs at 25%
		TurnAllAnalogOutputs(iTotAnaOut, 25);
		strYNVar = DE("", "", "Are All Analog Outputs at 25%?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;

		//tests the analog outputs at 50%
		TurnAllAnalogOutputs(iTotAnaOut, 50);
		strYNVar = DE("", "", "Are All Analog Outputs at 50%?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;

		//tests the analog outputs at 75%
		TurnAllAnalogOutputs(iTotAnaOut, 75);
		strYNVar = DE("", "", "Are All Analog Outputs at 75%?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;

		//tests the analog outputs at 100%
		TurnAllAnalogOutputs(iTotAnaOut, 100);
		strYNVar = DE("", "", "Are All Analog Outputs at 100%?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
			bPassFlag = false;




		Pause();
		TurnAllAnalogOutputs(iTotAnaOut, 0);	//turns all analog outputs to 0%

	//this tests AnalogIO.ramp output rate and period.
		Display.Display(20, L"Testing");
		Display.Display(21, L"AnalogIO.RampOutputPeriod(1,100,60);");
		Display.Display(22, L"and");
		Display.Display(23, L"AnalogIO.RampOutputRate(2,100,2);");
		Display.Display(24, L"for 50 seconds");
		Display.Entry(0, 0, true);

		PinesTime7.StartTimer();
		AnalogIO.RampOutputPeriod(1, 100, 60);
		AnalogIO.RampOutputRate(2, 100, 2);
		PinesTime7.ZeroTime();

		while (PinesTime7.GetElapsedTime() <= 60)
		{
			Sleep(100);
			strYNVar.Format(L"%d", 50 - PinesTime7.GetElapsedTime());
			Display.DisplayStatus(L"Please Wait " + strYNVar + L" Seconds");
		}
		Display.KillEntry();

		float flrmpoutPeriod = AnalogIO.ReadAnalogOut(1);
		float flrmpoutRate = AnalogIO.ReadAnalogOut(2);

		PinesTime7.StopTimer();

		//if (flrmpoutPeriod < 82 || flrmpoutPeriod > 85) //should be ~25
		if (flrmpoutPeriod < 97 || flrmpoutPeriod > 103) //should be ~25
		{

			DL("RampOutputPeriod", " FAILED", "", 0, flrmpoutPeriod, "");
			bPassFlag = false;
			//Sleep(100);
		}
		else
		{
			DL("RampOutputPeriod", " Passed!", "", 0, flrmpoutPeriod, "");
		}

		//	if (flrmpoutRate < 24 || flrmpoutRate > 26 ) //should be ~83.333
		if (flrmpoutRate < 97 || flrmpoutRate > 103) //should be ~83.333
		{
			DL("RampOutputRate", " FAILED", "", 0, flrmpoutRate, "");
			bPassFlag = false;
			//Sleep(100);

		}
		else
		{
			DL("RampOutputRate", " Passed!", "", 0, flrmpoutPeriod, "");
		}



		strYNVar = DE("", "", "Re-Test Analog Outputs?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
		{
			bLoopFlag = false;

		}
		else
		{
			bPassFlag = true; //resets the flag because digital inputs are going to be retested.
		}


	} while (bLoopFlag);






	if (bPassFlag)
		Display.Log(L"Analog Ouput Test Passed", L"Passed");
	else
		Display.Log(L"Analog Ouput Test Failed", L"Failed");

	return bPassFlag;
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^CheckAnalogIn
bool CheckAnalogIn(int iTotAnaIn = 12, float flLowTestVal = 1, float flHighTestVal = 100)	//default to 12 for 1 bobs io card with 2 simulators.
{
	bool bPassFlag = true,
		bLoopFlag = true;
	int i;
	float flAnaTempVar = 0;
	CString strYNVar = L" ";

	do
	{
		DM("Please Turn All Analog Inputs to Lowest Setting", "", "Less Than", NULL, flLowTestVal, "Press enter when ready.");

		for (int i = 1; i <= iTotAnaIn; i++)
		{
			flAnaTempVar = AnalogIO.ReadAnalogIn(i);
			if (flAnaTempVar > flLowTestVal)
			{
				DL("Analog Input ", "is > Lower Boundry, AI: ", "", i, flAnaTempVar, "");
				bPassFlag = false; //means one of the Analog Outputs was higher then 1%
				//Sleep(100); //LK

			}
		}

		DM("Please Turn All Analog Inputs to Highest setting", "", "Greater than", NULL, flHighTestVal, "Press enter when ready.");

		for (i = 1; i <= iTotAnaIn; i++)
		{
			flAnaTempVar = AnalogIO.ReadAnalogIn(i);
			if (flAnaTempVar < flHighTestVal)
			{
				DL("Analog Input ", "is < Upper Boundry, AI: ", "", i, flAnaTempVar, "");
				bPassFlag = false; //means one of the Analog Outputs was less then 99%
				//Sleep(100);  //LK
			}

		}

		strYNVar = DE("", "", "Re-Test Analog Inputs?", NULL, NULL, "Please enter Y/N then press enter.");
		if (strYNVar.CompareNoCase(L"y"))
		{
			bLoopFlag = false;

		}
		else
		{
			bPassFlag = true; //resets the flag because Analog Inputs are going to be retested.
		}

	} while (bLoopFlag);

	if (bPassFlag)
		Display.Log(L"Analog Input Test Passed", L"Passed");
	else
		Display.Log(L"Analog Input Test Failed", L"Failed");

	return bPassFlag;
}


void TestGenFns()
{

	//as 9/28/2010 Display.Log(L" ");

	Display.Log(L"Generic fns");

	double fGenericRet = 0;

	fGenericRet = PV.GetGenericTemp();
	DL("Test 1: ", "PV.GetGenericTemp()", "", NULL, (float)fGenericRet, "");
	fGenericRet = PV.GetGenericTime();
	DL("Test 2: ", "PV.GetGenericTime()", "", NULL, (float)fGenericRet, "");
}

void TestBallFns()
{


	double double_Var;

	Display.Log(L" ");

	Display.Log(L"ball fns ");
	//test 1			
	double_Var = Ball.GetJHValue();
	DL("test 1: ", "Ball.GetJHValue(); ", "", NULL, (float)double_Var, "");

	//test 2
	double_Var = Ball.GetFHValue();
	DL("test 2: ", "Ball.GetFHValue(); ", "", NULL, (float)double_Var, "");

	//test 3
	double_Var = Ball.GetF2Value();
	DL("test 3: ", "Ball.GetF2Value(); ", "", NULL, (float)double_Var, "");

	//test 4
	double_Var = Ball.GetXBHValue();
	DL("test 4: ", "Ball.GetXBHValue(); ", "", NULL, (float)double_Var, "");

	//test 5
	double_Var = Ball.GetFValue();
	DL("test 5: ", "Ball.GetFValue(); ", "", NULL, (float)double_Var, "");

	//test 6
	double_Var = Ball.GetZValue();
	DL("test 6: ", "Ball.GetZValue(); ", "", NULL, (float)double_Var, "");

	//test 7
	double_Var = Ball.GetRefValue();
	DL("test 7: ", "Ball.GetRefValue(); ", "", NULL, (float)double_Var, "");


	//test 8
	double_Var = Ball.GetMGValue();
	DL("test 8: ", "Ball.GetMGValue(); ", "", NULL, (float)double_Var, "");

	//test 9
	double_Var = Ball.GetITLowValue();
	DL("test 9: ", "Ball.GetITLowValue(); ", "", NULL, (float)double_Var, "");

	//test 10
	double_Var = Ball.GetITHighValue();
	DL("test 10: ", "Ball.GetITHighValue(); ", "", NULL, (float)double_Var, "");

	//test 11
	double_Var = Ball.GetSetpointValue();
	DL("test 11: ", "Ball.GetSetpointValue(); ", "", NULL, (float)double_Var, "");

	//test 12
	double_Var = Ball.GetLowRTValue();
	DL("test 12: ", "Ball.GetLowRTValue(); ", "", NULL, (float)double_Var, "");

	//test 13
	double_Var = Ball.GetRTIntValue();
	DL("test 13: ", "Ball.GetRTIntValue(); ", "", NULL, (float)double_Var, "");

	//test 14
	double_Var = Ball.GetMinProcessValue();
	DL("test 14: ", "Ball.GetMinProcessValue(); ", "", NULL, (float)double_Var, "");

	//test 15
	double_Var = Ball.GetCUTValue();
	DL("test 15: ", "Ball.GetCUTValue(); ", "", NULL, (float)double_Var, "");

	//test 16
	double_Var = Ball.GetCorrectionValue();
	DL("test 16: ", "Ball.GetCorrectionValue(); ", "", NULL, (float)double_Var, "");

	//test 17
	double_Var = PV.CalculateBallProcess(flIT);
	DL("test 17: ", "PV.CalculateBallProcess() ", "", NULL, double_Var, "");

	//test 18		
	double_Var = PV.CalculateDeviatedBallProcess(255.0, flIT);
	DL("test 18: ", "CalculateDeviatedBallProcess", "", NULL, double_Var, "");

	//test 19
	double_Var = PV.GetBallTemp(0);
	DL("test 19: ", "PV.GetBallTemp(0) ", "", NULL, (float)double_Var, "");

	double_Var = PV.CalculateDeviatedBallProcess(250.0, flIT);
	DL("test 20: ", "CalculateDeviatedBallProcess 250", "", NULL, double_Var, "");
	double_Var = PV.CalculateDeviatedBallProcess(245.0, flIT);
	DL("test 21: ", "CalculateDeviatedBallProcess 245", "", NULL, double_Var, "");
	double_Var = PV.CalculateDeviatedBallProcess(240.0, flIT);
	DL("test 22: ", "CalculateDeviatedBallProcess 240", "", NULL, double_Var, "");

}



void TestNumericalFns() {
	int q;
	//this function tests the functions included in the Numerical. class and PV.
	Display.Log(L" ");

	Display.Log(L"Numerical fns ");

	//test 1
	double NumericalDoubleVar = Numerical.GetJHValue();
	DL("test 1: ", "Numerical.GetJHValue()", "", NULL, (float)NumericalDoubleVar, "");

	//test 2
	NumericalDoubleVar = Numerical.GetFHValue();
	DL("test 2: ", " Numerical.GetFHValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 3
	NumericalDoubleVar = Numerical.GetF2Value();
	DL("test 3: ", "Numerical.GetF2Value", "", NULL, (float)NumericalDoubleVar, "");

	//test 4
	NumericalDoubleVar = Numerical.GetXBHValue();
	DL("test 4: ", "Numerical.GetXBHValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 5
	NumericalDoubleVar = Numerical.GetSchF0Value();
	DL("test 5: ", "Numerical.GetSchF0Value", "", NULL, (float)NumericalDoubleVar, "");

	//test 6
	NumericalDoubleVar = Numerical.GetMinF0Heating();
	DL("test 6: ", "Numerical.GetMinF0Heating", "", NULL, (float)NumericalDoubleVar, "");

	//test 7
	NumericalDoubleVar = Numerical.GetFValue();
	DL("test 7: ", "Numerical.GetFValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 8
	NumericalDoubleVar = Numerical.GetZValue();
	DL("test 8: ", "Numerical.GetZValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 9
	NumericalDoubleVar = Numerical.GetJCValue();
	DL("test 9: ", "Numerical.GetJCValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 10
	NumericalDoubleVar = Numerical.GetFCValue();
	DL("test 10: ", "Numerical.GetFCValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 11
	NumericalDoubleVar = Numerical.GetSetpointValue();
	DL("test 11: ", "Numerical.GetSetpointValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 12
	NumericalDoubleVar = Numerical.GetReferenceValue();
	DL("test 12: ", "Numerical.GetReferenceValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 13
	NumericalDoubleVar = Numerical.GetRTIntValue();
	DL("test 13: ", "Numerical.GetRTIntValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 14
	NumericalDoubleVar = Numerical.GetLowITValue();
	DL("test 14: ", "Numerical.GetLowITValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 15
	NumericalDoubleVar = Numerical.GetHighITValue();
	DL("test 15: ", "Numerical.GetHighITValue", "", NULL, (float)NumericalDoubleVar, "");

	//test 16
	NumericalDoubleVar = Numerical.GetInitialRTTemp();
	DL("test 16: ", "Numerical.GetInitialRTTemp", "", NULL, (float)NumericalDoubleVar, "");

	//test 17
	NumericalDoubleVar = Numerical.GetMinRTTemp();
	DL("test 17: ", "Numerical.GetMinRTTemp", "", NULL, (float)NumericalDoubleVar, "");

	//test 18
	NumericalDoubleVar = Numerical.GetMinimumProcessTime();
	DL("test 18: ", "Numerical.GetMinimumProcessTime()", "", NULL, (float)NumericalDoubleVar, "");

	//test 19
	CString NumericalStringVar = Numerical.GetLowITTime();
	Display.Log(L"test 19:  Numerical.GetLowITTime()", NumericalStringVar);
	//DL("test 19: ","Numerical.GetLowITTime() ",NumericalStringVar,NULL,NULL,"");

//test 20
	NumericalStringVar = Numerical.GetHighITTime();
	Display.Log(L"test 20:  Numerical.GetHighITTime()", NumericalStringVar);
	//DL("test 20: ","Numerical.GetHighITTime()",NumericalStringVar,NULL,NULL,"");

//test 21
	NumericalDoubleVar = PV.CalculateNumericalProcess((float)20);	//calling this function when running a Generic Process will lock up the controller.
	DL("test 21: ", "PV.CalculateNumericalProcess()", "", NULL, NumericalDoubleVar, "");

	//test 22
	NumericalDoubleVar = PV.GetNumericalTemp(0);
	DL("test 22: ", "PV.GetNumericalTemp(0)", "", NULL, NumericalDoubleVar, "");

	//test 23
	NumericalDoubleVar = PV.CalculateDeviatedNumericalProcess(251.0, 20);//250.0 is the setpoint and 190.0 is the Initial Temperature	
	DL("test 23: ", "PV.CalculateDeviatedNumericalProcess(251.0,20.0)", "", NULL, NumericalDoubleVar, "");

	NumericalDoubleVar = PV.CalculateDeviatedNumericalProcess(251.0, 25);//250.0 is the setpoint and 190.0 is the Initial Temperature	
	DL("test 24: ", "CalculateDeviatedNmrcalProcess(251.0,25.0)", "", NULL, NumericalDoubleVar, "");
	NumericalDoubleVar = PV.CalculateDeviatedNumericalProcess(251.0, 30);//250.0 is the setpoint and 190.0 is the Initial Temperature	
	DL("test 25: ", "CalculateDeviatedNmrcalProcess(251.0,30.0)", "", NULL, NumericalDoubleVar, "");

	Display.Log(L" ");
	Display.Log(L"Numerical fns Segment Fns");

	int iNumOfSegments = 7;	//this is the number of segments to return values for.

	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUAdjTime(q);
		DL("Numerical.GetCUAdjTime() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUAdjCorrection(q);
		DL("Numerical.GetCUAdjCorrection() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUAdjOffset(q);
		DL("Numerical.GetCUAdjOffset() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUAdjPressure(q);
		DL("Numerical.GetCUAdjPressure() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCOOLAdjTime(q);
		DL("Numerical.GetCOOLAdjTime() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCOOLAdjCorrection(q);
		DL("Numerical.GetCOOLAdjCorrection() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCOOLAdjOffset(q);
		DL("Numerical.GetCOOLAdjOffset() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCOOLAdjPressure(q);
		DL("Numerical.GetCOOLAdjPressure() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCOOLPsiOffset(q);
		DL("Numerical.GetCOOLPsiOffset() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}

	DL("Duplicate Commands CU", "", "", 0, 0, "");

	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUSegTime(q);
		DL("Numerical.GetSegTime() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUStartTemp(q);
		DL("Numerical.GetCUStartTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUFinalTemp(q);
		DL("Numerical.GetCUFinalTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUTempOffset(q);
		DL("Numerical.GetCUTempOffset() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUStartPress(q);
		DL("Numerical.GetCUFinalTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCUFinalPress(q);
		DL("Numerical.GetCUFinalPress() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}


	DL("Duplicate Commands Cool", "", "", 0, 0, "");

	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolSegTime(q);
		DL("Numerical.GetCoolSegTime() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolStartTemp(q);
		DL("Numerical.GetCoolStartTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolFinalTemp(q);
		DL("Numerical.GetCoolFinalTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolTempOffset(q);
		DL("Numerical.GetCoolTempOffset() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolStartPress(q);
		DL("Numerical.GetCoolFinalTemp() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}
	for (q = 1; q <= iNumOfSegments; q++)
	{
		NumericalDoubleVar = Numerical.GetCoolFinalPress(q);
		DL("Numerical.GetCoolFinalPress() ", "seg#: ", "", (long)q, (double)NumericalDoubleVar, "");
	}

}//end of TestNumericalFns();


void TestTunacalFns() {


	Display.Log(L" ");


	Display.Log(L"TunaCal fns ");
	//test 1
	float	FloatTunacalVar = Tuna.GetTunaTotalWidth();
	DL("test 1: ", "Tuna.GetTunaTotalWidth() ", "", NULL, FloatTunacalVar, "");

	//test 2
	FloatTunacalVar = Tuna.GetTunaTotalHeight();
	DL("test 2: ", "Tuna.GetTunaTotalHeight() ", "", NULL, FloatTunacalVar, "");

	//test 3
	FloatTunacalVar = Tuna.GetTunaTotalLength();
	DL("test 3: ", "Tuna.GetTunaTotalLength() ", "", NULL, FloatTunacalVar, "");

	//test 4
	FloatTunacalVar = Tuna.GetPreCookTime();
	DL("test 4: ", "Tuna.GetPreCookTime() ", "", NULL, FloatTunacalVar, "");

	//test 5
	FloatTunacalVar = Tuna.GetBackBoneIT();
	DL("test 5: ", "Tuna.GetBackBoneIT() ", "", NULL, FloatTunacalVar, "");

	//test 6
	FloatTunacalVar = Tuna.GetBackboneTemp();
	DL("test 6: ", "Tuna.GetBackboneTemp() ", "", NULL, FloatTunacalVar, "");

	//test 7
	FloatTunacalVar = Tuna.GetBackBoneDepth();
	DL("test 7: ", "Tuna.GetBackBoneDepth() ", "", NULL, FloatTunacalVar, "");

	//test 8
	FloatTunacalVar = Tuna.GetKValue();
	DL("test 8: ", "Tuna.GetKValue() ", "", NULL, FloatTunacalVar, "");

	//test 9
	FloatTunacalVar = Tuna.GetPValue();
	DL("test 9: ", "Tuna.GetPValue() ", "", NULL, FloatTunacalVar, "");

	//test 10
	FloatTunacalVar = Tuna.GetCPValue();
	DL("test 10: ", "Tuna.GetCPValue() ", "", NULL, FloatTunacalVar, "");

	//test 11
	FloatTunacalVar = Tuna.GetHValue();
	DL("test 11: ", "Tuna.GetHValue() ", "", NULL, FloatTunacalVar, "");

	//test 12
	FloatTunacalVar = Tuna.GetTemp1();
	DL("test 12: ", "Tuna.GetTemp1() ", "", NULL, FloatTunacalVar, "");

	//test 13
	FloatTunacalVar = Tuna.GetTemp2();
	DL("test 13: ", "Tuna.GetTemp2", "", NULL, FloatTunacalVar, "");

	//test 14
	FloatTunacalVar = Tuna.GetTemp3();
	DL("test 14: ", "Tuna.GetTemp3() ", "", NULL, FloatTunacalVar, "");

}//end of TestTunacalFns()



void InitObjects()
{

	HRESULT hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hResult != S_OK)
		return;

	strVal = "PinesDisplay.PSLDisplay.1";
	pclsidDisplay = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidDisplay);
	hResult = CoCreateInstance(*pclsidDisplay, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownDisplay);
	if (hResult != S_OK)
		return;
	pIUnknownDisplay->QueryInterface(IID_IDispatch, (void**)&pDispatchDisplay);

	Display.m_bAutoRelease = FALSE;
	Display.AttachDispatch(pDispatchDisplay);

	//AfxMessageBox(L"Display Passed");
	strVal = "PinesIO.AnalogIO.1";
	pclsidAIO = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidAIO);
	hResult = CoCreateInstance(*pclsidAIO, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownIO);
	if (hResult != S_OK)
		return;
	pIUnknownIO->QueryInterface(IID_IDispatch, (void**)&pDispatchIO);

	AnalogIO.m_bAutoRelease = FALSE;
	AnalogIO.AttachDispatch(pDispatchIO);

	//AfxMessageBox(L"analogIO Passed");

	strVal = "PinesIO.DigitalIO.1";
	pclsidDIO = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidDIO);
	hResult = CoCreateInstance(*pclsidDIO, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownDIO);
	if (hResult != S_OK)
		return;
	pIUnknownDIO->QueryInterface(IID_IDispatch, (void**)&pDispatchDIO);

	DigitalIO.m_bAutoRelease = FALSE;
	DigitalIO.AttachDispatch(pDispatchDIO);

	//AfxMessageBox(L"DigitalIO Passed");

	strVal = "PinesIO.PIDCommands.1";
	pclsidPID = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidPID);
	hResult = CoCreateInstance(*pclsidPID, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownPID);
	if (hResult != S_OK)
		return;
	pIUnknownPID->QueryInterface(IID_IDispatch, (void**)&pDispatchPID);

	PID.m_bAutoRelease = FALSE;
	PID.AttachDispatch(pDispatchPID);

	//AfxMessageBox(L"PID Passed");

	strVal = "PinesIO.ProcessVariable.1";
	pclsidPV = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidPV);
	hResult = CoCreateInstance(*pclsidPV, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownPV);
	if (hResult != S_OK)
		return;
	pIUnknownPV->QueryInterface(IID_IDispatch, (void**)&pDispatchPV);

	PV.m_bAutoRelease = FALSE;
	PV.AttachDispatch(pDispatchPV);

	//Querying Three New Interfaces Here
	//NUMERICAL	
	strVal = "PinesIO.NumericalProcess.1";
	pclsidNumerical = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidNumerical);
	hResult = CoCreateInstance(*pclsidNumerical, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownNumerical);
	if (hResult != S_OK)
		return;
	pIUnknownNumerical->QueryInterface(IID_IDispatch, (void**)&pDispatchNumerical);

	Numerical.m_bAutoRelease = FALSE;
	Numerical.AttachDispatch(pDispatchNumerical);

	//BALL	
	strVal = "PinesIO.BallProcess.1";
	pclsidBall = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidBall);
	hResult = CoCreateInstance(*pclsidBall, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownBall);
	if (hResult != S_OK)
		return;
	pIUnknownBall->QueryInterface(IID_IDispatch, (void**)&pDispatchBall);

	Ball.m_bAutoRelease = FALSE;
	Ball.AttachDispatch(pDispatchBall);

	//TUNACAL	
	strVal = "PinesIO.TunacalProcess.1";
	pclsidTunacal = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidTunacal);
	hResult = CoCreateInstance(*pclsidTunacal, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownTunacal);
	if (hResult != S_OK)
		return;
	pIUnknownTunacal->QueryInterface(IID_IDispatch, (void**)&pDispatchTunacal);

	Tuna.m_bAutoRelease = FALSE;
	Tuna.AttachDispatch(pDispatchTunacal);

	//AfxMessageBox(L"PV Passed");

	strVal = "PinesIO.TaskLoop.1";
	pclsidTaskloop = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidTaskloop);
	hResult = CoCreateInstance(*pclsidTaskloop, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownTaskloop);
	if (hResult != S_OK)
		return;
	pIUnknownTaskloop->QueryInterface(IID_IDispatch, (void**)&pDispatchTaskloop);

	TaskLoop.m_bAutoRelease = FALSE;
	TaskLoop.AttachDispatch(pDispatchTaskloop);

	strVal = "PinesIO.Profiler.1";
	pclsidProfiler = new CLSID;
	CLSIDFromProgID((LPCOLESTR)strVal.AllocSysString(), pclsidProfiler);
	hResult = CoCreateInstance(*pclsidProfiler, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&pIUnknownProfiler);
	if (hResult != S_OK)
		return;
	pIUnknownProfiler->QueryInterface(IID_IDispatch, (void**)&pDispatchProfiler);

	Profiler.m_bAutoRelease = FALSE;
	Profiler.AttachDispatch(pDispatchProfiler);

	Display.ShowMainWindow(); // added this here because we are always running the program with a GUI
	Sleep(1000);
	Profiler.Init();			//initializes the profiler. //NOTE: this will  cause a crash on older hosts.

}



void TurnAllDigitalOutputs(int iNumODigIn = 0, bool bOnOrOff = 0)
{// this function turns all the digital outputs off.
	for (int i = 0; i <= iNumODigIn; i++)
	{
		DigitalIO.SetDigitalOut(i, bOnOrOff);
	}
}	//	TurnAllDigitalOutputs	

void TurnAllAnalogOutputs(int iNumberOfOutputs = 0, float percent = 0)
{//this function turns all the analog outputs off.
	for (int i = 0; i <= iNumberOfOutputs; i++)
	{
		AnalogIO.SetAnalogOut(i, percent);
	}

}	//	TurnAllAnalogOutputs

void DL(CString DMsg1 = " ", CString DMsg2 = " ", CString DMsg3 = " ", long LMsg4 = NULL, double FMsg5 = NULL, CString strStatusNothing = " ")
{
	// this function takes a string and then displays it on the screen and waits till enter is pressed.
	//displays the strings on lines 20 thru 22 and long on 23 float on 24
	//test function call
	//DL("Test line 20","test line 21","test line 22",(long)13,(float)13.2);
	//also note the if statments... if nothing is inputed in the field or NULL is inputed this function will not print over the line.
	//the way this can be used is if display.display(22,"some message here");
	// is that my functin will not print over that line.

	//PrintMemoryInfo();

	CString tempvar2;
	CString tempvar;


	if (DMsg1 != "" && DMsg1 != " ") { DMsg1.Format(L"%s", DMsg1); tempvar2 = DMsg1; };
	if (DMsg2 != "" && DMsg2 != " ") { DMsg2.Format(L"%s", DMsg2); tempvar2 += DMsg2; };
	if (DMsg3 != "" && DMsg3 != " ") { DMsg3.Format(L"%s", DMsg3); tempvar2 += DMsg3; };
	if (LMsg4 != NULL) { tempvar.Format(L"%d", LMsg4); tempvar2 += " "; tempvar2 += tempvar; };
	if (FMsg5 != NULL) { tempvar.Format(L"%5.2f", FMsg5); };
	Display.Log(tempvar2, tempvar, SendLogStatus);
	//Sleep(100);
};
void DM(CString DMsg1 = " ", CString DMsg2 = " ", CString DMsg3 = " ", long LMsg4 = NULL, double FMsg5 = NULL, CString DStat = " ") {
	// this function takes a string and then displays it on the screen and waits till enter is pressed.
	//displays the strings on lines 20 thru 22 and long on 23 float on 24
	//test function call
	//DM("Test line 20","test line 21","test line 22",(long)13,(float)13.2,"test Status");
	//also note the if statments... if nothing is inputed in the field or NULL is inputed this function will not print over the line.
	//the way this can be used is if display.display(22,"some message here");
	// that this functin will not print over that line.


	CString tempvar;

	if (DMsg1 != "" && DMsg1 != " ") { DMsg1.Format(L"%s", DMsg1); Display.Display(20, DMsg1); };
	if (DMsg2 != "" && DMsg2 != " ") { DMsg2.Format(L"%s", DMsg2); Display.Display(21, DMsg2); };
	if (DMsg3 != "" && DMsg3 != " ") { DMsg3.Format(L"%s", DMsg3); Display.Display(22, DMsg3); };
	if (LMsg4 != NULL) { tempvar.Format(L"%d", LMsg4); Display.Display(23, tempvar); };
	if (FMsg5 != NULL) { tempvar.Format(L"%5.2f", FMsg5); Display.Display(24, tempvar); };
	if (DStat != "" && DStat != " ") { DStat.Format(L"%s", DStat); Display.DisplayStatus(DStat); };
	Display.Entry(0, 0);

	WaitForEnterKey(false);
};

CString DE(CString DMsg1 = " ", CString DMsg2 = " ", CString DMsg3 = " ", long LMsg4 = NULL, double FMsg5 = NULL, CString DStat = " ") {
	// this function takes a string and then displays it on the screen and waits till enter is pressed.
	//displays the strings on lines 20 thru 22 and long on 23 float on 24
	//test function call
	//DM("Test line 20","test line 21","test line 22",(long)13,(float)13.2,"test Status");
	//also note the if statments... if nothing is inputed in the field or NULL is inputed this function will not print over the line.
	//the way this can be used is if display.display(22,"some message here");
	// is that my functin will not print over that line.


	CString tempvar;

	if (DMsg1 != "" && DMsg1 != " ")
	{
		DMsg1.Format(L"%s", DMsg1);
		Display.Display(20, DMsg1);
	};
	if (DMsg2 != "" && DMsg2 != " ")
	{
		DMsg2.Format(L"%s", DMsg2);
		Display.Display(21, DMsg2);
	};
	if (DMsg3 != "" && DMsg3 != " ")
	{
		DMsg3.Format(L"%s", DMsg3);
		Display.Display(22, DMsg3);
	};
	if (LMsg4 != NULL)
	{
		tempvar.Format(L"%d", LMsg4);
		Display.Display(23, tempvar);
	};
	if (FMsg5 != NULL)
	{
		tempvar.Format(L"%5.2f", FMsg5);
		Display.Display(24, tempvar);
	};
	if (DStat != "" && DStat != " ")
	{
		DStat.Format(L"%s", DStat);
		Display.DisplayStatus(DStat);
	};


	Display.Entry(3, 0);

	WaitForEnterKey(false);

	return Display.GetEnteredString(3);
	//NOTE: use Display.GetEnteredData(3) on the next line after calling this function to get the numerical value.
};
/* =====================Function DisplayInitialProcess Screen ==========*/
void DisplayInitialProcessScreen()
{//this sets up the inital process screen.

	//long lcount = 0;
	//long lcount1 = 0;


	Display.Display(1, L"Analog Input 1", L"000.0", FALSE, L"AI", L"1");
	Display.Display(2, L"Analog Input 2", L"000.0", FALSE, L"AI", L"2");
	Display.Display(3, L"Analog Input 3", L"000.0", FALSE, L"AI", L"3");
	Display.Display(4, L"Analog Input 4", L"000.0", FALSE, L"AI", L"4");
	Display.Display(5, L"Analog Input 5", L"000.0", FALSE, L"AI", L"5");
	Display.DisplayVariable(6, L"Counter1", L"LONG", (long)&lcount);
	Display.DisplayVariable(7, L"Counter1", L"LONG", (long)&lcount1);
	//Display.Display(6, L"Analog Input 6", L"000.0", FALSE, L"AI", L"6");
	//Display.Display(7, L"Analog Input 7", L"20.0", FALSE, L"AI", L"7");
	Display.Display(8, L"Analog Input 8", L"100.0", FALSE, L"AI", L"8");
	Display.Display(9, L"Analog Input 9", L"000.0", FALSE, L"AI", L"9");
	Display.Display(10, L"Analog Input 10", L"000.0", FALSE, L"AI", L"10");
	Display.Display(11, L"Analog Input 11", L"000.0", FALSE, L"AI", L"11");
	Display.Display(12, L"Analog Input 12", L"000.0", FALSE, L"AI", L"12");


}; // end of DisplayInitialProcessScreen


void WaitForEnterKey(bool bShowAlarm = false)
{
	PinesTime8.StartTimer();
	PinesTime8.ZeroTime();

	while (!Display.IsEnterKeyPressed())
	{
		Sleep(500);
		if (PinesTime8.GetElapsedTime() > 420)
		{
			if (bShowAlarm)
				Display.Alarm(2, 1, 16, L"Operator Entry Delay");

			PinesTime8.ZeroTime();
		}
	}

	PinesTime8.StopTimer();
	PinesTime8.ZeroTime();

	// NOPE	Sleep(250); // RIP Added 4/17/17 - to prevent lockup
}

void Pause()
{	// pause after each test inside display test
	DM("-----------------------------", "------------Paused-----------", "-----------------------------", NULL, NULL, "Please press enter...");
};



//this displays a log entry that prints the date
void PrintDateLog()
{

	int		iCurrentYear = 0,
		iCurrentMonth = 0,
		iCurrentDay = 0;

	CString	strCurrentYear = L"",
		strCurrentMonth = L"",
		strCurrentDay = L"",
		strVarDate = L"";


	//this sets the ole datetime object.
	COleDateTime oleDate = COleDateTime::GetCurrentTime();

	//gets all the date and time values.
	iCurrentMonth = oleDate.GetMonth();
	iCurrentDay = oleDate.GetDay();
	iCurrentYear = oleDate.GetYear();


	//formats all the returns from the date and time to a string
	strCurrentMonth.Format(L"%.2d", iCurrentMonth);
	strCurrentDay.Format(L"%.2d", iCurrentDay);
	strCurrentYear.Format(L"%.4d", iCurrentYear);

	//this formats and displays the date.
	strVarDate = strVarDate + strCurrentMonth + L"/" + strCurrentDay + L"/" + strCurrentYear;
	//prints the date
	Display.Log(L"Date", strVarDate, SendLogStatus);

}

//this displays a log entry that prints the Time
void PrintTimeLog()
{
	int 	iCurrentHour = 0,
		iCurrentMin = 0,
		iCurrentSec = 0;

	CString	strCurrentHour = L"",
		strCurrentMin = L"",
		strCurrentSec = L"",
		strVarTime = L"";

	//this sets the ole datetime object.
	COleDateTime oleDate = COleDateTime::GetCurrentTime();

	//gets the Time
	iCurrentHour = oleDate.GetHour();
	iCurrentMin = oleDate.GetMinute();
	iCurrentSec = oleDate.GetSecond();

	//formats the Time to 2 digits
	strCurrentHour.Format(L"%.2d", iCurrentHour);
	strCurrentMin.Format(L"%.2d", iCurrentMin);
	strCurrentSec.Format(L"%.2d", iCurrentSec);

	//this formats and displays the Time
	strVarTime = strVarTime + strCurrentHour + L":" + strCurrentMin + L":" + strCurrentSec;
	//prints the Time in the log File
	Display.Log(L"Time", strVarTime, SendLogStatus);
}

void PidTestStart(int iNumOPids = 0, float flSetpoint = 0) {
	//this function starts a bunch of pids.
	for (int i = 1; i <= iNumOPids; i++)
	{
		PID.PIDSetPoint(i, flSetpoint, 0);
		PID.PIDStart(i, i);
	}
}

void PidTestStop(int iNumOPids = 0) {
	//this function stops a bunch of pids.

	for (int i = 0; i <= iNumOPids; i++)
	{
		PID.PIDStop(i, true);
	}
}
bool ControllerPassFail(bool bPassFail)
{
	//defined globally.
	//bool ControllerPassOrFailFlag=true; //this flag is used to determine if the controller passes or fails.

	if (bPassFail == false)
		if (ControllerPassOrFailFlag == true)
			ControllerPassOrFailFlag = false;
	return ControllerPassOrFailFlag;
}




bool AlarmTestFns()
{

	bool bPassFlag = true;
	CString strYNVar = L"";

	//testing alarms
	Display.SnoozePeriod(60);
	Display.Alarm(2, 1, 16, L"This is a Test Alarm", SendLogStatus, -1, false);



	Display.Phase(L"Testing snooze.");
	Display.ResetSnooze();
	Sleep(15000);
	DM("Paused waiting for another alarm.", "should be 60+15 seconds.", "", 0, 0, "");







	Display.Display(20, L"Testing");
	Display.Display(21, L"Display.SnoozePeriod(60),Display.Alarm(),Display.CancleAlarm()");
	Display.Display(22, L"Display.ClearAlarm()");
	Display.Display(23, L"Please Acknowledge the First alarm only");
	Display.Display(24, L"");
	Display.Entry(0, 0, true);

	bool bLoopFlag = false;


	//this loop loopes forever until the alarm is acknowledged
	do //this Loop makes sure that the alarm channel 16 stays off for 5 seconds
	{
		bLoopFlag = false;

		for (int i = 1; i <= 20; i++)	//the 20 means that the alarm channel has to be off for 5 seconds 250ms*20=5000ms
		{
			if (DigitalIO.ReadDigitalOut(16) == 1)
			{
				bLoopFlag = true;
			}
			Sleep(250);
		}

	} while (bLoopFlag);

	PinesTime7.StartTimer();
	PinesTime7.ZeroTime();


	while (PinesTime7.GetElapsedTime() <= 60)
	{
		Sleep(250);
		strYNVar.Format(L"%d", 60 - PinesTime7.GetElapsedTime());
		Display.DisplayStatus(L"Please Wait " + strYNVar + L" Seconds");
	}


	Display.KillEntry();
	Display.CancelAlarm();
	PinesTime7.StopTimer();

	Display.Log(L"Display.Alarm(false);", L"Passed", SendLogStatus);
	Display.Log(L"Display.SnoozePeriod()", L"Passed", SendLogStatus);
	Display.Log(L"Display.CancleAlarm()", L"Passed", SendLogStatus);

	Display.Display(20, L"Please Acknowledge the Second Alarm");
	Display.Display(21, L"to clear the alarm window");
	Display.Display(22, L"");
	Display.Display(23, L"");
	Display.Display(24, L"");
	Display.DisplayStatus(L"Please wait...");
	Display.Entry(0, 0, true);

	//this loop loops forever until an alarm is acknowledged.
	do //this Loop makes sure that the alarm channel 16 stays off for 5 seconds
	{
		bLoopFlag = false;
		for (int i = 1; i <= 20; i++)	//the 20 means that the alarm channel has to be off for 5 seconds 250ms*20=5000ms
		{
			if (DigitalIO.ReadDigitalOut(16) == 1)
			{
				bLoopFlag = true;
			}
			Sleep(250);
		}
	} while (bLoopFlag);
	Display.KillEntry();
	Display.ClearAlarm(0);

	strYNVar = DE("", "", "Did the Alarm Window Clear?", NULL, NULL, "Please enter Y/N then press enter. Then Enter.");

	if (strYNVar.CompareNoCase(L"y"))
	{
		bPassFlag = false;
		SendLogStatus = DEVIATION;
		Display.Log(L"Clear Alarm Window Fn Doesnt work", L"Failed", SendLogStatus);
	}
	else
	{

		SendLogStatus = ABORT;
		Display.Log(L"All Alarm Functions Work Properly", L"Passed", SendLogStatus);

	}


	return bPassFlag;
}


bool CheckSecurity()
{

	//this waits till a valid Operator Password is entered
	while (!SecurityCheck(false))
	{
		Sleep(250);
	}

	//this waits till a valid Manager Password is entered
	while (!SecurityCheck(true))
	{
		Sleep(250);
	}

	return true;
}



bool SecurityCheck(bool bManagerPassCheck = false)
{	//iSecurityType this variable is if the function operates as manager check or operator check.
	int iAttempts = 1;
	long lPassFail = false;		//this is a boolean variable and only 1 and 0 are used. but the Display.IsSecurityValid() function
								//returns a long so im using a long but only using true or false in it.

	do {
		if (bManagerPassCheck)
			Display.ManagerPass();

		if (!bManagerPassCheck)
			Display.OperatorPass();

		WaitForEnterKey();	//NOTE: this must be here to capture the enter key that the functions Display.ManagerPass and Operator Pass enter into the input stream.	

		lPassFail = Display.IsSecurityValid();

		if ((iAttempts >= 3) && !lPassFail)
		{
			Display.Log(L"Multiple Login Attempts Failed", L"", SendLogStatus);
			DM("Security Problem!", "Multiple Login Attempts Failed", "Call Your System Administrator", NULL, NULL, "Press Enter To Continue...");
			return false;	//means security check failed.
		}

		if ((iAttempts < 3) && !lPassFail)
		{
			DM("Security Problem!", "Please Contact", "Your System Administrator.", NULL, NULL, "Press Enter To Try Again.");

		}

		iAttempts++;

	} while (!lPassFail);
	return true;			//means security check validated.
}// end of function SecurityCheck(bool)

void PrintProdType(int iProdNumfn)
{
	long lProdTypefn = 0;
	CString strRecipeNumber = L"";

	lProdTypefn = Display.GetProductType(iProdNumfn);

	//these set up the autoprint header should typically be set after recipe is set via PV.setproduct.
	Display.SetCookTemp(L"252.55 °F");
	Display.SetCookTime(L"5:32 Min");
	Display.SetInitialTemp(170.25);

	strRecipeNumber.Format(L"%d", iProdNumfn);

	switch (lProdTypefn)
	{
	case -1:
	{
		Display.Log(L"No Product", strRecipeNumber);
		break;
	}
	case 0:
	{
		Display.Log(L"BALL Recipe selected", strRecipeNumber);
		PV.SetProduct(iProdNumfn);
		break;
	}
	case 1:
	{
		Display.Log(L"GENERIC Recipe selected", strRecipeNumber);
		PV.SetProduct(iProdNumfn);
		break;
	}
	case 2:
	{
		Display.Log(L"NUMERICAL Recipe selected", strRecipeNumber);
		PV.SetProduct(iProdNumfn);
		break;
	}
	case 3:
	{
		Display.Log(L"TUNACAL Recipe selected", strRecipeNumber);
		PV.SetProduct(iProdNumfn);
		break;
	}
	default:
	{
		Display.Log(L"Invalid Return from function.", strRecipeNumber);
		break;
	}
	}//end of case switch prod type



}//end of function PrintProdType();

void TestTimers()
{
	Display.Phase(L"Timers");

	//this function tests timers,inverse timers, hide display, and display variable;
	Display.HideDisplay(13);
	Display.HideDisplay(14);
	Display.HideDisplay(15);
	Display.HideDisplay(16);

	PinesTime7.StartTimer();
	PinesTime7.ZeroTime();

	Display.Log(L"Timers", L"Started", SendLogStatus);
	Display.DisplayTimer(13, L"PinesTime7 up", (long)&PinesTime7);
	Display.DisplayInverseTimer(14, L"PinesTime7 dwn", 60, (long)&PinesTime7);



	DM("Stop Timer", "", "", 0, 0, "");
	PinesTime7.StopTimer();
	Display.Log(L"PinesTime7 Timer", L"Stopped", SendLogStatus);
	Sleep(5000);

	DM("Start Timer", "", "", 0, 0, "");
	PinesTime7.StartTimer();
	Display.Log(L"PinesTime7 Timer Resumed?", L"Started R", SendLogStatus);
	Sleep(5000);

	DM("zero timer", "", "", 0, 0, "");
	PinesTime7.ZeroTime();
	Display.Log(L"PinesTime7 Timer zeroed?", L"Zeroed", SendLogStatus);

	DM("Testing ", "Display.GetElapsedTimeInString()", "in the status", 0, 0, "---");
	while (PinesTime7.GetElapsedTime() < 90)
	{

		Display.DisplayStatus(PinesTime7.GetElapsedTimeInString());
		Sleep(250);
	}

	DL("PhaseTimer Time in seconds", "", "", 0, Display.GetPhaseTimer(), "");
}
void PrintMemoryInfo()
{
	DWORD dMemUsage;
	CString cs, logTESTvar;
	MEMORYSTATUS	MemStatus;

	MemStatus.dwLength = sizeof(MemStatus);

	GlobalMemoryStatus(&MemStatus);
	dMemUsage = MemStatus.dwTotalPhys - MemStatus.dwAvailPhys;

	cs.Format(_T("Amount of memory used %.3f MB "), (dMemUsage /
		(1024.0 * 1024)));
	OutputDebugString(cs);



	logTESTvar.Format(_T(" %.3f MB "), ((dMemUsage - mem) /
		(1024.0 * 1024)));

	//	DL("dMemusage= ","","",0,dMemUsage,"");

	//	DL("mem= ","","",0,mem,"");

	//	if  ( (dMemUsage < (mem-1000)) ||
	//		  (dMemUsage > (mem+1000))
	//		)
	//	{
	Display.Log(cs, logTESTvar);
	//	}

	mem = dMemUsage;


}

void CheckTimeDiffLampsLoop()
{
	Display.Log(L"Entered Time Difference Checker");
	double dTick;
	CString strTimes;

	dTick = GetTickCount();

	while (bF8Running && !BKillTheTask) //!Display.IsEnterKeyPressed())
	{
		if ((dTick + 500) < (GetTickCount()))
		{
			strTimes.Format(L"TIME DELAY = %.3f Seconds", (float)((GetTickCount() - dTick)) / 1000);
			Display.Log(strTimes);
		}
		dTick = GetTickCount();
		Sleep(25);
	}

	bF8Running = false;
}



void DOLampsLooping()
{
	// DigitalIO.SetDigitalOut(iNumberOfLights -iLight, Off);
//	bFnRunning=true;
	Display.Log(L"Entered Time Lamps Looping Checker");
	Display.Log(L"Adjust Analog Input #1 for Loop Speed");

	int iLightSpeed = (int)AnalogIO.ReadAnalogIn(1);

	iLightSpeed < 10 ? iLightSpeed = 10 : iLightSpeed;

	int iLight = 1;

	int iNumberOfLights = 16;
	//iLightSpeed = 10;
	bF8Running = true;
	BKillTheTask = false;
	bKillDoLampsLooping = false;



	while (bF8Running && !BKillTheTask && !bKillDoLampsLooping) // !Display.IsEnterKeyPressed())
	{
		iNumberOfLights < 1 ? iNumberOfLights = 1 : iNumberOfLights = iNumberOfLights;

		AnalogIO.ReadAnalogIn(1) < 100 ? iLightSpeed = 100 : iLightSpeed = (10 * (int)AnalogIO.ReadAnalogIn(1));

		while (iLight <= iNumberOfLights)    // This loop makes the lights scroll from left to right
		{
			DigitalIO.SetDigitalOut(iLight, Off);
			DigitalIO.SetDigitalOut(++iLight, Onn);
			Sleep(iLightSpeed);
		}

		while (iLight > 1) //iNumberOfLights)           // This loop makes the lights scroll from right to left
		{
			DigitalIO.SetDigitalOut(iLight, Off);
			DigitalIO.SetDigitalOut(--iLight, Onn);
			Sleep(iLightSpeed);
		}
	}
	bF8Running = false;
}