#include <nusys.h>
#include "stage00.h"
#include "config.h"

#define CheckController(cont) (contPattern & (1<<(cont)))

NUContData contdata[1]; // Number of controllers to keep track of, 1
u8 contPattern; // Which controllers are plugged in

void vsyncCallback(int pendingTaskCount) {
	stage00_update();
	if (pendingTaskCount < 1) {
		stage00_draw();
	}
}

void mainproc(void *dummy) {
	nuGfxInit();
	contPattern = nuContInit(); // Initialize controller manager
	nuGfxFuncSet((NUGfxFunc)vsyncCallback);
	nuGfxDisplayOn();
	while(1);
}
