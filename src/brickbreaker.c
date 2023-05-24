/*
Copyright 2023 Shaun Nicholson - 3DOHD

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// 
//	Brick Breaker main game
//

*/

#include "brickbreaker.h" 
#include "celutils.h" 
#include "HD3DO.h"
#include "HD3DOAudioSFX.h"
#include "HD3DOAudioSoundInterface.h"



void initSPORT();
void initGraphics();
void initSystem();
void loadData();
void GameLoop();
void InitGame();
void InitNewLevel(int level);
void HandleInput(); 
void HandleTimer();
void MoveLeft();
void MoveRight();
void MoveDown();
void Decelerate();
void Move();
void UpdateGameStats();
void DisplayStartScreen();
void DisplayGameplayScreen();
void DisplayGameOverScreen(CCB *celBG, CCB *celFG);
void AnimateLostLife();
void AnimateNextLevel();
void AnimatePlayer1();
void AnimateStars();
void AnimatePlanets();
void AnimateLasers();
void AnimateBalls();
void AnimateBricks();
void AnimateBrickExplosions();
void AnimateLevelComplete();
void HandleAnimation();
void HandleBrickCollision(int idx, int damage);
void TrackPowerUps();
void ShootLaserLeft();
void ShootLaserRight();
void ShootBall(int speedX, int speedY);
bool CheckCollisionLasers(int x, int y);
void ApplySelectedColorPalette();
void SwapBackgroundImage(char *file, int imgIdx);
void ShowGameOver();
void ShowVictory();

void ShowIntroSplash();
void PlaySFX(int id);
void PlayBackgroundMusic();

void InitCCBFlags(CCB *cel); // Lives in HD3DO.c

/* ----- GAME VARIABLES -----*/

static ScreenContext screen;

static Item bitmapItems[SCREEN_PAGES];
static Bitmap *bitmaps[SCREEN_PAGES];

static Item VRAMIOReq;
static Item vsyncItem;
static IOInfo ioInfo;

static DebugData dData;

bool ShowDebugStats = false;

static int visibleScreenPage = 0; 

static int counter1 = 0;
static int debugMode = 0;

