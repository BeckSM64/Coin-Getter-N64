#include <nusys.h>
#include "stage00.h"
#include "config.h"

void vsyncCallback(int pendingTaskCount) {
	stage00_update();
	if (pendingTaskCount < 1) {
		stage00_draw();
	}
}

void mainproc(void *dummy) {
	nuGfxInit();
	nuGfxFuncSet((NUGfxFunc)vsyncCallback);
	nuGfxDisplayOn();
	while(1);
}
