#include "polygon.h"

#include <vector>
#include <cstdlib>
#include <string>
#include <list>
#include <algorithm>

/* clip rectangle; clips to the screen */
int ClipMinX = 0, ClipMinY = 0;
int ClipMaxX = SCREEN_WIDTH, ClipMaxY = SCREEN_HEIGHT;

int RecalcAllXforms = 1, NumObjects = 0;
Xform WorldViewXform;   /* initialized from floats */
						/* pointers to objects */
Object *ObjectList[MAX_OBJECTS];

static std::vector<bcolor_t> backbuffer;
static bcolor_t PaletteBlock[256]; /* 256 RGBA entries */

int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

void InitializePalette()
{
	static unsigned char Gamma4Levels[] = { 0, 39, 53, 63 };
	static unsigned char Gamma64Levels[] = {
		0, 10, 14, 17, 19, 21, 23, 24, 26, 27, 28, 29, 31, 32, 33, 34,
		35, 36, 37, 37, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 46,
		47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55,
		56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63,
	};
	int Red, Green, Blue, Index;
	for (Red = 0; Red<4; Red++) {
		for (Green = 0; Green<4; Green++) {
			for (Blue = 0; Blue<4; Blue++) {
				Index = (Red << 4) + (Green << 2) + Blue;
				PaletteBlock[Index].r = Gamma4Levels[Red];
				PaletteBlock[Index].g = Gamma4Levels[Green];
				PaletteBlock[Index].b = Gamma4Levels[Blue];
				PaletteBlock[Index].a = 0xFF;
			}
		}
	}

	for (Red = 0; Red<64; Red++) {
		PaletteBlock[64 + Red].r = Gamma64Levels[Red];
		PaletteBlock[64 + Red].g = 0;
		PaletteBlock[64 + Red].b = 0;
	}

	for (Green = 0; Green<64; Green++) {
		PaletteBlock[128 + Green].r= 0;
		PaletteBlock[128 + Green].g = Gamma64Levels[Green];
		PaletteBlock[128 + Green].b = 0;
	}

	for (Blue = 0; Blue<64; Blue++) {
		PaletteBlock[192 + Blue].r = 0;
		PaletteBlock[192 + Blue].g = 0;
		PaletteBlock[192 + Blue].b = Gamma64Levels[Blue];
	}
}
/* Converts a model color (a color in the RGB color cube, in the current
color model) to a color index for mode X. Pure primary colors are
special-cased, and everything else is handled by a 2-2-2 model. */
int ModelColorToColorIndex(bcolor_t c)
{
	if (c.r== 0) {
		if (c.g == 0) {
			/* Pure blue */
			return(192 + (c.b >> 2));
		}
		else if (c.b == 0) {
			/* Pure green */
			return(128 + (c.g >> 2));
		}
	}
	else if ((c.g == 0) && (c.b == 0)) {
		/* Pure red */
		return(64 + (c.r >> 2));
	}
	/* Multi-color mix; look up the index with the two most significant bits
	of each color component */
	return(((c.r & 0xC0) >> 2) | ((c.g & 0xC0) >> 4) | ((c.b & 0xC0) >> 6));
}

void blackbox_init(size_t width, size_t height) {
	SCREEN_WIDTH = (int)width;
	SCREEN_HEIGHT = (int)height;
	/* clip rectangle; clips to the screen */
	backbuffer.resize(width*height);
	ClipMinX = 0, ClipMinY = 0;
	ClipMaxX = SCREEN_WIDTH, ClipMaxY = SCREEN_HEIGHT;
	InitializePalette();
	InitializeFixedPoint(); /* set up fixed-point data */
	InitializeCubes();      /* set up cubes and add them to object list; other
							objects would be initialized now, if there were any */
}


