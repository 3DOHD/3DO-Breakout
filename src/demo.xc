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
//	CEL Grid Rendering Demo - Good entry point for familiarizing yourself with
//	some of the 3DO graphics basics
//

*/

#include "demo.h" 
#include "celutils.h" 
#include "HD3DO.h"
#include "tools.h"


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
void Move();
void UpdateGameStats();
void DisplayGameplayScreen();
void DisplayGameOverScreen(CCB *celBG, CCB *celFG);
void AnimateGameOver();
void AnimateNextLevel();
void AnimatePlayer1();
void AnimateFire();
void AnimateStars();
void AnimatePlanets();
void AnimateComet();
void AnimateLasers();
void AnimateBalls();
void AnimateBomb();
void AnimateBrickExplosions();
void AnimateLevelComplete();
void AnimateBackground();
void HandleAnimation();
void HandleBrickCollision(int idx, int damage);
void TrackPowerUps();
void ShootLaserLeft();
void ShootLaserRight();
void ShootBall(int speedX, int speedY);
bool CheckCollisionLasers(int x, int y);
void ApplySelectedColorPalette();
void SwapBackgroundImage(char *file, int imgIdx);

void InitCCBFlags(CCB *cel); // Lives in HD3DO.c

/* ----- GAME VARIABLES -----*/

static ScreenContext screen;

static Item bitmapItems[SCREEN_PAGES];
static Bitmap *bitmaps[SCREEN_PAGES];

static Item VRAMIOReq;
static Item vsyncItem;
static IOInfo ioInfo;

static DebugData dData;
static int frameCount = 0;
bool ShowDebugStats = false;

static int visibleScreenPage = 0; 


static int LVDATA[4][50] =
{
	{  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,	11, 12, 13, 12, 11, 10,  9,  8,  7,  6,    5,  4,  3,  2,  1,  2,  3,  4,  5,  6,    7,  8,  9, 10, 11, 12, 13, 12, 11, 10,    9,  8,  7,  6,  5,  4,  3,  2,  1,  2 },
	{  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,	13, 13, 13, 13, 13, 13, 13, 13, 13, 13,    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,   10, 10, 10, 10, 10, 10, 10, 10, 10, 10,    2,  2,  2,  2,  2,  2,  2,  2,  2,  2 },
	{  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    0,  1,  1,  1,  1,  1,  1,  1,  1,  1,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    1,  1,  1,  1,  1,  1,  1,  1,  1,  1 },
	{ 14,  3,  3,  3,  3,  3,  3,  3,  3, 14,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,    5,  5,  5,  5,  0,  0,  5,  5,  5,  5,    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,   14,  1,  1,  1,  1,  1,  1,  1,  1, 14 }
};