static int LVDATA[20][50] =
{
	{  0,  1,  2,  3,  4,  5,  6,  0,  1,  2,	 6,  0,  1,  2,  3,  4,  5,  6,  0,  1,    5,  6,  0,  1,  2,  3,  4,  5,  6,  0,    4,  5,  6,  0,  1,  2,  3,  4,  5,  6,    3,  4,  5,  6,  0,  1,  2,  3,  4,  5 },
	{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,	 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{  4, -1,  4, -1,  4, -1,  4, -1,  4, -1,	-1,  5, -1,  5, -1,  5, -1,  5, -1,  5,    4, -1,  4, -1,  4, -1,  4, -1,  4, -1,	-1,  5, -1,  5, -1,  5, -1,  5, -1,  5,   15, 15, 15, 15, 15, 15, 15, 15, 15, 15 },
	{ 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,   14, 14, 14, 14, 14, 14, 14, 14, 14, 14,   14, 14, 14, 14, 14, 14, 14, 14, 14, 14,   14, 14, 14, 14, 14, 14, 14, 14, 14, 14,   14, 14, 14, 14, 14, 14, 14, 14, 14, 14 },
	{  8,  6,  6,  6,  6,  6,  6,  6,  6,  8,    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,    8,  4,  4,  4,  4,  4,  4,  4,  4,  8,    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,    8,  2,  2,  2,  2,  2,  2,  2,  2,  8 },
	
	{  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,	 6,  6,  6,  6,  6,  7,  7,  7,  7,  7,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,    7,  7,  7,  7,  7,  6,  6,  6,  6,  6,    5,  5,  5,  5,  5,  4,  4,  4,  4,  4 },
	{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	11, 11,  8,  8,  8,  8,  8,  8, 11, 11,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,   11, 11,  8,  8,  8,  8,  8,  8, 11, 11,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{  5,  6,  7,  9,  9,  9,  9,  7,  6,  5,	 5,  6,  7,  9,  9,  9,  9,  7,  6,  5,    5,  6,  7,  9,  9,  9,  9,  7,  6,  5,    5,  6,  7,  9,  9,  9,  9,  7,  6,  5,    5,  6,  7,  9,  9,  9,  9,  7,  6,  5 },
	{ 13, -1, 13, -1, 13, -1, 13, -1, 13, -1,	-1, 13, -1, 13, -1, 13, -1, 13, -1, 13,   13, -1, 13, -1, 13, -1, 13, -1, 13, -1,   -1, 13, -1, 13, -1, 13, -1, 13, -1, 13,   13, -1, 13, -1, 13, -1, 13, -1, 13, -1 },
	{  0,  1,  2,  3,  4,  5,  6,  0,  1,  2,	 6,  0,  1,  2,  3,  4,  5,  6,  0,  1,    5,  6,  0,  1,  2,  3,  4,  5,  6,  0,    4,  5,  6,  0,  1,  2,  3,  4,  5,  6,    3,  4,  5,  6,  0,  1,  2,  3,  4,  5 },
	
	{  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,	11, 11, 11, 11, 11, 11, 11, 11, 11, 11,    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,   11, 11, 11, 11, 11, 11, 11, 11, 11, 11,    1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
	{ 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,	 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   12, 12, 12, 12, 12, 12, 12, 12, 12, 12 },
	{ 10,  0, 10,  0, 10,  0, 10,  0, 10,  0,	 0, 10,  0, 10,  0, 10,  0, 10,  0, 10,   10, -1, -1, -1, -1, -1, -1, -1, -1, 10,    0, 10,  0, 10,  0, 10,  0, 10,  0, 10,   10,  0, 10,  0, 10,  0, 10,  0, 10,  0 },
	{  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,	 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,    5,  5,  5,  5,  5,  5,  5,  5,  5,  5 },
	{  3,  4,  3,  4,  3,  4,  3,  4,  3,  4,	 4,  3,  4,  3,  4,  3,  4,  3,  4,  3,    3,  4,  3,  4,  3,  4,  3,  4,  3,  4,    4,  3,  4,  3,  4,  3,  4,  3,  4,  3,    3,  4,  3,  4,  3,  4,  3,  4,  3,  4 },
	
	{ 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,	13, 13, 13, 13, 13, 13, 13, 13, 13, 13,   13, -1, -1, -1, -1, -1, -1, -1, -1, 13,   13, 13, 13, 13, 13, 13, 13, 13, 13, 13,   13, 13, 13, 13, 13, 13, 13, 13, 13, 13 },
	{  0,  1,  2,  1,  0,  1,  2,  1,  0,  1,	 2,  1,  0,  1,  2,  1,  0,  1,  2,  1,    0,  1,  2,  1,  0,  1,  2,  1,  0,  1,	 2,  1,  0,  1,  2,  1,  0,  1,  2,  1,    0,  1,  2,  1,  0,  1,  2,  1,  0,  1 },
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,	 7,  7,  7,  7,  7,  7,  7,  7,  7,  7,    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,    9,  9,  9,  9,  9,  9,  9,  9,  9,  9 },
	{ 15, -1, 15, -1, 15, 15, -1, 15, -1, 15,	15, -1, 15, -1, 15, 15, -1, 15, -1, 15,   15, -1, 15, -1, 15, 15, -1, 15, -1, 15,   15, -1, 15, -1, 15, 15, -1, 15, -1, 15,   15, -1, 15, -1, 15, 15, -1, 15, -1, 15 },
	{ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,   10, 10, 10, 10, 10, 10, 10, 10, 10, 10,   10, 10, 10, 10, 10, 10, 10, 10, 10, 10,   10, 10, 10, 10, 10, 10, 10, 10, 10, 10,    8,  8, -1,  8,  8,  8,  8, -1,  8,  8 }
};

/*
	0 - Ball 1
	1 - Ball 2
	2 - Ball 3
	3 - Min Idx
	4 - Max Idx
	5 - BG Planet 1
	6 - BG Planet 2
	7 - Brick HP Multiplier
	8 - Timer
*/
static int LVMETA[20][9] = 
{
	{ 1, 0, 1,   0,  6,   6,  1,  1, 120 },
	{ 1, 0, 1,   0,  3,   4, -1,  1,  90 },
	{ 1, 0, 1,   0,  6,   3, -1,  1,  90 },
	{ 1, 0, 1,   0,  6,   2, -1,  1,  90 },
	{ 1, 0, 1,   0,  7,   5, -1,  1,  90 },
			 	  	 	 	 
	{ 1, 0, 1,   4,  7,   1,  4,  1,  60 },
	{ 0, 0, 0,  -1, -1,  -1,  6,  1,  30 },
	{ 1, 0, 0,  -1, -1,   0,  6,  1,  60 },
	{ 1, 0, 1,   0,  3,   4, -1,  2,  90 },
	{ 1, 0, 1,   0,  6,   6,  1,  2, 120 },
	
	{ 0, 0, 1,  -1, -1,   6,  1,  1,  75 },
	{ 1, 0, 1,  -1, -1,   4,  0,  1,  75 },
	{ 0, 0, 1,  -1, -1,   1, -1,  1,  75 },
	{ 1, 0, 0,  -1, -1,   2, -1,  1,  75 },
	{ 1, 0, 1,   3,  4,   -1, 4,  1,  75 },
	
	{ 0, 0, 1,  -1, -1,  -1,  6,  1,  60 },
	{ 1, 0, 1,   0,  2,  -1, -1,  1,  60 },
	{ 0, 0, 1,  -1, -1,   4,  2,  1,  60 },
	{ 1, 0, 0,  -1, -1,   3, -1,  1,  60 },
	{ 1, 0, 1,  -1, -1,   -1, 6,  3, 120 }
};
// Joypad stuff

bool kpLeft = false;
bool kpRight = false;
bool kpUp = false;
bool kpDown = false;
bool kpLS = false;
bool kpRS = false;
bool kpA = false;
bool kpB = false;
bool kpC = false;
bool kpStart = false;
bool kpStop = false;

int swLeft = 0;
int swRight = 0;
int swUp = 0;
int swDown = 0;
int swLS = 0;
int swRS = 0;
int swA = 0;
int swB = 0;
int swC = 0;

static bool ShowingIntroAnimation = false;
static bool SkipIntro = false;

static int lastElapsedTime = 0;

static int CurrScore = 0;
static int CurrTarget = 0;
static int Counter1 = 0;
static int CounterFire = 0;

static int CurrLostBalls = 0;

static int AvailTime = 0;
static int CurrElapsedTime = 0;

static int AvailBalls = 5;
static int AvailLasers = 99;
static int AvailLives = 3;
static int TotalScore = 0;

static int TimerBalls = 0;
static int TimerLasers = 0;

//

const int FIRE_OFFSET_X = 11;
const int FIRE_OFFSET_Y = 17;
const int LASER_OFFSET_X = 2;
const int LASER_OFFSET_Y = 2;
const int P1_WIDTH = 60;
const int BRICK_WIDTH = 28;
const int BRICK_HEIGHT = 8;

//

static int P1_X = 320 / 2 - 26;
static int P1_Y = 260;
static int CounterBricks = 0;
static int CounterHover = 0;

static int P1_Speed = 0;

static bool GameOver = false;
static int CurrLevel = 1;

//

// TODO LOAD ALL PLANETS

static CCB *cel_AllBricks[16]; // Load all potential game block data
static CCB *cel_AllBalls[2];

CCB *cel_Start1;
CCB *cel_Start2;
CCB *cel_Start3;

CCB *cel_Top;
CCB *cel_Left;
CCB *cel_Right;
CCB *cel_Bottom;

CCB *cel_Player1;

CCB *cel_LvlComplete;

CCB *cel_Text1;
CCB *cel_Text2;

Planet planets[7];

Brick bricks[50]; 
Star stars[24]; // Background stars

Laser lasersL[8];
Laser lasersR[8];

Ball balls[3];

static int LastLaserL = -1;
static int LastLaserR = -1;

int getFrameNum = 0;

static bool CurrLevelComplete = true;

TimeVal tvLastBall;

static ubyte *backgroundBufferPtr1 = NULL; // Pointer to IMAG file passed to the SPORT buffer for the background image

static int sfxInit = 0;
static int sfxLoad = 0;

static bool GameStarted = false;
static bool GamePaused = false;


int main() // Main entry point
{
	initSystem(); // Required system inits
	initGraphics(); // Required graphic inits

	InitNumberCels(8); // 3DOHD Initialize 6 sets of number CCBs for chaining
	
	//initTools();

	InitNumberCel(0, 40, 11, 0, false);
	InitNumberCel(1, 98, 11, 0, false);
	InitNumberCel(2, 155, 11, 0, false);
	InitNumberCel(3, 216, 11, 0, false);	
	InitNumberCel(4, 230, 11, 0, true);
	
	InitNumberCel(5, 164, 221, 0, false); // Bottom Score
	
	InitNumberCel(6, -200, -200, false);
	InitNumberCel(7, -200, -200, false);
	
	loadData(); // Load and initialize CELs

	initSPORT(); // Turn the background black
	
	sfxInit = initsound(); // Initialize the EFMM Sound Library
	sfxLoad = loadsfx(); // In theory I can spool from here also 
	
	PlayBackgroundMusic();	
		
	GameLoop(); // The actual game play happens here - Nothing above gets called again
}

void InitGame()
{
	int x;
	
	ResetCelNumbers();
	
	FadeFromBlack( &screen, 1 );
	
	CurrLevel = 1;
	
	CurrScore = 0;
	
	AvailBalls = 5;
	AvailLasers = 99;
	AvailLives = 3;
	
	// comet.X = 340;
	// comet.Y = 50;
	
	GameStarted = false;
	GamePaused = false;
	GameOver = false;
	
	SampleSystemTimeTV(&dData.TvInit);				// Used for timing and performance benchmarking
	SampleSystemTimeTV(&dData.TvFrames30Start);		// Used for timing and performance benchmarking
	SampleSystemTimeTV(&tvLastBall);
}

void InitNewLevel(int level)
{
	/*
	0 - Ball 1
	1 - Ball 2
	2 - Ball 3
	3 - Min Idx
	4 - Max Idx
	5 - BG Planet 1
	6 - BG Planet 2
	7 - Brick HP Multiplier
	8 - Timer
	*/

	int x, p1, p2, bType;
	int lvB1, lvB2, lvB3, lvMinIdx, lvMaxIdx, lvP1, lvP2, lvHpMult, lvTime;
	
	int l = level - 1;
	
	char str[14];
	
	P1_X = 320 / 2 - 26;
	P1_Y = 260;

	P1_Speed = 0;
	
	CurrScore = 0;
	CurrTarget = 0; 

	sprintf(str, "data/bg_%d.img", ((level + 19) % 20) + 1);
	
	SwapBackgroundImage(str, level);
	
	lvB1 = LVMETA[l][0];
	lvB2 = LVMETA[l][1];
	lvB3 = LVMETA[l][2];
	
	lvMinIdx = LVMETA[l][3];
	lvMaxIdx = LVMETA[l][4];
	
	lvP1 = LVMETA[l][5];
	lvP2 = LVMETA[l][6];
	
	lvHpMult = LVMETA[l][7];
	
	lastElapsedTime = 0;
	CurrElapsedTime = 0;
	
	AvailTime = LVMETA[l][8];
	
	for (x = 0; x < 3; x++)
	{
		balls[x].Active = false;
		balls[x].Damage = 0;
		balls[x].cel->ccb_SourcePtr = cel_AllBalls[0]->ccb_SourcePtr;
		
		SetFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB1 == 1)
	{
		balls[0].Active = true;
		
		balls[0].X = 28;
		balls[0].Y = 24;
		
		balls[0].SpeedX = 2;
		balls[0].SpeedY = 2;
		
		ClearFlag(balls[0].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB2 == 1)
	{
		balls[1].Active = true;
		
		balls[1].X = 157;
		balls[1].Y = 24;
		
		balls[1].SpeedX = 0;
		balls[1].SpeedY = 4;
		
		ClearFlag(balls[1].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB3 == 1)
	{
		balls[2].Active = true;
		
		balls[2].X = 284;
		balls[2].Y = 24;
		
		balls[2].SpeedX = -2;
		balls[2].SpeedY = 2;
		
		ClearFlag(balls[2].cel->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 7; x++)
	{
		planets[x].Active = false;
		
		SetFlag(planets[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvP1 >= 0)
	{
		planets[lvP1].Active = true;
		
		planets[lvP1].X = planets[lvP1].InitX + ((rand() % 20) - 10);
		planets[lvP1].Y = planets[lvP1].InitY + ((rand() % 20) - 10);
		
		ClearFlag(planets[lvP1].cel->ccb_Flags, CCB_SKIP);
		
		PositionCel(planets[lvP1].cel, planets[lvP1].X, planets[lvP1].Y);
	}
	
	if (lvP2 >= 0)
	{
		planets[lvP2].Active = true;
		
		planets[lvP2].X = planets[lvP2].InitX + ((rand() % 20) - 10);
		planets[lvP2].Y = planets[lvP2].InitY + ((rand() % 20) - 10);
		
		ClearFlag(planets[lvP2].cel->ccb_Flags, CCB_SKIP);
		
		PositionCel(planets[lvP2].cel, planets[lvP2].X, planets[lvP2].Y);
	}
	
	for (x = 0; x < 50; x++) 
	{
		bType = LVDATA[level - 1][x];
		
		if (bType == -1)
		{
			bricks[x].Active = false;
			bricks[x].HP = 0;
			
			SetFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);
		}
		else
		{
			bricks[x].ImageIdx = bType;
			
			bricks[x].cel->ccb_SourcePtr = cel_AllBricks[bricks[x].ImageIdx]->ccb_SourcePtr;
			
			if (lvHpMult == 0)
			{
				bricks[x].HP = 1;
			}
			else
			{
				if (bType >= 10)
				{
					bricks[x].HP = 10;
				}
				else
				{
					bricks[x].HP = 5 * lvHpMult;
				}
			}
			
			if (bType == 8) // White brick
			{
				bricks[x].HP = 9999999;
			}
			else
			{
				CurrTarget++;
			}
			
			bricks[x].Active = true;
			bricks[x].Animate = bricks[x].ImageIdx <= 7;
			bricks[x].ExplodeFrame = 0;
			
			if (bType < 10)
			{
				bricks[x].AnimMinIdx = lvMinIdx;
				bricks[x].AnimMaxIdx = lvMaxIdx; 
			}
			else
			{
				bricks[x].AnimMinIdx = -1;
				bricks[x].AnimMaxIdx = -1; 
			}
			
			bricks[x].X = bricks[x].InitX;
			bricks[x].Y = bricks[x].InitY;	
				
			bricks[x].cel->ccb_XPos = Convert32_F16(bricks[x].InitX);
			bricks[x].cel->ccb_YPos = Convert32_F16(bricks[x].InitY);
				
			bricks[x].cel->ccb_HDX  = Convert32_F16(1) << 4;
			bricks[x].cel->ccb_VDY  = Convert32_F16(1);
			
			ClearFlag(bricks[x].cel->ccb_Flags, CCB_MARIA);
			ClearFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);			
		}
	}
	
	for (x = 0; x < 8; x++)
	{
		lasersL[x].Active = false;
		lasersL[x].Y = 240; // Past gameplay screen
		
		lasersR[x].Active = false;
		lasersR[x].Y = 240; // Past gameplay screen
		
		SetFlag(lasersL[x].cel->ccb_Flags, CCB_SKIP);
		SetFlag(lasersR[x].cel->ccb_Flags, CCB_SKIP);
	}	
	
	CurrLevelComplete = true; // Don't fight with P1 Animation
	
	UpdateGameStats();
}

void GameLoop()
{	
	while (true)
	{		
		InitGame();		

		ShowIntroSplash();	
		
		InitEventUtility(1, 0, LC_Observer);
		
		while (GameStarted == false)
		{
			HandleInput();
			
			DisplayStartScreen();
		}
		
		KillEventUtility();	
		
		while (GameOver == false && CurrLevel <= 20)
		{
			InitNewLevel(CurrLevel);

			AnimateNextLevel();
				
			CurrLevelComplete = false;
			
			InitEventUtility(1, 0, LC_Observer); 
			
			while (GameOver == false && CurrScore < CurrTarget) // In a real game some game over condition would break the loop back to the main start screen
			{			
				HandleInput();
				
				if (GamePaused == true) continue;
				
				HandleAnimation();
				
				HandleTimer();
				
				UpdateGameStats();
				
				TrackPowerUps();

				DisplayGameplayScreen();
				
				if (CurrElapsedTime > AvailTime)
				{
					GameOver = true;
				}
				else if (AvailBalls == 0 && balls[0].Active == false && balls[1].Active == false && balls[2].Active == false)
				{
					GameOver = true;
				}
			}
			
			if (GameOver == true)
			{
				if (--AvailLives >= 0)
				{	
					if (AvailBalls < 5) AvailBalls = 5;
					if (AvailLasers < 99) AvailLasers = 99;
	
					GameOver = false;
					
					AnimateLostLife();
				}
				else
				{
					AvailLives = 0;
				}
			}
			else
			{				
				CurrLevelComplete = true;
				
				AnimateLevelComplete();
				
				CurrLevel++;
			}
			
			KillEventUtility();		
		}
		
		if (GameOver == true || CurrLevel < 21)
		{
			ShowGameOver();
		}
		else
		{
			ShowVictory();
		}
	}
}

void HandleAnimation()
{
	AnimatePlanets();
	AnimateBalls();
	AnimateBricks();
	AnimatePlayer1();
	AnimateStars();
	AnimateLasers();
	AnimateBrickExplosions();
}

void HandleTimer() // TODO ADD TIME FOR PAUSE
{
	TimeVal tvElapsed, tvNow;
	
	SampleSystemTimeTV(&tvNow);
	
	SubTimes(&dData.TvInit, &tvNow, &tvElapsed);
	
	lastElapsedTime = CurrElapsedTime;
	
	if (tvElapsed.tv_Seconds > lastElapsedTime) // Random Emulator bug
	{
		CurrElapsedTime = tvElapsed.tv_Seconds;
	}
}

void TrackPowerUps()
{
	if (++TimerBalls > (60 * 60))
	{
		TimerBalls = 0;
		
		if (AvailBalls < 24) AvailBalls++;
	}
	
	if (++TimerLasers > 60)
	{
		TimerLasers = 0;
		
		if (AvailLasers < 99) AvailLasers++;
	}
}


void loadData()
{
	int x, y, z;
	uint32 sColor;
	
	cel_Start1 = LoadCel("data/titlemain.cel", MEMTYPE_CEL);
	cel_Start2 = LoadCel("data/titlesb.cel", MEMTYPE_CEL);
	cel_Start3 = LoadCel("data/titlestart.cel", MEMTYPE_CEL);	
	
	InitCCBFlags(cel_Start1);
	InitCCBFlags(cel_Start2);
	InitCCBFlags(cel_Start3);
	
	cel_Start1->ccb_NextPtr = cel_Start2;
	cel_Start2->ccb_NextPtr = cel_Start3;
	cel_Start3->ccb_NextPtr = NULL;
	
	SetFlag(cel_Start3->ccb_Flags, CCB_LAST);
	
	PositionCel(cel_Start1, 40, 20);
	PositionCel(cel_Start2, 50, 72);
	PositionCel(cel_Start3, 20, 180);
	
	cel_AllBricks[0] = LoadCel("data/brick1.cel", MEMTYPE_CEL); 
	cel_AllBricks[1] = LoadCel("data/brick2.cel", MEMTYPE_CEL); 
	cel_AllBricks[2] = LoadCel("data/brick3.cel", MEMTYPE_CEL);	
	cel_AllBricks[3] = LoadCel("data/brick4.cel", MEMTYPE_CEL); 
	cel_AllBricks[4] = LoadCel("data/brick5.cel", MEMTYPE_CEL); 
	cel_AllBricks[5] = LoadCel("data/brick6.cel", MEMTYPE_CEL); 	
	cel_AllBricks[6] = LoadCel("data/brick7.cel", MEMTYPE_CEL); 
	cel_AllBricks[7] = LoadCel("data/brick8.cel", MEMTYPE_CEL);
	cel_AllBricks[8] = LoadCel("data/brick9.cel", MEMTYPE_CEL);	
	cel_AllBricks[9] = LoadCel("data/brick10.cel", MEMTYPE_CEL); 
	cel_AllBricks[10] = LoadCel("data/brick11.cel", MEMTYPE_CEL); 
	cel_AllBricks[11] = LoadCel("data/brick12.cel", MEMTYPE_CEL); 
	cel_AllBricks[12] = LoadCel("data/brick13.cel", MEMTYPE_CEL); 
	cel_AllBricks[13] = LoadCel("data/brick14.cel", MEMTYPE_CEL); 
	cel_AllBricks[14] = LoadCel("data/brick15.cel", MEMTYPE_CEL); 
	cel_AllBricks[15] = LoadCel("data/brick16.cel", MEMTYPE_CEL);	
	
	for (x = 0; x < 16; x++)
	{		
		InitCCBFlags(cel_AllBricks[x]);
	}
	
	cel_AllBalls[0] = LoadCel("data/ball_blue.cel", MEMTYPE_CEL);
	cel_AllBalls[1] = LoadCel("data/ball_red.cel", MEMTYPE_CEL);
	
	for (x = 0; x < 2; x++)
	{		
		InitCCBFlags(cel_AllBalls[x]);
	}
	
	planets[0].cel = LoadCel("data/planet7.cel", MEMTYPE_CEL);  // Sun
	planets[1].cel = LoadCel("data/planet1.cel", MEMTYPE_CEL);  // Sm Red Mercury
	planets[2].cel = LoadCel("data/planet2.cel", MEMTYPE_CEL);  // Large Blue
	planets[3].cel = LoadCel("data/planet3.cel", MEMTYPE_CEL);  // Sm Grey
	planets[4].cel = LoadCel("data/planet4.cel", MEMTYPE_CEL);  // Sm Rings
	planets[5].cel = LoadCel("data/planet5.cel", MEMTYPE_CEL);  // Large Green
	planets[6].cel = LoadCel("data/planet6.cel", MEMTYPE_CEL);  // Med Earth
	
	for (x = 0; x < 7; x++)
	{		
		InitCCBFlags(planets[x].cel);
		
		planets[x].Active == false;
		
		planets[x].FramesX = 0;
		planets[x].FramesY = 0;
	}
	
	planets[0].InitX = 80;
	planets[0].InitY = 42;
	planets[0].DelayX = 960;
	planets[0].DelayY = 12000;
	
	planets[1].InitX = 140;
	planets[1].InitY = 80;
	planets[1].DelayX = 30;
	planets[1].DelayY = 1024;
	
	planets[2].InitX = 260;
	planets[2].InitY = 70;
	planets[2].DelayX = 42;
	planets[2].DelayY = 480;
	
	planets[3].InitX = 240;
	planets[3].InitY = 40;
	planets[3].DelayX = 90;
	planets[3].DelayY = 360;
	
	planets[4].InitX = 210;
	planets[4].InitY = 90;
	planets[4].DelayX = 90;
	planets[4].DelayY = 12000;
	
	planets[5].InitX = 280;
	planets[5].InitY = 120;
	planets[5].DelayX = 24;
	planets[5].DelayY = 120;
	
	planets[6].InitX = 180;
	planets[6].InitY = 140;
	planets[6].DelayX = 36;
	planets[6].DelayY = 12000;
	
	for (x = 1; x < 7; x++)
	{
		planets[x - 1].cel->ccb_NextPtr = planets[x].cel;
	}
	
	for (x = 0; x < 24; x++)
	{
		y = rand() % 10;
		
		switch (y)
		{
			case 0:
				sColor = MakeRGB15(31, 0, 0);
				break;
				
			case 1:
				sColor = MakeRGB15(0, 31, 31);
				break;
				
			case 2:
				sColor = MakeRGB15(31, 31, 0);
				break;
				
			case 3:
				sColor = MakeRGB15(20, 20, 20);
				break;
				
			case 4:
				sColor = MakeRGB15(25, 25, 25);
				break;
				
			default:
				sColor = MakeRGB15(31, 31, 31);
				break;
		}
		
		stars[x].cel = CreateBackdropCel(1, 1, sColor, 100);
		
		stars[x].X = rand() % 340;
		stars[x].Y = rand() % 240;
		
		stars[x].Speed = 1 + (rand() % 4);
	}
	
	for (x = 0; x < 24; x++)
	{		
		ClearFlag(stars[x].cel->ccb_Flags, CCB_SKIP);
		ClearFlag(stars[x].cel->ccb_Flags, CCB_LAST);
		
		PositionCel(stars[x].cel, stars[x].X, stars[x].Y);
	}
	
	for (x = 1; x < 24; x++)
	{
		stars[x - 1].cel->ccb_NextPtr = stars[x].cel;
	}
	
	for (x = 0; x < 8; x++)
	{
		lasersL[x].cel = CreateBackdropCel(1, 8, MakeRGB15(3, 31, 31), 100);
		lasersR[x].cel = CreateBackdropCel(1, 8, MakeRGB15(3, 31, 31), 100);
		
		ClearFlag(lasersL[x].cel->ccb_Flags, CCB_LAST);
		ClearFlag(lasersR[x].cel->ccb_Flags, CCB_LAST);
	}
	
	for (x = 1; x < 8; x++)
	{
		lasersL[x - 1].cel->ccb_NextPtr = lasersL[x].cel;
		lasersR[x - 1].cel->ccb_NextPtr = lasersR[x].cel;
	}
	
	lasersL[7].cel->ccb_NextPtr = lasersR[0].cel;
	
	for (x = 0; x < 3; x++)
	{
		balls[x].cel = CopyCel(cel_AllBalls[0]);
		balls[x].Damage = 0;
		balls[x].Active = false;
		balls[x].X = 0;
		balls[x].Y = 240;
		balls[x].SpeedX = 0;
		balls[x].SpeedY = -1;
		
		SetFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 1; x < 3; x++)
	{
		balls[x - 1].cel->ccb_NextPtr = balls[x].cel;
	}
	
	cel_Player1 = LoadCel("data/p1.cel", MEMTYPE_CEL);
	InitCCBFlags(cel_Player1);
	
	PositionCel(cel_Player1, P1_X, P1_Y);
	
	cel_Text1 = LoadCel("data/txt_top.cel", MEMTYPE_CEL);
	InitCCBFlags(cel_Text1);
	
	PositionCel(cel_Text1, 23, 10);
	
	cel_Text2 = LoadCel("data/txt_bottom.cel", MEMTYPE_CEL);
	InitCCBFlags(cel_Text2);
	
	PositionCel(cel_Text2, 112, 222);
	
	cel_Top = LoadCel("data/wall_t.cel", MEMTYPE_CEL);
	cel_Left = LoadCel("data/wall_l.cel", MEMTYPE_CEL);
	cel_Right = LoadCel("data/wall_r.cel", MEMTYPE_CEL);
	cel_Bottom = LoadCel("data/wall_b.cel", MEMTYPE_CEL);
	
	PositionLoadedCel(cel_Top, 24, 0);
	PositionLoadedCel(cel_Left, 0, 0);
	PositionLoadedCel(cel_Right, 296, 0);
	PositionLoadedCel(cel_Bottom, 24, 220);
	
	// Initialize the game block CCBs
	
	y = 0;
	z = 0;
	
	for (x = 0; x < 50; x++) // 28x12
	{
		bricks[x].cel = CopyCel(cel_AllBricks[0]);
		bricks[x].X = 20 + (y * 28);
		bricks[x].Y = 48 + (z * 8);
		bricks[x].InitX = bricks[x].X;
		bricks[x].InitY = bricks[x].Y;
		bricks[x].HP = 3;
		bricks[x].Active = true;
		bricks[x].ExplodeFrame = 0;
		
		PositionCel(bricks[x].cel, bricks[x].X, bricks[x].Y);	
		
		if (++y > 9)
		{
			y = 0;
			z++;
		}
	}
	
	for (x = 1; x < 50; x++)
	{
		bricks[x - 1].cel->ccb_NextPtr = bricks[x].cel;
	}
	
	cel_LvlComplete = LoadCel("data/lvlcomplete.cel", MEMTYPE_CEL);
	
	PositionLoadedCel(cel_LvlComplete, 80, 60);
	
	SetFlag(cel_LvlComplete->ccb_Flags, CCB_SKIP);	
	
	stars[23].cel->ccb_NextPtr = planets[0].cel;	
	planets[6].cel->ccb_NextPtr = cel_Top;
	cel_Top->ccb_NextPtr = cel_Left;
	cel_Left->ccb_NextPtr = cel_Right;
	cel_Right->ccb_NextPtr = cel_Bottom;
	cel_Bottom->ccb_NextPtr = lasersL[0].cel;
	lasersR[7].cel->ccb_NextPtr = balls[0].cel;
	balls[2].cel->ccb_NextPtr = bricks[0].cel;
	bricks[49].cel->ccb_NextPtr = cel_Player1;
	cel_Player1->ccb_NextPtr = cel_Text1;
	cel_Text1->ccb_NextPtr = cel_Text2;
	cel_Text2->ccb_NextPtr = cel_LvlComplete;
	cel_LvlComplete->ccb_NextPtr = TrackedNumbers[0].cel_NumCels[0];
	
	srand(ReadHardwareRandomNumber()); 
}

void initSPORTwriteValue(unsigned value)
{
	WaitVBL(vsyncItem, 1); // Prevent screen garbage presumably
	
    memset(&ioInfo,0,sizeof(ioInfo));
    ioInfo.ioi_Command = FLASHWRITE_CMD;
    ioInfo.ioi_CmdOptions = 0xffffffff;
    ioInfo.ioi_Offset = value; // background colour
    ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
    ioInfo.ioi_Recv.iob_Len = SCREEN_SIZE_IN_BYTES;	
}

void initSPORTcopyImage(ubyte *srcImage)
{	
	memset(&ioInfo,0,sizeof(ioInfo));
	ioInfo.ioi_Command = SPORTCMD_COPY;
	//ioInfo.ioi_Offset = 0xf8f8f8f8; // Fun green and blue
	ioInfo.ioi_Offset = 0xffffffff; //  
	ioInfo.ioi_Send.iob_Buffer = srcImage;
	ioInfo.ioi_Send.iob_Len = SCREEN_SIZE_IN_BYTES;
	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
	ioInfo.ioi_Recv.iob_Len = SCREEN_SIZE_IN_BYTES;
	
	//WaitVBL(vsyncItem, 1); // Prevent screen garbage presumably
}

void initSPORTcopyImage2(ubyte *srcImage)
{	
	memset(&ioInfo,0,sizeof(ioInfo));
	ioInfo.ioi_Command = SPORTCMD_COPY;
	//ioInfo.ioi_Offset = 0xf8f8f8f8; // Fun green and blue
	ioInfo.ioi_Offset = 0x00ffff00; //  
	ioInfo.ioi_Send.iob_Buffer = srcImage;
	ioInfo.ioi_Send.iob_Len = SCREEN_SIZE_IN_BYTES;
	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
	ioInfo.ioi_Recv.iob_Len = SCREEN_SIZE_IN_BYTES;
	
	//WaitVBL(vsyncItem, 1); // Prevent screen garbage presumably
}

void initSPORT()
{
	VRAMIOReq = CreateVRAMIOReq(); // Obtain an IOReq for all SPORT operations
	
	SwapBackgroundImage("data/bgblack.img", -99);

	initSPORTcopyImage(backgroundBufferPtr1);
}

void SwapBackgroundImage(char *file, int imgIdx)
{	
	if (backgroundBufferPtr1 != NULL)
	{
		UnloadImage(backgroundBufferPtr1);
		backgroundBufferPtr1 = NULL;
	}

	backgroundBufferPtr1 = LoadImage(file, NULL, (VdlChunk **)NULL, &screen);
}

void initGraphics()
{
	int i;

	CreateBasicDisplay(&screen, DI_TYPE_DEFAULT, SCREEN_PAGES);

	for(i = 0; i < SCREEN_PAGES; i++)
	{
		bitmapItems[i] = screen.sc_BitmapItems[i];
		bitmaps[i] = screen.sc_Bitmaps[i];
	}

	EnableVAVG(screen.sc_Screens[0]);
	EnableHAVG(screen.sc_Screens[0]);
	EnableVAVG(screen.sc_Screens[1]);
	EnableHAVG(screen.sc_Screens[1]);

	vsyncItem = GetVBLIOReq();
}

void initSystem()
{
    OpenGraphicsFolio();
	OpenMathFolio();
	OpenAudioFolio();
}

void UpdateGameStats()
{
	int rem = AvailTime - CurrElapsedTime;
	
	if (rem < 0) rem = 0;
	
	if (rem < 10)
	{
		// Shake effect
	}
	
	SetCelNumbers(0, CurrLevel);
	SetCelNumbers(1, AvailLives);
	SetCelNumbers(2, AvailBalls);
	SetCelNumbers(3, AvailLasers);
	SetCelNumbers(4, rem);
	
	SetCelNumbers(5, TotalScore);
}

void DisplayStartScreen()
{
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Start1);

	DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);	

	visibleScreenPage = 1 - visibleScreenPage;

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
	
	DoIO(VRAMIOReq, &ioInfo);
}

/*
int32 lastSeconds = 0;
int32 lastDrawCels = 0;
int32 lastRoundTrip = 0;
int32 last30Time = 0;
int32 avgFrameMS = 0;
int32 frameCount = 0;
int32 msCount = 0;
int32 avgMS = 0;
int32 totElapsedMS = 0;

void DisplayGameplayScreen()
{
	debugMode = 2;
	frameCount++;
	if (++counter1 > 3) counter1 = 0;
	
	if (frameCount >= 30)
	{
		if (debugMode > 0)
		{
			TimeVal tv30Elapsed;
			
			SampleSystemTimeTV(&dData.TvFrames30End);
			
			SubTimes(&dData.TvFrames30Start, &dData.TvFrames30End, &tv30Elapsed);	
			
			last30Time = tv30Elapsed.tv_Microseconds / 1000; // 30 frames should be around 500 I think
			
			SampleSystemTimeTV(&dData.TvFrames30Start);
		}
		
		frameCount = 0;
	}
	
	if (debugMode > 0)
	{		
		TimeVal tvCurrLoopElapsed;
		
		SampleSystemTimeTV(&dData.TvCurrLoopEnd);
		
		SubTimes(&dData.TvCurrLoopStart, &dData.TvCurrLoopEnd, &tvCurrLoopElapsed);
		
		msCount++;
		totElapsedMS += tvCurrLoopElapsed.tv_Microseconds / 1000;
		avgMS = totElapsedMS / msCount;
		
		if (msCount > 120)
		{
			msCount = totElapsedMS = 0;
		}
		
		SampleSystemTimeTV(&dData.TvCurrLoopStart);
	}
	
	SampleSystemTimeTV(&dData.TvRenderStart);	
	

	
	if (debugMode > 0)
	{		
		SetCelNumbers(0, debugMode);
		SetCelNumbers(1, lastSeconds);
		SetCelNumbers(2, lastDrawCels);
		SetCelNumbers(3, lastRoundTrip);
		SetCelNumbers(4, last30Time);
		SetCelNumbers(5, avgMS);
	}

	
	SampleSystemTimeTV(&dData.TvDrawCelsStart);	
	
	if (debugMode == 2)
	{
		DrawScreenCels(screen.sc_Screens[visibleScreenPage], stars[0].cel); 
	}
	else
	{
		DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], stars[0].cel); 
	}
	
	//displayMem(screen.sc_BitmapItems[ visibleScreenPage ]);
	
	SampleSystemTimeTV(&dData.TvDrawCelsEnd);	
	
	if (debugMode > 0)
	{
		TimeVal tvElapsed, tvDrawCels, tvRoundTrip;
		
		SubTimes(&dData.TvInit, &dData.TvDrawCelsEnd, &tvElapsed);	
		SubTimes(&dData.TvDrawCelsStart, &dData.TvDrawCelsEnd, &tvDrawCels);
		SubTimes(&dData.TvRenderEnd, &dData.TvDrawCelsEnd, &tvRoundTrip);
		
		lastSeconds = tvElapsed.tv_Seconds;
		lastDrawCels = tvDrawCels.tv_Microseconds;
		lastRoundTrip = tvRoundTrip.tv_Microseconds;
	}	
	
    DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);
	
	visibleScreenPage = 1 - visibleScreenPage;

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
	
	DoIO(VRAMIOReq, &ioInfo);

	SampleSystemTimeTV(&dData.TvRenderEnd);	
}
*/

void DisplayGameplayScreen()
{
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], stars[0].cel); 
	DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);
	
	visibleScreenPage = 1 - visibleScreenPage;

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;
	
	DoIO(VRAMIOReq, &ioInfo);
}