void DrawPixel(int x, int y, int index) {
	if (x < 0 || x >= SCREEN_WIDTH) return;
	if (y < 0 || y >= SCREEN_HEIGHT) return;
	backbuffer[y * SCREEN_WIDTH + x] = PaletteBlock[index];
}
void FillRectangleX(int StartX, int StartY, int EndX, int EndY, unsigned int PageBase, int Color) {
	// we have to swap sides
	/* We'll always draw top to bottom, to reduce the number of cases we have to
	handle, and to make lines between the same endpoints draw the same pixels */

	if (StartY > EndY) {
		std::swap(StartY, EndY);
		std::swap(StartX, EndX);
	}
	//EndX = std::min((int)backbuffer->getSize().x, EndX);
	//EndY = std::min((int)backbuffer->getSize().y, EndY);
	//if (StartX < 0 || EndX < StartX || (StartX + EndX) >= (int)backbuffer->getSize().x) return;
	//if (StartY < 0 || EndY < StartY || (StartY + EndY) >= (int)backbuffer->getSize().y) return;
	
	auto c = PaletteBlock[Color]; // slow, should atleast use memset &m_pixels[(x + y * m_size.x) * 4];

	//uint8_t* line = const_cast<uint8_t*>(backbuffer->getPixelsPtr() + x + y * SCREEN_WIDTH);
#if 0
	for (int y = StartY; y < EndY && y < (int)backbuffer->getSize().y; y++) {
		for (int x = StartX; x < EndX && x < (int)backbuffer->getSize().x ; x++) {
			backbuffer->setPixel(x, y, c);

			Uint8* pixel = &m_pixels[(x + y * m_size.x) * 4];
			*pixel++ = color.r;
			*pixel++ = color.g;
			*pixel++ = color.b;
			*pixel++ = color.a;

		}
	}
#else
	const size_t width = std::min(EndX - StartX, SCREEN_WIDTH);
	auto line = &backbuffer[StartX + StartY * SCREEN_WIDTH];
	for (int y = StartY; y < EndY && y < SCREEN_HEIGHT; y++) {
		std::fill_n(line, width, c);
		line += SCREEN_WIDTH;
	}

#endif
}

uint32_t* blackbox_refresh() {
	int Done = 0;
	int i = 0;
	Object *ObjectPtr;
	memset(backbuffer.data(), 0, SCREEN_HEIGHT*SCREEN_WIDTH * sizeof(uint32_t));
	/* Keep transforming the cube, drawing it to the undisplayed page,
	and flipping the page to show it */
		/* For each object, regenerate viewing info, if necessary */
	for (i = 0; i<NumObjects; i++) {
		if ((ObjectPtr = ObjectList[i])->RecalcXform ||
			RecalcAllXforms) {
			ObjectPtr->RecalcFunc(); //ObjectPtr->RecalcFunc(ObjectPtr);
			ObjectPtr->RecalcXform = 0;
		}
	}
	RecalcAllXforms = 0;
	/* For each object, clear the portion of the non-displayed page
	that was drawn to last time, then reset the erase extent */
	for (i = 0; i<NumObjects; i++) {
		ObjectPtr = ObjectList[i];
		FillRectangleX(ObjectPtr->EraseRect.Left,
			ObjectPtr->EraseRect.Top,
			ObjectPtr->EraseRect.Right,
			ObjectPtr->EraseRect.Bottom,
			0, 0);
		ObjectPtr->EraseRect.Left =
			ObjectPtr->EraseRect.Top = 0x7FFF;
		ObjectPtr->EraseRect.Right =
			ObjectPtr->EraseRect.Bottom = 0;
	}
	/* Draw all objects */
	for (i = 0; i<NumObjects; i++)
		ObjectList[i]->DrawFunc(); // ObjectList[i]->DrawFunc(ObjectList[i]);
	/* Flip to display the page into which we just drew */
	/* Move and reorient each object */
	for (i = 0; i<NumObjects; i++)
		ObjectList[i]->MoveFunc(); ///ObjectList[i]->MoveFunc(ObjectList[i]);

	return (uint32_t*)backbuffer.data();
}