/*
	0 - Ball 1
	1 - Ball 2
	2 - Ball 3
	3 - Bomb
	4 - BG Planet 1
	5 - BG Planet 2
	6 - Timer
*/
static int LVMETA[4][7] = 
{
	{ 1, 0, 0, 0, 5,  6, 300 },
	{ 0, 1, 0, 0, 2, -1, 240 },
	{ 1, 1, 1, 0, 4, -1, 120 },
	{ 1, 1, 1, 1, 5,  3,  90 },
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

static int CurrScore = 0;
static int CurrTarget = 0;
static int Counter1 = 0;
static int CounterFire = 0;

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
const int BRICK_HEIGHT = 12;

//

static int P1_X = 320 / 2 - 26;
static int P1_Y = 260;
static int HoverCounter = 0;

static int P1_Speed = 0;

static bool GameOver = false;
static int CurrLevel = 1;

static int BG_X = 0;
static int BG_Y = 0;

//

// TODO LOAD ALL PLANETS

static CCB *cel_AllBricks[40]; // Load all potential game block data
static CCB *cel_AllBalls[1];
static CCB *cel_AllBombs[4];
static CCB *cel_FireL[7];
static CCB *cel_FireR[7];

static int FireYL[7];
static int FireYR[7];

CCB *cel_Top;
CCB *cel_Left;
CCB *cel_Right;
CCB *cel_Bottom;

CCB *cel_Player1;

CCB *cel_Text1;

CCB *cel_BG;

CCB *cel_BG1;
CCB *cel_BG2;

//Ball balls[4];
Bomb bomb;
Comet comet;
Planet planets[7];

Brick bricks[50]; 
Star stars[100]; // Background stars

Laser lasersL[8];
Laser lasersR[8];

Ball balls[3];

static int LastLaserL = -1;
static int LastLaserR = -1;

int getFrameNum = 0;

TimeVal tvLastBall;

static ubyte *backgroundBufferPtr1 = NULL; // Pointer to IMAG file passed to the SPORT buffer for the background image

int main() // Main entry point
{
	initSystem(); // Required system inits
	initGraphics(); // Required graphic inits

	InitNumberCels(8); // 3DOHD Initialize 6 sets of number CCBs for chaining
	
	initTools();

	InitNumberCel(0, 40, 11, 0, false);
	InitNumberCel(1, 98, 11, 0, false);
	InitNumberCel(2, 155, 11, 0, false);
	InitNumberCel(3, 216, 11, 0, false);	
	InitNumberCel(4, 230, 11, 0, true);
	
	InitNumberCel(5, 64, 220, 0, false); // Bottom Score
	
	InitNumberCel(6, -200, -200, false);
	InitNumberCel(7, -200, -200, false);
	
	loadData(); // Load and initialize CELs

	initSPORT(); // Turn the background black
		
	GameLoop(); // The actual game play happens here - Nothing above gets called again
}

void InitGame()
{
	int x;
	
	ResetCelNumbers();
	
	FadeFromBlack( &screen, 1 );
	
	CurrScore = 0;
	
	comet.X = 340;
	comet.Y = 50;
	
	GameOver = false;
	
	SampleSystemTimeTV(&dData.TvInit);				// Used for timing and performance benchmarking
	SampleSystemTimeTV(&dData.TvFrames30Start);		// Used for timing and performance benchmarking
	SampleSystemTimeTV(&tvLastBall);
}

void InitNewLevel(int level)
{
	int x, p1, p2, bType;
	int lvB1, lvB2, lvB3, lvBomb, lvP1, lvP2, lvTime;
	
	char str[14];
	
	P1_X = 320 / 2 - 26;
	P1_Y = 260;

	P1_Speed = 0;
	
	CurrScore = 0;
	CurrTarget = 0;
	
	BG_X = 0;
	BG_Y = 0;

	sprintf(str, "data/bg_%d.cel", level);

	UnloadCel(cel_BG);
	
	cel_BG = LoadCel(str, MEMTYPE_CEL); // Wrap around BG 320px wide
	
	cel_BG1->ccb_SourcePtr = cel_BG->ccb_SourcePtr;
	cel_BG2->ccb_SourcePtr = cel_BG->ccb_SourcePtr;
	
	lvB1 = LVMETA[level - 1][0];
	lvB2 = LVMETA[level - 1][1];
	lvB3 = LVMETA[level - 1][2];
	
	lvBomb = LVMETA[level - 1][3];
	
	lvP1 = LVMETA[level - 1][4];
	lvP2 = LVMETA[level - 1][5];
	
	AvailTime = LVMETA[level - 1][6];
	
	for (x = 0; x < 3; x++)
	{
		balls[x].Active = false;
		
		SetFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB1 == 1)
	{
		balls[0].Active = true;
		
		balls[0].X = 24;
		balls[0].Y = 24;
		
		balls[0].SpeedX = 2;
		balls[0].SpeedY = 2;
		
		ClearFlag(balls[0].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB2 == 1)
	{
		balls[1].Active = true;
		
		balls[1].X = 158;
		balls[1].Y = 24;
		
		balls[1].SpeedX = 0;
		balls[1].SpeedY = 4;
		
		ClearFlag(balls[1].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvB3 == 1)
	{
		balls[2].Active = true;
		
		balls[2].X = 288;
		balls[2].Y = 24;
		
		balls[2].SpeedX = -2;
		balls[2].SpeedY = 2;
		
		ClearFlag(balls[2].cel->ccb_Flags, CCB_SKIP);
	}
	
	if (lvBomb == 0)
	{
		bomb.Active = false;
		
		SetFlag(bomb.cel->ccb_Flags, CCB_SKIP);
	}
	else
	{
		bomb.Active = true;
		bomb.X = 48;
		bomb.Y = 24;
		bomb.SpeedX = 1;
		bomb.SpeedY = 1;
		
		ClearFlag(bomb.cel->ccb_Flags, CCB_SKIP);
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
	}
	
	if (lvP2 >= 0)
	{
		planets[lvP2].Active = true;
		
		planets[lvP2].X = planets[lvP2].InitX + ((rand() % 20) - 10);
		planets[lvP2].Y = planets[lvP2].InitY + ((rand() % 20) - 10);
		
		ClearFlag(planets[lvP2].cel->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 50; x++) 
	{
		bType = LVDATA[level - 1][x];
		
		if (bType == 0)
		{
			bricks[x].ImageIdx = 0;
		}
		else
		{
			bricks[x].ImageIdx = (bType - 1) * 3; // 1, 2, 3, etc.
		}
		
		bricks[x].cel->ccb_SourcePtr = cel_AllBricks[bricks[x].ImageIdx]->ccb_SourcePtr;
		bricks[x].HP = (bType == 14 ? 999999999 : 4);
		
		bricks[x].Active = true;
		bricks[x].ExplodeFrame = 0;
		
		bricks[x].X = bricks[x].InitX;
		bricks[x].Y = bricks[x].InitY;	
			
		bricks[x].cel->ccb_XPos = Convert32_F16(bricks[x].InitX);
		bricks[x].cel->ccb_YPos = Convert32_F16(bricks[x].InitY);
			
		bricks[x].cel->ccb_HDX  = Convert32_F16(1) << 4;
		bricks[x].cel->ccb_VDY  = Convert32_F16(1);
		
		ClearFlag(bricks[x].cel->ccb_Flags, CCB_MARIA);
		ClearFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);
		
		if (bType == 0)
		{
			bricks[x].Active = false;
			bricks[x].HP = 0;
			
			SetFlag(bricks[x].cel->ccb_Flags, CCB_SKIP);
		}
		
		if (bType > 0 && bType < 14) CurrTarget++;
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
	
	UpdateGameStats();
}

void GameLoop()
{	
	while (true)
	{		
		InitGame();
		
		while (GameOver == false)
		{
			InitNewLevel(CurrLevel);

			AnimateNextLevel();
			
			InitEventUtility(1, 0, LC_Observer); 
			
			while (GameOver == false && CurrScore < CurrTarget) // In a real game some game over condition would break the loop back to the main start screen
			{			
				HandleInput();
				
				HandleAnimation();
				
				HandleTimer();
				
				UpdateGameStats();
				
				TrackPowerUps();

				DisplayGameplayScreen();
			}
			
			if (GameOver == false)
			{
				CurrLevel++;
				
				AnimateLevelComplete();
			}
			
			KillEventUtility();		
		}
		
		if (GameOver == true)
		{
			AnimateGameOver();
		}
		
		KillEventUtility();
	}
}

void HandleAnimation()
{
	AnimateBackground();
	AnimatePlanets();
	AnimateComet(); 
	AnimateBomb();
	AnimateBalls();
	AnimatePlayer1();
	AnimateFire();
	AnimateStars();
	AnimateLasers();
	AnimateBrickExplosions();
}

void HandleTimer() // TODO GAME OVER AS WE GET CLOSER
{
	TimeVal tvElapsed, tvNow;
	
	SampleSystemTimeTV(&tvNow);
	
	SubTimes(&dData.TvInit, &tvNow, &tvElapsed);
	
	CurrElapsedTime = tvElapsed.tv_Seconds;
}

void TrackPowerUps()
{
	if (++TimerBalls > (15 * 60))
	{
		TimerBalls = 0;
		
		if (AvailBalls < 24) AvailBalls++;
	}
	
	if (++TimerLasers > (3 * 60))
	{
		TimerLasers = 0;
		
		if (AvailLasers < 99) AvailLasers++;
	}
}


void loadData()
{
	int x, y, z;
	uint32 sColor;
	
	cel_AllBricks[0] = LoadCel("data/brick_a1.cel", MEMTYPE_CEL); 
	cel_AllBricks[1] = LoadCel("data/brick_a2.cel", MEMTYPE_CEL); 
	cel_AllBricks[2] = LoadCel("data/brick_a3.cel", MEMTYPE_CEL); 
	
	cel_AllBricks[3] = LoadCel("data/brick_b1.cel", MEMTYPE_CEL); 
	cel_AllBricks[4] = LoadCel("data/brick_b2.cel", MEMTYPE_CEL); 
	cel_AllBricks[5] = LoadCel("data/brick_b3.cel", MEMTYPE_CEL); 
	
	cel_AllBricks[6] = LoadCel("data/brick_c1.cel", MEMTYPE_CEL); 
	cel_AllBricks[7] = LoadCel("data/brick_c2.cel", MEMTYPE_CEL); 
	cel_AllBricks[8] = LoadCel("data/brick_c3.cel", MEMTYPE_CEL); 
	
	cel_AllBricks[9] = LoadCel("data/brick_d1.cel", MEMTYPE_CEL); 
	cel_AllBricks[10] = LoadCel("data/brick_d2.cel", MEMTYPE_CEL); 
	cel_AllBricks[11] = LoadCel("data/brick_d3.cel", MEMTYPE_CEL); 

	cel_AllBricks[12] = LoadCel("data/brick_e1.cel", MEMTYPE_CEL); 
	cel_AllBricks[13] = LoadCel("data/brick_e2.cel", MEMTYPE_CEL); 
	cel_AllBricks[14] = LoadCel("data/brick_e3.cel", MEMTYPE_CEL); 

	cel_AllBricks[15] = LoadCel("data/brick_f1.cel", MEMTYPE_CEL); 
	cel_AllBricks[16] = LoadCel("data/brick_f2.cel", MEMTYPE_CEL); 
	cel_AllBricks[17] = LoadCel("data/brick_f3.cel", MEMTYPE_CEL); 

	cel_AllBricks[18] = LoadCel("data/brick_g1.cel", MEMTYPE_CEL); 
	cel_AllBricks[19] = LoadCel("data/brick_g2.cel", MEMTYPE_CEL); 
	cel_AllBricks[20] = LoadCel("data/brick_g3.cel", MEMTYPE_CEL); 

	cel_AllBricks[21] = LoadCel("data/brick_h1.cel", MEMTYPE_CEL); 
	cel_AllBricks[22] = LoadCel("data/brick_h2.cel", MEMTYPE_CEL); 
	cel_AllBricks[23] = LoadCel("data/brick_h3.cel", MEMTYPE_CEL); 

	cel_AllBricks[24] = LoadCel("data/brick_i1.cel", MEMTYPE_CEL); 
	cel_AllBricks[25] = LoadCel("data/brick_i2.cel", MEMTYPE_CEL); 
	cel_AllBricks[26] = LoadCel("data/brick_i3.cel", MEMTYPE_CEL); 

	cel_AllBricks[27] = LoadCel("data/brick_j1.cel", MEMTYPE_CEL); 
	cel_AllBricks[28] = LoadCel("data/brick_j2.cel", MEMTYPE_CEL); 
	cel_AllBricks[29] = LoadCel("data/brick_j3.cel", MEMTYPE_CEL); 

	cel_AllBricks[30] = LoadCel("data/brick_k1.cel", MEMTYPE_CEL); 
	cel_AllBricks[31] = LoadCel("data/brick_k2.cel", MEMTYPE_CEL); 
	cel_AllBricks[32] = LoadCel("data/brick_k3.cel", MEMTYPE_CEL); 

	cel_AllBricks[33] = LoadCel("data/brick_l1.cel", MEMTYPE_CEL); 
	cel_AllBricks[34] = LoadCel("data/brick_l2.cel", MEMTYPE_CEL); 
	cel_AllBricks[35] = LoadCel("data/brick_l3.cel", MEMTYPE_CEL); 

	cel_AllBricks[36] = LoadCel("data/brick_m1.cel", MEMTYPE_CEL); 
	cel_AllBricks[37] = LoadCel("data/brick_m2.cel", MEMTYPE_CEL); 
	cel_AllBricks[38] = LoadCel("data/brick_m3.cel", MEMTYPE_CEL); 
	
	cel_AllBricks[39] = LoadCel("data/brick_z.cel", MEMTYPE_CEL); 
	
	
	for (x = 0; x < 40; x++)
	{		
		InitCCBFlags(cel_AllBricks[x]);
	}
	
	cel_AllBalls[0] = LoadCel("data/ball1.cel", MEMTYPE_CEL);
	
	for (x = 0; x < 1; x++)
	{		
		InitCCBFlags(cel_AllBalls[x]);
	}
	
	planets[0].cel = LoadCel("data/planet1.cel", MEMTYPE_CEL); // Sm Red Mercury
	planets[1].cel = LoadCel("data/planet2.cel", MEMTYPE_CEL); // Large Blue
	planets[2].cel = LoadCel("data/planet3.cel", MEMTYPE_CEL); // Sm Grey
	planets[3].cel = LoadCel("data/planet4.cel", MEMTYPE_CEL); // Sm Rings
	planets[4].cel = LoadCel("data/planet5.cel", MEMTYPE_CEL); // Large Green
	planets[5].cel = LoadCel("data/planet6.cel", MEMTYPE_CEL); // Med Earth
	planets[6].cel = LoadCel("data/planet7.cel", MEMTYPE_CEL); // Med Sun
	
	for (x = 0; x < 7; x++)
	{		
		InitCCBFlags(planets[x].cel);
		
		planets[x].FramesX = 0;
		planets[x].FramesY = 0;
	}
	
	planets[0].InitX = 80;
	planets[0].InitY = 42;
	planets[0].DelayX = 60;
	planets[0].DelayY = 12000;
	
	planets[1].InitX = 300;
	planets[1].InitY = 80;
	planets[1].DelayX = 36;
	planets[1].DelayY = 1024;
	
	planets[2].InitX = 260;
	planets[2].InitY = 140;
	planets[2].DelayX = 120;
	planets[2].DelayY = 480;
	
	planets[3].InitX = 320;
	planets[3].InitY = 40;
	planets[3].DelayX = 180;
	planets[3].DelayY = 360;
	
	planets[4].InitX = 320;
	planets[4].InitY = 90;
	planets[4].DelayX = 240;
	planets[4].DelayY = 12000;
	
	planets[5].InitX = 280;
	planets[5].InitY = 100;
	planets[5].DelayX = 48;
	planets[5].DelayY = 960;
	
	planets[6].InitX = 40;
	planets[6].InitY = 40;
	planets[6].DelayX = 1024;
	planets[6].DelayY = 12000;
	
	for (x = 1; x < 7; x++)
	{
		planets[x - 1].cel->ccb_NextPtr = planets[x].cel;
	}
	
	cel_AllBombs[0] = LoadCel("data/bomb1.cel", MEMTYPE_CEL);
	cel_AllBombs[1] = LoadCel("data/bomb2.cel", MEMTYPE_CEL);
	cel_AllBombs[2] = LoadCel("data/bomb3.cel", MEMTYPE_CEL);
	cel_AllBombs[3] = LoadCel("data/bomb4.cel", MEMTYPE_CEL);
	
	for (x = 0; x < 4; x++)
	{		
		InitCCBFlags(cel_AllBombs[x]);
	}
	
	cel_FireL[0] = CreateBackdropCel(1, 1, MakeRGB15(31, 31, 0), 80);
	cel_FireL[1] = CreateBackdropCel(1, 1, MakeRGB15(31, 11, 0), 60);
	cel_FireL[2] = CreateBackdropCel(1, 1, MakeRGB15(31, 21, 0), 90);
	cel_FireL[3] = CreateBackdropCel(1, 1, MakeRGB15(31, 14, 0), 70);	
	cel_FireL[4] = CreateBackdropCel(1, 1, MakeRGB15(31, 22, 0), 80);
	cel_FireL[5] = CreateBackdropCel(1, 1, MakeRGB15(31, 00, 0), 60);
	cel_FireL[6] = CreateBackdropCel(1, 1, MakeRGB15(31, 31, 0), 70);
	
	for (x = 1; x < 7; x++)
	{
		cel_FireL[x - 1]->ccb_NextPtr = cel_FireL[x];
	}
	
	cel_FireR[0] = CreateBackdropCel(1, 1, MakeRGB15(31, 24, 0), 80);
	cel_FireR[1] = CreateBackdropCel(1, 1, MakeRGB15(31, 02, 0), 60);
	cel_FireR[2] = CreateBackdropCel(1, 1, MakeRGB15(31, 14, 0), 40);
	cel_FireR[3] = CreateBackdropCel(1, 1, MakeRGB15(31, 26, 0), 90);	
	cel_FireR[4] = CreateBackdropCel(1, 1, MakeRGB15(31, 03, 0), 80);
	cel_FireR[5] = CreateBackdropCel(1, 1, MakeRGB15(31, 17, 0), 60);
	cel_FireR[6] = CreateBackdropCel(1, 1, MakeRGB15(31, 19, 0), 70);
	
	for (x = 1; x < 7; x++)
	{
		cel_FireR[x - 1]->ccb_NextPtr = cel_FireR[x];
	}
	
	for (x = 0; x < 7; x++)
	{		
		ClearFlag(cel_FireL[x]->ccb_Flags, CCB_SKIP);
		ClearFlag(cel_FireL[x]->ccb_Flags, CCB_LAST);
		
		ClearFlag(cel_FireR[x]->ccb_Flags, CCB_SKIP);
		ClearFlag(cel_FireR[x]->ccb_Flags, CCB_LAST);
		
		FireYL[x] = 400;
		FireYR[x] = 400;
	}
	
	for (x = 0; x < 100; x++)
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
		
		stars[x].cel = CreateBackdropCel(1, 1, sColor, rand() % 100);
		
		stars[x].X = rand() % 340;
		stars[x].Y = rand() % 240;
		
		stars[x].Speed = 1 + (rand() % 4);
	}
	
	for (x = 0; x < 100; x++)
	{		
		ClearFlag(stars[x].cel->ccb_Flags, CCB_SKIP);
		ClearFlag(stars[x].cel->ccb_Flags, CCB_LAST);
		
		PositionCel(stars[x].cel, stars[x].X, stars[x].Y);
	}
	
	for (x = 1; x < 100; x++)
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
	
	comet.cel = LoadCel("data/comet1.cel", MEMTYPE_CEL);
	InitCCBFlags(comet.cel);
	
	comet.X = 320;
	comet.Y = 50;
	comet.XFrames = 0;
	comet.YFrames = 0;
	
	PositionCel(comet.cel, comet.X, comet.Y);
	
	bomb.cel = CopyCel(cel_AllBombs[0]);
	
	bomb.X = 20;
	bomb.Y = 20;
	bomb.SpeedX = 1;
	bomb.SpeedY = 1;
	
	for (x = 0; x < 3; x++)
	{
		balls[x].cel = CopyCel(cel_AllBalls[0]);
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
	PositionCel(bomb.cel, 24, 24);
	
	cel_Text1 = LoadCel("data/txt_top.cel", MEMTYPE_CEL);
	InitCCBFlags(cel_Text1);
	
	PositionCel(cel_Text1, 23, 10);
	
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
		bricks[x].Y = 48 + (z * 12);
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
	
	cel_BG = LoadCel("data/bg_1.cel", MEMTYPE_CEL); // Wrap around BG 320px wide
	InitCCBFlags(cel_BG);
	
	cel_BG1 = CopyCel(cel_BG);
	cel_BG2 = CopyCel(cel_BG);
	
	PositionCel(cel_BG1, BG_X, BG_Y); 
	PositionCel(cel_BG2, BG_X + 320, BG_Y);
	
	cel_BG1->ccb_NextPtr = cel_BG2;
	cel_BG2->ccb_NextPtr = planets[0].cel;	
	planets[6].cel->ccb_NextPtr = comet.cel;
	comet.cel->ccb_NextPtr = stars[0].cel;
	stars[99].cel->ccb_NextPtr = cel_Top;
	cel_Top->ccb_NextPtr = cel_Left;
	cel_Left->ccb_NextPtr = cel_Right;
	cel_Right->ccb_NextPtr = cel_Bottom;
	cel_Bottom->ccb_NextPtr = lasersL[0].cel;
	lasersR[7].cel->ccb_NextPtr = balls[0].cel;
	balls[2].cel->ccb_NextPtr = bomb.cel;
	bomb.cel->ccb_NextPtr = bricks[0].cel;
	bricks[49].cel->ccb_NextPtr = cel_Player1;
	cel_Player1->ccb_NextPtr = cel_FireL[0];
	cel_FireL[6]->ccb_NextPtr = cel_FireR[0];
	cel_FireR[6]->ccb_NextPtr = cel_Text1;
	cel_Text1->ccb_NextPtr = TrackedNumbers[0].cel_NumCels[0];
	
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
		BG_Y = ((rand() % 5) - 2);
	}
	
	SetCelNumbers(0, CurrLevel);
	SetCelNumbers(1, AvailLives);
	SetCelNumbers(2, AvailBalls);
	SetCelNumbers(3, AvailLasers);
	SetCelNumbers(4, AvailTime - CurrElapsedTime);
	
	SetCelNumbers(5, TotalScore);
}

void DisplayGameplayScreen()
{	
	//displayMem(screen.sc_BitmapItems[ visibleScreenPage ]);
	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_BG1);
	
    DisplayScreen(screen.sc_Screens[visibleScreenPage], 0);
	
	visibleScreenPage = (1 - visibleScreenPage);

	ioInfo.ioi_Recv.iob_Buffer = bitmaps[visibleScreenPage]->bm_Buffer;	
	DoIO(VRAMIOReq, &ioInfo);
}

void DisplayGameOverScreen(CCB *celBG, CCB *celFG)
{
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], celBG);	
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_BG1);
	
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
	DrawCels(screen.sc_BitmapItems[ visibleScreenPage ], cel_BG1);
	
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
	
	AnimateBomb();
	AnimateBalls();
	AnimateStars();
	AnimateComet();
	AnimatePlanets();
	
	for (x = 0; x < 90; x++)
	{		
		AnimateStars();
		AnimateComet();
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
		AnimateComet();
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
		AnimateComet();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	cel_Player1->ccb_HDX = Convert32_F16(1) << 4;
	cel_Player1->ccb_VDY = Convert32_F16(1);
	
	for (x = 0; x < 7; x++)
	{
		FireYL[x] = P1_Y + 18 + (x * 4);
		FireYR[x] = P1_Y + 18 + (x * 4);
		
		ClearFlag(cel_FireL[x]->ccb_Flags, CCB_SKIP);
		ClearFlag(cel_FireR[x]->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 60; x++)
	{
		AnimatePlayer1();
		AnimateFire();
		AnimateStars();
		AnimateComet();
		AnimatePlanets();
		DisplayGameplayScreen();
	}
	
	SampleSystemTimeTV(&dData.TvInit);
}

void AnimateLevelComplete()
{
	int x;
	
	for (x = 0; x < 60; x++) // Finish up any pending animations
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
	
	for (x = 0; x < 7; x++)
	{
		SetFlag(cel_FireL[x]->ccb_Flags, CCB_SKIP);
		SetFlag(cel_FireR[x]->ccb_Flags, CCB_SKIP);
	}
	
	for (x = 0; x < 3; x++)
	{
		SetFlag(balls[x].cel->ccb_Flags, CCB_SKIP);
	}
	
	SetFlag(bomb.cel->ccb_Flags, CCB_SKIP);
	
	P1_Speed = 0;
	
	while (P1_X != 130)
	{		
		if (P1_X < 130) P1_X++;
		if (P1_X > 130) P1_X--;
		
		AnimatePlayer1();
		AnimateComet();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	for (x = 0; x < 30; x++)
	{		
		AnimatePlayer1();
		AnimateComet();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	while (--P1_Y >= 21)
	{		
		if (P1_Y % 2 == 0) P1_X++;
		
		if (P1_Y > 100) P1_X++;
		
		cel_Player1->ccb_HDX = DivSF16(Convert32_F16(P1_Y), Convert32_F16(200)) << 4;
		cel_Player1->ccb_VDY = DivSF16(Convert32_F16(P1_Y), Convert32_F16(200));
		
		AnimatePlayer1();
		AnimateComet();
		AnimatePlanets();
		AnimateStars();
		
		DisplayGameplayScreen();
	}
	
	SetFlag(cel_Player1->ccb_Flags, CCB_SKIP); 
	
	// TODO SHOW SCORES - Timer and Balls Only bonus maybe
	
	InitNumberCel(6, 160, 40, CurrLevel, false);
	InitNumberCel(7, 160, 100, AvailTime, false);
	
	for (x = 0; x < 30; x++)
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
	
	while (++CurrElapsedTime < AvailTime)
	{
		for (x = 0; x < 4; x++)
		{
			HandleAnimation();
			
			DisplayGameplayScreen();
		}
	}
	
	InitNumberCel(6, -200, -200, 0, false);
	InitNumberCel(7, -200, -200, 0, false);
	
	for (x = 0; x < 30; x++)
	{
		HandleAnimation();
		
		DisplayGameplayScreen();
	}
}

void AnimateGameOver()
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
}

void AnimateBomb()
{
	if (bomb.Active == false) return;
	
	{
		int i;
		
		int currX1 = bomb.X;
		int currX2 = bomb.X + 7;
		
		int currY1 = bomb.Y;
		int currY2 = bomb.Y + 7;
		
		int newX1 = bomb.X + bomb.SpeedX;
		int newX2 = newX1 + 7;
		
		int newY1 = bomb.Y + bomb.SpeedY;
		int newY2 = newY1 + 7;
		
		int collisionX = 0;
		int collisionY = 0;
		
		if (++bomb.AnimFrame >= 184) bomb.AnimFrame = 0;
		
		if (bomb.AnimFrame == 0) bomb.cel->ccb_SourcePtr = cel_AllBombs[0]->ccb_SourcePtr;
		if (bomb.AnimFrame == 120) bomb.cel->ccb_SourcePtr = cel_AllBombs[1]->ccb_SourcePtr;
		if (bomb.AnimFrame == 132) bomb.cel->ccb_SourcePtr = cel_AllBombs[2]->ccb_SourcePtr;
		if (bomb.AnimFrame == 148) bomb.cel->ccb_SourcePtr = cel_AllBombs[3]->ccb_SourcePtr;
		if (bomb.AnimFrame == 160) bomb.cel->ccb_SourcePtr = cel_AllBombs[2]->ccb_SourcePtr;
		if (bomb.AnimFrame == 172) bomb.cel->ccb_SourcePtr = cel_AllBombs[1]->ccb_SourcePtr;
		
		// TODO MAKE CD WORK
		
		if (bomb.SpeedX < 0 && newX1 < 20) collisionX++;
		if (bomb.SpeedX > 0 && newX2 > 300) collisionX++;
		if (bomb.SpeedY < 0 && newY1 < 20) collisionY++;
		
		if (bomb.SpeedY > 0 && newY2 > 240)
		{
			collisionY++;
			
			GameOver = true;
			
			return;
		}
		
		if (bomb.SpeedY >= 0)
		{
			if (newX2 >= P1_X && newX1 <= P1_X + P1_WIDTH - 1)
			{
				if (newY2 == P1_Y)
				{
					collisionY++;
				}
			}
		}

		for (i = 49; i >= 0; i--)
		{
			if (bricks[i].HP < 1) continue;		

			if (newX2 >= bricks[i].X && newX1 <= bricks[i].X + BRICK_WIDTH - 1) // Within Width
			{
				if (bomb.SpeedY > 0) // Moving down
				{			
					if (currY2 < bricks[i].Y && newY2 >= bricks[i].Y)
					{
						collisionY++;
					}
				}
				else
				{
					if (currY1 > bricks[i].Y + BRICK_HEIGHT - 1 && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1)
					{
						collisionY++;
					}
				}
			}
			
			if (collisionY == 2) break;
			
			if (newY2 >= bricks[i].Y && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1) // Within Width
			{
				if (bomb.SpeedX > 0) // Moving down
				{			
					if (currX2 < bricks[i].X && newX2 >= bricks[i].X)
					{
						collisionX++;
					}
				}
				else
				{
					if (currX1 > bricks[i].X + BRICK_WIDTH - 1 && newX1 <= bricks[i].X + BRICK_WIDTH - 1)
					{
						collisionX++;
					}
				}
			}
			
			if (collisionX == 2) break;
		}
		
		if (collisionX == 2 || collisionY == 2)
		{
			if (collisionX == 2)
			{
				bomb.SpeedX *= -1;
			}
			else
			{
				bomb.SpeedY *= -1;
			}
		}
		else
		{
			if (collisionX == 1) bomb.SpeedX *= -1;
			if (collisionY == 1) bomb.SpeedY *= -1;
		}
		
		bomb.X += bomb.SpeedX;
		bomb.Y += bomb.SpeedY;
		
		PositionCel(bomb.cel, bomb.X, bomb.Y);
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
		currX2 = balls[j].X + 8;
		
		currY1 = balls[j].Y;
		currY2 = balls[j].Y + 8;
		
		newX1 = balls[j].X + balls[j].SpeedX;
		newX2 = newX1 + 8;
		
		newY1 = balls[j].Y + balls[j].SpeedY;
		newY2 = newY1 + 8;
		
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
					collisionY++;
					
					balls[j].SpeedX += ( ((currX1 + 2) - (P1_X + (P1_WIDTH - 1) / 2)) / 12 ); // Range of -3 to +3
					
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
						
						HandleBrickCollision(i, 2);
					}
				}
				else
				{
					if (currY1 > bricks[i].Y + BRICK_HEIGHT - 1 && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1)
					{
						collisionY++;
						
						HandleBrickCollision(i, 2);
					}
				}
			}
			
			if (collisionY == 2) break;
			
			if (newY2 >= bricks[i].Y && newY1 <= bricks[i].Y + BRICK_HEIGHT - 1) // Within Width
			{
				if (balls[j].SpeedX > 0) // Moving down
				{			
					if (currX2 < bricks[i].X && newX2 >= bricks[i].X)
					{
						collisionX++;
						
						HandleBrickCollision(i, 2);
					}
				}
				else
				{
					if (currX1 > bricks[i].X + BRICK_WIDTH - 1 && newX1 <= bricks[i].X + BRICK_WIDTH - 1)
					{
						collisionX++;
						
						HandleBrickCollision(i, 2);
					}
				}
			}
			
			if (collisionX == 2) break;
		}
	
		if (collisionX == 2 || collisionY == 2)
		{
			if (collisionX == 2)
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
			if (collisionX == 1) balls[j].SpeedX *= -1;
			if (collisionY == 1) balls[j].SpeedY *= -1;
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

void AnimateComet()
{
	bool change = false;
	
	if (++comet.XFrames > 3)
	{
		comet.XFrames = 0;
		
		change = true;
		
		if (--comet.X < -20)
		{
			comet.X = 340;
			comet.Y = (rand() % 120);
		}
	}
	
	if (++comet.YFrames > 5)
	{
		comet.YFrames = 0;
		
		change = true;
		
		comet.Y++;
	}
	
	if (change == true)
	{
		PositionCel(comet.cel, comet.X, comet.Y);
	}
}

void AnimateFire()
{
	int x;
	
	CounterFire = 1 - CounterFire;
	
	if (CounterFire == 0) return;
	
	for (x = 0; x < 7; x++)
	{
		if (++FireYL[x] > P1_Y + 48)
		{
			FireYL[x] = P1_Y + FIRE_OFFSET_Y;
			
			cel_FireL[x]->ccb_XPos = Convert32_F16(P1_X + (FIRE_OFFSET_X - 1) + ((rand() % 3) - 1));
		}
		
		cel_FireL[x]->ccb_YPos = Convert32_F16(FireYL[x]);
		
		if (++FireYR[x] > P1_Y + 48)
		{
			FireYR[x] = P1_Y + FIRE_OFFSET_Y;
			
			cel_FireR[x]->ccb_XPos = Convert32_F16(P1_X + (P1_WIDTH - FIRE_OFFSET_X) + ((rand() % 3) - 1));
		}
		
		cel_FireR[x]->ccb_YPos = Convert32_F16(FireYR[x]);
	}
}

void AnimateStars()
{
	int x;
	
	for (x = 0; x < 100; x++)
	{
		stars[x].X = stars[x].X - stars[x].Speed;
		
		if (stars[x].X < -20)
		{
			stars[x].X = (rand() % 320) + 320;
		}
		
		PositionCel(stars[x].cel, stars[x].X, stars[x].Y);
	}
}

void AnimateBackground() // TODO - Based on user movement maybe?
{
	if (++Counter1 >= 1024) Counter1 = 0;
	
	if (Counter1 % 4 == 0)
	{
		if (--BG_X <= -320) BG_X = 0;
			
		PositionCel(cel_BG1, BG_X, BG_Y);
		PositionCel(cel_BG2, BG_X + 320, BG_Y);
	}	
}

void AnimatePlayer1()
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
	
	HoverCounter++;
	
	if (HoverCounter == 20) P1_Y--;
	if (HoverCounter == 40) P1_Y++;
	if (HoverCounter == 60) P1_Y++;
	if (HoverCounter == 80) P1_Y--;
	
	if (HoverCounter > 80) HoverCounter = 0;
	
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
		
		if (tvDiff.tv_Seconds < 2) return;
		
		for (x = 0; x < 3; x++)
		{		
			if (balls[x].Active == true) ab++;
		}
		
		if (ab == 3) return; // Only 3 active allowed at a time
		
		for (x = 0; x < 3; x++)
		{		
			if (balls[x].Active == true) continue;
			
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
		TotalScore += 25;
		
		SetFlag(bricks[idx].cel->ccb_Flags, CCB_MARIA);
		
		if (bricks[idx].IsBomb) // TODO
		{
						
		}
	}
	else if (bricks[idx].HP < 3)
	{
		TotalScore += 5;
		
		bricks[idx].cel->ccb_SourcePtr = cel_AllBricks[(bricks[idx].ImageIdx + 3) - bricks[idx].HP]->ccb_SourcePtr; // TODO CONFIRM THIS
	}	
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
	
	if (joyBits & ControlX) 
	{
		if (kpStop == false)
		{			
	
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

		}

		kpStart = true;
	}
	else
	{
		kpStart = false;
	}

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
		if (kpLeft == false || swLeft % 4 == 0) // Slightly stagger the increase speed
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
		if (kpRight == false || swRight % 4 == 0) // Slightly stagger the increase speed
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
	
	if (kpLeft == false && kpRight == false) MoveDown();
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

void Move()
{

}



