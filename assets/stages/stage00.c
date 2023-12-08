#include <nusys.h>
#include "../../config.h"
#include "../sprites/player_sprite.h"

#define CheckController(cont) (contPattern & (1<<(cont)))

typedef struct {
	s16 posX;
	s16 posY;
	u16 sizeX;
	u16 sizeY;
	s8 velX;
	s8 velY;
} Square;

// Globals
static u8 b;
static Square player = { (SCREEN_WD / 2) - (64 / 2), (SCREEN_HT / 2) - (64 / 2), 16, 16, 0, 0 }; // Player object
static Square enemy = { (SCREEN_WD / 2) - (64 / 2), (SCREEN_HT / 2) - (64 / 2), 10, 10, 1, 1 }; // Enemy object

void ClearBackground(u8 r, u8 g, u8 b);
void CreateSquare(u8 r, u8 g, u8 b, Square *square);
void MoveSquare(Square *square);
void MoveEnemy(Square *enemy);
void CheckPlayerScreenCollision(Square *square);
void CheckEnemyScreenCollision(Square *enemy);
void DrawPlayer(Square *player);

void stage00_init(void) {
	b = 255;
}

void stage00_update(void) {
	nuContDataGetExAll(contdata);
	b -=5; // Because this is unsigned, -5 will wrap around to 250
	MoveSquare(&player); // Update the position of the player
	MoveEnemy(&enemy); // Update the position of the enemy
	CheckPlayerScreenCollision(&player); // Check for collision with wall and player
	CheckEnemyScreenCollision(&enemy); // Check for collision between wall and enemy
}

void stage00_draw(void) {
    glistp = glist;
    RCPInit(glistp);
    ClearBackground(0, 0, 255); // Clear background to blue
	//ClearBackground(0, 0, b); // Change background color to test not crashing
	
	//CreateSquare(255, 0, 0, &player); // Create red square for player on screen
	
	CreateSquare(0, 255, 0, &enemy); // Create enemy square on screen
	
	DrawPlayer(&player); // Draw the player sprite

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    nuGfxTaskStart(glist, (s32)(glistp - glist) * sizeof(Gfx),
                   NU_GFX_UCODE_F3DEX, NU_SC_SWAPBUFFER);
}

void ClearBackground(u8 r, u8 g, u8 b) {

	// Clear the Z-Buffer first
	gDPSetCycleType(glistp++, G_CYC_FILL);
	gDPSetDepthImage(glistp++, nuGfxZBuffer); // nuGfxZBuffer is Nusys’ Z-Buffer 
	gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, nuGfxZBuffer);
	gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0)));
	gDPFillRectangle(glistp++, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
	gDPPipeSync(glistp++);

	// Clear the background
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

void MoveSquare(Square *square) {
	
	// Check if controller 1 is plugged in
	//if (CheckController(1) == 1) {
		
		// Check if analog x direction is being held, update velocity accordingly
		if ( contdata[0].stick_x > 16 ) {
			square->velX = 1; // Fixed velocity for now
		} else if ( contdata[0].stick_x < -16 ) {
			square->velX = -1; // Fixed velocity for now
		} else {
			square->velX = 0; // Stop moving
		}
		
		// Check if analog y direction is being held, update velocity accordingly
		if ( contdata[0].stick_y > 16 ) {
			square->velY = -1; // Fixed velocity for now
		} else if ( contdata[0].stick_y < -16 ) {
			square->velY = 1; // Fixed velocity for now
		} else {
			square->velY = 0; // Stop moving
		}
	//}
	
	// Update square position
	square->posX += square->velX;
	square->posY += square->velY;
}

void MoveEnemy(Square *enemy) {
	enemy->posX += enemy->velX;
	enemy->posY += enemy->velY;
}

void CheckPlayerScreenCollision(Square *square) {
	
	// Check if square hit left or right side of screen
	if ( (square->posX < 0) ) {
		square->posX = 0;
	} else if ((square->posX + square->sizeX) >= SCREEN_WD) {
		square->posX = SCREEN_WD - square->sizeX;
	}
	
	// Check if square hit top or bottom of screen
	if ( (square->posY < 0) ) {
		square->posY = 0;
	} else if ((square->posY + square->sizeY) >= SCREEN_HT) {
		square->posY = SCREEN_HT - square->sizeY;
	}
}

void CheckEnemyScreenCollision(Square *enemy) {
	// Check if enemey hit left or right side of screen
	if ( (enemy->posX <= 0) || ((enemy->posX + enemy->sizeX) >= SCREEN_WD) ) {
		enemy->velX *= -1; // Reverse velocity, enemy bounced off left or right of screen
	}

	// Check if enemy hit top or bottom of screen
	if ( (enemy->posY <= 0) || ((enemy->posY + enemy->sizeY) >= SCREEN_HT) ) {
		enemy->velY *= -1;
	}
}

void DrawPlayer(Square *player) {

	gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(glistp++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE);
    gDPSetDepthSource(glistp++, G_ZS_PRIM);
    gDPSetPrimDepth(glistp++, 0, 0);
    gDPSetTexturePersp(glistp++, G_TP_NONE);
    gDPLoadTextureBlock(glistp++,
        player_sprite, 
        G_IM_FMT_RGBA, G_IM_SIZ_16b, 
        16, 16, 0, 
        G_TX_WRAP, G_TX_WRAP, 
        G_TX_NOMASK, G_TX_NOMASK, 
        G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(glistp++, 
        player->posX << 2, player->posY << 2, 
        (player->posX + player->sizeX) << 2, (player->posY + player->sizeY) << 2,
        G_TX_RENDERTILE, 
        0 << 5, 0 << 5, 
        1 << 10, 1 << 10);
    gDPPipeSync(glistp++);
}

