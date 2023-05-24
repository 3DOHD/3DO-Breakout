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

#ifndef BRICKBREAKER_H
#define BRICKBREAKER_H
#include "displayutils.h"
#include "debug.h"
#include "nodes.h"
#include "kernelnodes.h"
#include "list.h"
#include "folio.h"
#include "task.h"
#include "kernel.h"
#include "mem.h"
#include "operamath.h"
#include "math.h"
#include "semaphore.h"
#include "io.h"
#include "strings.h"
#include "stdlib.h"
#include "event.h"

#include "controlpad.h"

#include "stdio.h"
#include "graphics.h"
#include "audio.h"

#include "3dlib.h"

#include "timerutils.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_SIZE_IN_BYTES (SCREEN_WIDTH * SCREEN_HEIGHT * 2)
#define SCREEN_PAGES 2
#endif


typedef struct Brick
{
	int X;
	int Y;
	int InitX;
	int InitY;
	int Style; 
	int HP;	
	int ExplodeFrame;
	int ImageIdx;
	int AnimMinIdx;
	int AnimMaxIdx;
	bool Active;
	bool Animate;
	bool IsBomb;
	CCB *cel;
} Brick; 

typedef struct Star
{
	int X;
	int Y;
	int Speed;
	CCB *cel;
} Star;

typedef struct Comet
{
	int X;
	int Y;
	int XFrames;
	int YFrames;
	CCB *cel;
} Comet;

typedef struct Planet
{
	int X;
	int Y;
	int InitX;
	int InitY;
	int FramesX;
	int FramesY;
	int DelayX;
	int DelayY;
	bool Active;
	CCB *cel;
} Planet;

typedef struct Ball
{
	int Damage;
	int X;
	int Y;
	int SpeedX;
	int SpeedY; // Because of 3DO floating point numbers.. need a numerator and divisor
	bool Active;
	CCB *cel;
} Ball; 

typedef struct Bomb
{
	int X;
	int Y;
	int SpeedX;
	int SpeedY; // Because of 3DO floating point numbers.. need a numerator and divisor
	int AnimFrame;
	bool Active;
	CCB *cel;
} Bomb;   

typedef struct Laser
{
	int X;
	int Y;
	bool Active;
	CCB *cel;
} Laser; 

