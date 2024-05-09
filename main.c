#include <nusys.h>
#include "assets/stages/stage00.h"
#include "config.h"

#define CheckController(cont) (contPattern & (1<<(cont)))

// Globals
NUContData contdata[1]; // Number of controllers to keep track of, 1
u8 contPattern; // Which controllers are plugged in
char mem_heap[1024*512]; // Half a megabyte of heap memory

void vsyncCallback(int pendingTaskCount) {
	stage00_update();
	if (pendingTaskCount < 1) {
		stage00_draw();
	}
}

void mainproc(void *dummy) {
	nuGfxInit();
	
	// Initialize the heap for dynamic memory allocation
	if (InitHeap(mem_heap, sizeof(mem_heap)) == -1) {
	
		// Kill the program if failed to initialize heap memory
		return;
	}
	
	// Initialize controller manager
	contPattern = nuContInit();
	
	// Set the callback
	nuGfxFuncSet((NUGfxFunc)vsyncCallback);
	
	// Activate ability to output to graphics to display
	nuGfxDisplayOn();
	while(1);
}