void DisplayGameOverScreen(CCB *celBG, CCB *celFG)
{
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], celBG);	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], stars[0].cel);
	
	if (celFG != NULL)
	{
		DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], celFG);
	}
	
    DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);
	
	visibleScreenPage = (1 - visibleScreenPage);

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;	
	DoIO(VRAMIOReq, &ioInfo);	
}

void DisplayLevelUpScreen(CCB *celBG, CCB *celFG)
{
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], celBG);	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], stars[0].cel);
	
	if (celFG != NULL)
	{
		DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], celFG);
	}
	
    DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);
	
	visibleScreenPage = (1 - visibleScreenPage);

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;	
	DoIO(VRAMIOReq, &ioInfo);	
}

void AnimateNextLevel()
{
	int x = 0;
	
	AnimateBalls();
	AnimateStars();
	AnimatePlanets();
	
	for (x = 0; x < 90; x++)
	{		
		AnimateStars();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	ClearFlag(cel_Player1->ccb_Flags, CCB_SKIP); 
	
	cel_Player1->ccb_HDX = Convert32_F16(8) << 4;
	cel_Player1->ccb_VDY = Convert32_F16(8);
	
	P1_Y = -60;
	P1_X = (320 / 2 - 26) - 220;
	
	cel_Player1->ccb_YPos = Convert32_F16(P1_Y);
	cel_Player1->ccb_XPos = Convert32_F16(P1_X);
		
	while (P1_Y < 60)
	{		
		P1_Y += 4;
		
		cel_Player1->ccb_YPos = Convert32_F16(P1_Y);

		AnimateStars();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	for (x = 70; x > 0; x--)
	{
		P1_X += 3;	
		P1_Y += 2;

		cel_Player1->ccb_HDX = DivSF16(Convert32_F16(x + 10), Convert32_F16(10)) << 4;
		cel_Player1->ccb_VDY = DivSF16(Convert32_F16(x + 10), Convert32_F16(10));
		
		cel_Player1->ccb_XPos = Convert32_F16(P1_X);
		cel_Player1->ccb_YPos = Convert32_F16(P1_Y);
		
		AnimateStars();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	cel_Player1->ccb_HDX = Convert32_F16(1) << 4;
	cel_Player1->ccb_VDY = Convert32_F16(1);
	
	for (x = 0; x < 60; x++)
	{
		AnimatePlayer1();
		AnimateStars();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	SampleSystemTimeTV(&dData.TvInit);
}

void AnimateLevelComplete()
{
	int x;
	
	int timeBonus = AvailTime - CurrElapsedTime;
	
	lastElapsedTime = 0;
	CurrElapsedTime = 0;
	
	if (timeBonus < 0) timeBonus = 0;
	
	timeBonus = timeBonus * 5;
	
	for (x = 0; x < 3; x++)
	{
		if (balls[x].Active == true && AvailBalls < 24)
		{
			AvailBalls++;
		}
		
		balls[x].Active = false;
		
		SetFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 8; x++)
	{
		lasersL[x].Active = false;
		lasersR[x].Active = false;
		
		SetFlag(lasersL[x].cel->ccb_Flags, CCB_SKIP);
		SetFlag(lasersR[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 30; x++) // Finish up any pending animations
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
	
	P1_Speed = 0;
	
	while (P1_Y != 200)
	{		
		if (P1_Y < 200) P1_Y++;
		if (P1_Y > 200) P1_Y--;
		
		AnimatePlayer1();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	while (P1_X != 130)
	{		
		if (P1_X < 130) P1_X++;
		if (P1_X > 130) P1_X--;
		
		AnimatePlayer1();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	for (x = 0; x < 30; x++)
	{		
		AnimatePlayer1();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	while (--P1_Y >= 21)
	{		
		if (P1_Y % 2 == 0 && P1_X < 220) P1_X++;
		
		if (P1_Y > 100 && P1_X < 250) P1_X++;
		
		cel_Player1->ccb_HDX = DivSF16(Convert32_F16(P1_Y), Convert32_F16(200)) << 4;
		cel_Player1->ccb_VDY = DivSF16(Convert32_F16(P1_Y), Convert32_F16(200));
		
		AnimatePlayer1();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	SetFlag(cel_Player1->ccb_Flags, CCB_SKIP); 
	
	// TODO SHOW SCORES - Timer and Balls Only bonus maybe
	
	ClearFlag(cel_LvlComplete->ccb_Flags, CCB_SKIP);
	
	InitNumberCel(6, 122, 63, CurrLevel, false);
	InitNumberCel(7, 176, 111, timeBonus, false);
	
	for (x = 0; x < 90; x++)
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
	
	while (timeBonus >= 0)
	{
		SetCelNumbers(7, timeBonus);
		
		TotalScore += 5;
		timeBonus -= 5;
		
		HandleAnimation();
			
		DisplayGameplayScreen();
	}
	
	for (x = 0; x < 30; x++)
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
	
	InitNumberCel(6, -200, -200, 0, false);
	InitNumberCel(7, -200, -200, 0, false);
	
	SetFlag(cel_LvlComplete->ccb_Flags, CCB_SKIP);
	
	for (x = 0; x < 30; x++)
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
}

void AnimateLostLife()
{
	int i, x;
	
	CCB *celRed[10];
	CCB *celWhite[10];
	
	for (x = 0; x < 10; x++)
	{
		celRed[x] = CreateBackdropCel(320, 240, MakeRGB15(31, 0, 0), (x + 1) * 10);
		celWhite[x] = CreateBackdropCel(320, 240, MakeRGB15(31, 31, 31), (x + 1) * 10);
	}
	
	for (x = 0; x < 10; x++)
	{
		for (i = 0; i < 6; i++)
		{
			DisplayGameOverScreen(celRed[x], NULL);
		}
	}
	
	for (x = 0; x < 10; x++)
	{
		for (i = 0; i < 6; i++)
		{
			DisplayGameOverScreen(celRed[9], celWhite[x]);
		}
	}
	
	for (x = 0; x < 60; x++)
	{
		DisplayGameOverScreen(celRed[9], celWhite[9]);
	}
	
	FadeToBlack( &screen, 30 );
	
	for (x = 0; x < 10; x++)
	{
		UnloadCel(celRed[x]);
		UnloadCel(celWhite[x]);
	}
	
	SetFlag(cel_Player1->ccb_Flags, CCB_SKIP);
	
	FadeFromBlack( &screen, 30 );
}

void AnimateBricks()
{
	if (++CounterBricks < 45) return;
	
	CounterBricks = 0;
	
	{
		int x;
		
		for (x = 0; x < 50; x++)
		{
			if (bricks[x].Active == false) continue;
			if (bricks[x].Animate == false) continue;
			if (bricks[x].HP < 1) continue;
			if (bricks[x].AnimMinIdx < 0) continue;
			if (bricks[x].AnimMaxIdx < 0) continue;
			
			if (--bricks[x].ImageIdx < bricks[x].AnimMinIdx) bricks[x].ImageIdx = bricks[x].AnimMaxIdx;
			
			bricks[x].cel->ccb_SourcePtr = cel_AllBricks[bricks[x].ImageIdx]->ccb_SourcePtr;
		}
	}
}

void AnimateBalls()
{
	int i, j;
	
	int currX1, currX2, currY1, currY2;
	
	int newX1, newX2, newY1, newY2;
	
	int collisionX, collisionY;
	
	for (j = 0; j < 3; j++)
	{
		if (balls[j].Active == false) continue;
		
		currX1 = balls[j].X;
		currX2 = balls[j].X + 7;
		
		currY1 = balls[j].Y;
		currY2 = balls[j].Y + 7;
		
		newX1 = balls[j].X + balls[j].SpeedX;
		newX2 = newX1 + 7;
		
		newY1 = balls[j].Y + balls[j].SpeedY;
		newY2 = newY1 + 7;
		
		collisionX = 0;
		collisionY = 0;
		
		if (balls[j].SpeedX < 0 && newX1 < 20) collisionX++;
		if (balls[j].SpeedX > 0 && newX2 > 300) collisionX++;
		if (balls[j].SpeedY < 0 && newY1 < 20) collisionY++;
		
		if (balls[j].SpeedY > 0 && newY2 > 240)
		{			
			balls[j].Active = false;
			
			SetFlag(balls[j].cel->ccb_Flags, CCB_SKIP);
			
			return;
		}
	
		if (balls[j].SpeedY > 0)
		{
			if (newX2 >= P1_X && newX1 <= P1_X + P1_WIDTH - 1)
			{
				if (newY1 < P1_Y && newY2 >= P1_Y)
				{
					PlaySFX(SFX_BONP1);
					
					if (balls[j].Damage == 0)
					{
						balls[j].Damage = 10;
						balls[j].cel->ccb_SourcePtr = cel_AllBalls[1]->ccb_SourcePtr;
					}
					
					collisionY++;
					
					if (P1_Y < 200)
					{
						balls[j].SpeedX = ( ((currX1 + 2) - (P1_X + (P1_WIDTH - 1) / 2)) / 7 ); // Range of -3 to +3
					}
					else
					{
						balls[j].SpeedX += ( ((currX1 + 2) - (P1_X + (P1_WIDTH - 1) / 2)) / 7 ); // Range of -3 to +3
					}
					
					if (balls[j].SpeedX < -3) balls[j].SpeedX = -3;
					if (balls[j].SpeedX > 3) balls[j].SpeedX = 3;
					
					if (balls[j].SpeedY > 0) balls[j].SpeedY = 4 - abs(balls[j].SpeedX);
					if (balls[j].SpeedY < 0) balls[j].SpeedY = -4 + abs(balls[j].SpeedX);
				}
			}
		}

		for (i = 49; i >= 0; i--)
		{
			if (bricks[i].HP < 1) continue;		

			if (newX2 >= bricks[i].X && newX1 <= bricks[i].X + BRICK_WIDTH - 1) // Within Width
			{
				if (balls[j].SpeedY > 0) // Moving down
				{			
					if (currY2 < bricks[i].Y && newY2 >= bricks[i].Y)
					{
						collisionY++;
						
						HandleBrickCollision(i, balls[j].Damage);
					}
				}
				else
				{
					if (currY1 > bricks[i].Y + BRICK_HEIGHT - 1 && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1)
					{
						collisionY++;
						
						HandleBrickCollision(i, balls[j].Damage);
					}
				}
			}
			
			//if (collisionY == 2) break;
			
			if (newY2 >= bricks[i].Y && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1) // Within Width
			{
				if (balls[j].SpeedX > 0) // Moving down
				{			
					if (currX2 < bricks[i].X && newX2 >= bricks[i].X)
					{
						collisionX++;
						
						HandleBrickCollision(i, balls[j].Damage);
					}
				}
				else
				{
					if (currX1 > bricks[i].X + BRICK_WIDTH - 1 && newX1 <= bricks[i].X + BRICK_WIDTH - 1)
					{
						collisionX++;
						
						HandleBrickCollision(i, balls[j].Damage);
					}
				}
			}
			
			//if (collisionX == 2) break;
		}
	
		if (collisionX >= 2 || collisionY >= 2)
		{
			if (collisionX >= 2)
			{
				balls[j].SpeedX *= -1;
			}
			else
			{
				balls[j].SpeedY *= -1;
			}
		}
		else
		{
			if (collisionX >= 1) balls[j].SpeedX *= -1;
			if (collisionY >= 1) balls[j].SpeedY *= -1;
		}
		
		balls[j].X += balls[j].SpeedX;
		balls[j].Y += balls[j].SpeedY;
		
		PositionCel(balls[j].cel, balls[j].X, balls[j].Y);
	}
}

void AnimatePlanets()
{
	int x;
	
	for (x = 0; x < 7; x++)
	{
		if (planets[x].Active == true)
		{
			if (++planets[x].FramesX >= planets[x].DelayX)
			{
				planets[x].FramesX = 0;
				
				planets[x].X--;
			}
			
			if (++planets[x].FramesY > planets[x].DelayY)
			{
				planets[x].FramesY = 0;
				
				planets[x].Y--;
			}
			
			PositionCel(planets[x].cel, planets[x].X, planets[x].Y);
		}
	}
}

void AnimateStars()
{
	int x;
	
	for (x = 0; x < 24; x++)
	{
		stars[x].X = stars[x].X - stars[x].Speed;
		
		if (stars[x].X < -1)
		{
			stars[x].X = (rand() % 48) + 320;
		}
		
		PositionCel(stars[x].cel, stars[x].X, stars[x].Y);
	}
}

void AnimatePlayer1()
{
	if (CurrLevelComplete == false)
	{
		P1_X += P1_Speed;
		
		if (P1_X < 20)
		{
			P1_X = 20;
			P1_Speed = -1 * (P1_Speed / 4);
		}
		else if (P1_X > 240)
		{
			P1_X = 240;
			P1_Speed = -1 * (P1_Speed / 4);
		}
		
		if (P1_Y >= 120 && kpUp)
		{
			P1_Y--;
		}
		
		if (kpUp == false)
		{
			if (P1_Y < 200)
			{
				P1_Y++;
			}
		
			CounterHover++;
			
			if (CounterHover == 20) P1_Y--;
			if (CounterHover == 40) P1_Y++;
			if (CounterHover == 60) P1_Y++;
			if (CounterHover == 80) P1_Y--;
			
			if (CounterHover > 80) CounterHover = 0;
			
		}
	}
	
	PositionCel(cel_Player1, P1_X, P1_Y);
}

void AnimateLasers()
{
	int x;
	
	for (x = 0; x < 8; x++)
	{
		if (lasersL[x].Active == true)
		{
			if (CheckCollisionLasers(lasersL[x].X, lasersL[x].Y - 4))
			{
				lasersL[x].Active = false;
				
				SetFlag(lasersL[x].cel->ccb_Flags, CCB_SKIP);
			}
			else
			{
				lasersL[x].Y -= 4;
			}
		}
		
		if (lasersR[x].Active == true)
		{
			if (CheckCollisionLasers(lasersR[x].X, lasersR[x].Y - 4))
			{
				lasersR[x].Active = false;
				
				SetFlag(lasersR[x].cel->ccb_Flags, CCB_SKIP);
			}
			else
			{
				lasersR[x].Y -= 4;
			}
		}
		
		PositionCel(lasersL[x].cel, lasersL[x].X, lasersL[x].Y);
		
		if (lasersL[x].Active == true)
		{
			ClearFlag(lasersL[x].cel->ccb_Flags, CCB_SKIP);
		}
		else
		{
			SetFlag(lasersL[x].cel->ccb_Flags, CCB_SKIP);
		}

		PositionCel(lasersR[x].cel, lasersR[x].X, lasersR[x].Y);
		
		if (lasersR[x].Active == true)
		{
			ClearFlag(lasersR[x].cel->ccb_Flags, CCB_SKIP); 
		}
		else
		{
			SetFlag(lasersR[x].cel->ccb_Flags, CCB_SKIP);
		}		
	}
}

void AnimateBrickExplosions()
{
	int x;
	
	for (x = 0; x < 50; x++)
	{
		if (bricks[x].Active == false) continue;
		if (bricks[x].HP >= 1) continue;
		
		// Bypassing the MARIA effect for now
		
		bricks[x].Active = false; 
		bricks[x].ExplodeFrame = 0;
			
		SetFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);
			
		ClearFlag(bricks[x].cel->ccb_Flags, CCB_MARIA);
		
		CurrScore++;
		
		continue;
		
		bricks[x].ExplodeFrame++;
		
		if (bricks[x].ExplodeFrame <= 10)
		{
			bricks[x].X -= 12;
			bricks[x].Y -= 6;
			
			bricks[x].cel->ccb_XPos = Convert32_F16(bricks[x].X);
			bricks[x].cel->ccb_YPos = Convert32_F16(bricks[x].Y);
			bricks[x].cel->ccb_HDX = Convert32_F16(BRICK_WIDTH + (bricks[x].ExplodeFrame * 12));
			bricks[x].cel->ccb_VDY = DivSF16(Convert32_F16(BRICK_HEIGHT + (bricks[x].ExplodeFrame * 12)), Convert32_F16(BRICK_HEIGHT));
		}
		
		if (bricks[x].ExplodeFrame >= 15)
		{
			bricks[x].Active = false;
			bricks[x].ExplodeFrame = 0;
			
			SetFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);
			
			ClearFlag(bricks[x].cel->ccb_Flags, CCB_MARIA);
			
			bricks[x].cel->ccb_XPos = Convert32_F16(bricks[x].InitX);
			bricks[x].cel->ccb_YPos = Convert32_F16(bricks[x].InitY);
			
			bricks[x].cel->ccb_HDX  = Convert32_F16(1) << 4;
			bricks[x].cel->ccb_VDY  = Convert32_F16(1);
			
			
		
			CurrScore++;
			//bricks[x].cel->ccb_HDX  = DivSF16(Convert32_F16(BRICK_WIDTH), Convert32_F16(bricks[x].cel->ccb_Width)) << 4;
			//bricks[x].cel->ccb_VDY  = DivSF16(Convert32_F16(BRICK_HEIGHT), Convert32_F16(bricks[x].cel->ccb_Height));
		}
	}
}

void ShootLaserLeft()
{
	if (AvailLasers == 0) return;
	
	PlaySFX(SFX_PEW);
	
	AvailLasers--;
	
	if (++LastLaserL > 7) LastLaserL = 0;
	
	lasersL[LastLaserL].Active = true;
	
	lasersL[LastLaserL].X = P1_X + 1;
	lasersL[LastLaserL].Y = P1_Y + LASER_OFFSET_Y - 4;
	
	ClearFlag(lasersL[LastLaserL].cel->ccb_Flags, CCB_SKIP);
}

void ShootLaserRight()
{
	if (AvailLasers == 0) return;
	
	PlaySFX(SFX_PEW);
	
	AvailLasers--;
	
	if (++LastLaserR > 7) LastLaserR = 0;
	
	lasersR[LastLaserR].Active = true;
	
	lasersR[LastLaserR].X = P1_X + (P1_WIDTH - 2);
	lasersR[LastLaserR].Y = P1_Y + LASER_OFFSET_Y - 4;
	
	ClearFlag(lasersR[LastLaserR].cel->ccb_Flags, CCB_SKIP);
}

void ShootBall(int speedX, int speedY) // Only 3 allowed in play at a time
{	
	if (AvailBalls == 0) return;	
	
	{
		int x;
		int ab = 0;
		
		TimeVal tvNow, tvDiff;
	
		SampleSystemTimeTV(&tvNow);
		
		SubTimes(&tvLastBall, &tvNow, &tvDiff);
		
		if (tvDiff.tv_Seconds < 1) return;
		
		PlaySFX(SFX_SHOOT);
		
		for (x = 0; x < 3; x++)
		{		
			if (balls[x].Active == true) ab++;
		}
		
		if (ab == 3) return; // Only 3 active allowed at a time
		
		for (x = 0; x < 3; x++)
		{		
			if (balls[x].Active == true) continue;
			
			balls[x].Damage = 10;
			balls[x].cel->ccb_SourcePtr = cel_AllBalls[1]->ccb_SourcePtr;
			balls[x].Active = true;
			
			balls[x].X = P1_X + (P1_WIDTH / 2) - 4;
			balls[x].Y = P1_Y + 4;
			
			balls[x].SpeedX = speedX;
			balls[x].SpeedY = speedY;
			
			ClearFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
			
			AvailBalls--;
			
			break;
		}
		
		SampleSystemTimeTV(&tvLastBall);
	}
}

bool CheckCollisionLasers(int x, int y)
{
	int i;
	
	for (i = 49; i >= 0; i--)
	{
		if (bricks[i].HP < 1) continue;
		if (x < bricks[i].X || x > bricks[i].X + BRICK_WIDTH - 1) continue;
		if (y > (bricks[i].Y + BRICK_HEIGHT - 1)) continue;
		
		HandleBrickCollision(i, 1);
		
		return true;		
	}
	
	return false;
}

void HandleBrickCollision(int idx, int damage)
{
	bricks[idx].HP -= damage;
	
	if (bricks[idx].HP < 1)
	{		
		PlaySFX(SFX_BRKBRK);
		
		TotalScore += 25;
		
		SetFlag(bricks[idx].cel->ccb_Flags, CCB_MARIA);
		
		if (bricks[idx].IsBomb) // TODO
		{
						
		}
	}
	else
	{
		PlaySFX(SFX_BOUNCE);
	}
	
	// else if (bricks[idx].Animate == false)// TODO Type of image mode
	// {
		// TotalScore += 5;
		
		// bricks[idx].cel->ccb_SourcePtr = cel_AllBricks[bricks[idx].HP - 1]->ccb_SourcePtr; // TODO CONFIRM THIS
	// }	
}

/*
	Handle input includes additional checks for debouncing and intentionally artificially
	introducing input delays. Otherwise button presses register far too quickly
	
	Any of the "kp" variables keep track of a button being pressed, and held
	Any of the "sw" variables keep track of the length of time in screen refreshes a button has been pressed
	
	Much of this may be overkill, but it is leftover from my Tetris app	
*/

static ControlPadEventData cped;

void HandleInput()
{
	uint32 joyBits;
	
	GetControlPad(1, 0, &cped); //  

	joyBits = cped.cped_ButtonBits;
	
	if (ShowingIntroAnimation == true)
	{
		if ((joyBits & ControlStart) || (joyBits & ControlA) || (joyBits && ControlB) || (joyBits && ControlC))
		{
			SkipIntro = true;
		}
		
		return;
	}		
	
	if (GameStarted == false)
	{
		if ((joyBits & ControlStart) || (joyBits & ControlA) || (joyBits && ControlB) || (joyBits && ControlC))
		{
			GameStarted = true;
		}
		
		return;
	}
	
	if (joyBits & ControlX) 
	{
		if (kpStop == false)
		{			
			GamePaused = true;
		}

		kpStop = true;
	}
	else
	{
		kpStop = false;
	}

	if (joyBits & ControlStart)
	{
		if (kpStart == false)
		{
			GamePaused = false;
		}

		kpStart = true;
	}
	else
	{
		kpStart = false;
	}
	
	if (GamePaused == true) return;

	if (joyBits & ControlLeftShift) 
	{
		if (kpLS == false)
		{
			ShootLaserLeft();
		}

		kpLS = true;
		swLS++;
	}
	else
	{
		swLS = 0;
		kpLS = false;
	}

	if (joyBits & ControlRightShift) 
	{
		if (kpRS == false)
		{
			ShootLaserRight();
		}

		kpRS = true;
		swRS++;

		if (swRS >= 60) swRS = 0;
	}
	else
	{
		swRS = 0;
		kpRS = false;
	}

	if (joyBits & ControlA)
	{
		if (kpA == false)
		{
			ShootBall(-2, -2);
		}

		kpA = true;

		if (swA >= 15) swA = 0;
	}
	else
	{
		kpA = false;
		swA = 0;
	}

	if (joyBits & ControlB)
	{
		if (kpB == false)
		{
			ShootBall(0, -4);
		}

		kpB = true;
	}
	else
	{
		kpB = false;
		swB = 0;
	}

	if (joyBits & ControlC)
	{
		if (kpC == false)
		{
			ShootBall(2, -2);
		}

		kpC = true;

		if (swC >= 15) swC = 0;
	}
	else
	{
		kpC = false;
		swC = 0;
	}

	if (joyBits & ControlUp)
	{
		if (kpUp == false || ++swUp >= 3) 
		{
			
		}

		kpUp = true;

		if (swUp >= 15) swUp = 0;
	}
	else
	{
		swUp = 0;
		kpUp = false;
	}

	if (joyBits & ControlDown)
	{
		if (kpDown == false || ++swDown >= 3) 
		{
			MoveDown();
		}

		kpDown = true;

		if (swDown >= 3) swDown = 0;
	}
	else
	{
		swDown = 0;
		kpDown = false;
	}

	if (joyBits & ControlLeft)
	{
		if (kpLeft == false || swLeft == 2 || swLeft % 4 == 0) // Slightly stagger the increase speed
		{
			MoveLeft();
		}

		kpLeft = true;
		swLeft++;
	}
	else
	{
		swLeft = 0;
		kpLeft = false;
	}

	if (joyBits & ControlRight)
	{
		if (kpRight == false || swRight == 2 || swRight % 4 == 0) // Slightly stagger the increase speed
		{
			MoveRight();
		}

		kpRight = true;
		swRight++;
	}
	else
	{
		swRight = 0;
		kpRight = false;
	}
	
	if (kpLeft == false && kpRight == false) Decelerate();
}

void MoveLeft()
{	
	if (P1_Speed > -8) P1_Speed--;	
}

void MoveRight()
{	
	if (P1_Speed < 8) P1_Speed++;
}

void MoveDown()
{	
	if (P1_Speed > 0) P1_Speed--;
	if (P1_Speed < 0) P1_Speed++;
}

void Decelerate()
{
	if (Counter1 % 4 == 0) MoveDown();
}

void Move()
{

}

void PlaySFX(int id)
{
	playsound(id);
}

void PlayBackgroundMusic() 
{
	spoolsound("music/TBM2.aiff", 256);
}

void ShowIntroSplash()
{
	int x;
	
	CCB *cel_Story1 = LoadCel("data/story1.cel", MEMTYPE_CEL);
	CCB *cel_Story2 = LoadCel("data/story2.cel", MEMTYPE_CEL);
	CCB *cel_Story3 = LoadCel("data/story3.cel", MEMTYPE_CEL);
	CCB *cel_Story4 = LoadCel("data/story4.cel", MEMTYPE_CEL);
	
	InitCCBFlags(cel_Story1);
	InitCCBFlags(cel_Story2);
	InitCCBFlags(cel_Story3);
	InitCCBFlags(cel_Story4);
	
	SetFlag(cel_Story1->ccb_Flags, CCB_LAST);
	SetFlag(cel_Story2->ccb_Flags, CCB_LAST);
	SetFlag(cel_Story3->ccb_Flags, CCB_LAST);
	SetFlag(cel_Story4->ccb_Flags, CCB_LAST);
	
	ShowingIntroAnimation = true;
	
	SwapBackgroundImage("data/bgblack.img", -99);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	FadeToBlack( &screen, 30 );
	
	SwapBackgroundImage("data/hdsplash.img", -98);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	FadeFromBlack( &screen, 60 );	
	
	for (x = 0; x < 180; x++) // Display 3DO HD logo for 3 seconds
	{
		WaitVBL(vsyncItem, 1);
	}
	
	FadeToBlack( &screen, 60 );
	
	//
	
	SwapBackgroundImage("data/bg_2.img", -97);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Story1);
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	InitEventUtility(1, 0, LC_Observer);
	
	FadeFromBlack( &screen, 30 );
	
	SkipIntro = false;	
	
	for (x = 0; x < 60 * 10; x++) 
	{
		HandleInput();
		
		WaitVBL(vsyncItem, 1);
		
		if (SkipIntro == true) break;
	}
	
	KillEventUtility();
	
	FadeToBlack( &screen, 30 );
	
	
	//
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Story2);
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	InitEventUtility(1, 0, LC_Observer);
	
	FadeFromBlack( &screen, 30 );	
	
	SkipIntro = false;	
	
	for (x = 0; x < 60 * 10; x++) 
	{
		HandleInput();
		
		WaitVBL(vsyncItem, 1);
		
		if (SkipIntro == true) break;
	}
	
	KillEventUtility();
	
	FadeToBlack( &screen, 30 );
	
	//
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Story3);
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	InitEventUtility(1, 0, LC_Observer);
	
	FadeFromBlack( &screen, 30 );
	
	SkipIntro = false;	
	
	for (x = 0; x < 60 * 10; x++) 
	{
		HandleInput();
		
		WaitVBL(vsyncItem, 1);
		
		if (SkipIntro == true) break;
	}
	
	KillEventUtility();
	
	FadeToBlack( &screen, 30 );
	
	//
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Story4);
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	InitEventUtility(1, 0, LC_Observer);
	
	FadeFromBlack( &screen, 30 );
	
	SkipIntro = false;	
	
	for (x = 0; x < 60 * 10; x++) 
	{
		HandleInput();
		
		WaitVBL(vsyncItem, 1);
		
		if (SkipIntro == true) break;
	}
	
	KillEventUtility();
	
	FadeToBlack( &screen, 30 );
	
	//

	SwapBackgroundImage("data/bgmain.img", 0);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	FadeFromBlack( &screen, 60 );
	
	UnloadCel(cel_Story1);
	UnloadCel(cel_Story2);
	UnloadCel(cel_Story3);
	UnloadCel(cel_Story4);
	
	ShowingIntroAnimation = false;
}

void ShowGameOver()
{
	int x;
	
	FadeToBlack( &screen, 15 );
	
	SwapBackgroundImage("data/gameover.img", -100);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	FadeFromBlack( &screen, 60 );	
	
	for (x = 0; x < 60 * 5; x++) // Display game over for 5 seconds
	{
		WaitVBL(vsyncItem, 1);
	}
}

void ShowVictory()
{
	int x;
	int celY = 240;
	
	CCB *cel_Victory = LoadCel("data/victory.cel", MEMTYPE_CEL);
	
	InitCCBFlags(cel_Victory);
	
	SetFlag(cel_Victory->ccb_Flags, CCB_LAST);
	
	FadeToBlack( &screen, 15 );
	
	SwapBackgroundImage("data/victorybg.img", -101);
	
	DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
	
	DisplayScreen( screen.sc_Screens[ visibleScreenPage ], 0);
	
	FadeFromBlack( &screen, 60 );	
	
	for (x = 0; x < 60; x++) // Display game over for 5 seconds
	{
		WaitVBL(vsyncItem, 1);
	}
	
	while (--celY >= -520)
	{
		for (x = 0; x < 2; x++)
		{
			PositionCel(cel_Victory, 18, celY);
			
			DrawImage( screen.sc_Screens[ visibleScreenPage ], backgroundBufferPtr1, &screen );
			
			DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_Victory);

			DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);	

			visibleScreenPage = 1 - visibleScreenPage;
			
			WaitVBL(vsyncItem, 1);
		}
	}
	
	FadeToBlack( &screen, 60 );
	
	for (x = 0; x < 60; x++) // Display game over for 5 seconds
	{
		WaitVBL(vsyncItem, 1);
	}
	
	SetFlag(cel_Victory->ccb_Flags, CCB_SKIP);
	
	FadeFromBlack( &screen, 60 );	
	
	UnloadCel(cel_Victory);
}



/* TODO

FIRE WAY TOO FAST
BRICKS - Brick Stack?
Smaller ball

Gaps between the bricks

Make more fun

Sound fx

music

Start screen

Pause Screen


*/
