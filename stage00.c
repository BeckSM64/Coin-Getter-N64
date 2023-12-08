#include <nusys.h>
#include "config.h"

typedef struct {
	u16 posX;
	u16 posY;
	u16 sizeX;
	u16 sizeY;
	s8 velX;
	s8 velY;
} Square;

// Globals
static u8 b;
static Square square = { (SCREEN_WD / 2) - (64 / 2), (SCREEN_HT / 2) - (64 / 2), 25, 25, -1, 1 };

void ClearBackground(u8 r, u8 g, u8 b);
void CreateSquare(u8 r, u8 g, u8 b, Square *square);
void MoveSquare(Square *square, u16 posX, u16 posY);
void CheckScreenCollision(Square *square);

void stage00_init(void) {
	b = 255;
}

void stage00_update(void) {
	b -=5; // because this is unsigned, -5 will wrap around to 250
	MoveSquare(&square, square.posX + square.velX, square.posY + square.velY);
	CheckScreenCollision(&square);
}

void stage00_draw(void) {
    glistp = glist;
    RCPInit(glistp);
    // ClearBackground(0, 0, 255); // Clear background to blue
	ClearBackground(0, 0, b); // Change background color to test not crashing
	
	CreateSquare(255, 0, 0, &square); // Create red square on screen

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx),
                   NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
}

void ClearBackground(u8 r, u8 g, u8 b) {

    gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                     osVirtualToPhysical(nuGfxCfb_ptr));
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(r, g, b, 1) << 16 |
                               GPACK_RGBA5551(r, g, b, 1)));
    gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPPipeSync(glistp++);
}

void CreateSquare(u8 r, u8 g, u8 b, Square *square) {

	gDPSetCycleType(glistp++, G_CYC_FILL);
    gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD,
                     osVirtualToPhysical(nuGfxCfb_ptr));
    gDPSetFillColor(glistp++, (GPACK_RGBA5551(r, g, b, 1) << 16 |
                               GPACK_RGBA5551(r, g, b, 1)));
    gDPFillRectangle(glistp++, square->posX, square->posY, square->posX + square->sizeX, square->posY + square->sizeY);
    gDPPipeSync(glistp++);
}

void MoveSquare(Square *square, u16 posX, u16 posY) {
	square->posX = posX;
	square->posY = posY;
}

void CheckScreenCollision(Square *square) {
	
	// Check if square hit left or right side of screen
	if ( (square->posX <= 0) || ((square->posX + square->sizeX) >= SCREEN_WD) ) {
		square->velX *= -1; // Reverse velocity, square bounced off left or right of screen
	}
	
	// Check if square hit top or bottom of screen
	if ( (square->posY <= 0) || ((square->posY + square->sizeY) >= SCREEN_HT) ) {
		square->velY *= -1;
	}
}
