#define _POSIX_C_SOURCE 200112L

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "mzapo_regs.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "colors.h"
#include "dimensions.h"
#include "peripherals.h"
#include "pads.h"
#include "ball.h"
#include "text.h"
#include "font_types.h"
#include "menu.h"

enum gameState{
	READY = 1,
	RUNNING,
	RESULT,
	MENU
};

int state = MENU;

struct timespec loopDelay = {.tv_sec = 0, .tv_nsec = 20 * 1000 * 1000};
// Game mode
pads_t pads = {.p1Pos = SCREEN_HEIGHT / 2 - PAD_HEIGHT / 2, .p2Pos = SCREEN_HEIGHT / 2 - PAD_HEIGHT / 2, .p1Vel = 1, .p2Vel = -1};
ball_t ball = {.x = START_POS_X, .y = START_POS_Y, .xVel = 1, .yVel = 1};
_Bool stateSwitch = true;

int scale;

void setup(){
	//Screen data init
	fb = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(unsigned short));
  	if(fb == NULL){
		exit(-1);
	}

	//LCD screen setup
	parlcdMemBase = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	if(parlcdMemBase == NULL){
		exit(-1);
	}
	//parlcd_hx8357_init(parlcdMemBase);

	//Clear the screen
	parlcd_write_cmd(parlcdMemBase, 0x2c);
	int ptr = 0;
	unsigned int c;
  	for (int i = 0; i < SCREEN_HEIGHT; i++){
    	for (int j = 0; j < SCREEN_WIDTH; j++){
			c = 0;
      		fb[ptr] = c;
      		parlcd_write_data(parlcdMemBase, fb[ptr++]);
    	}
  	}
	//Loop timer setup
	loopDelay.tv_sec = 0;
  	loopDelay.tv_nsec = 17 * 1000 * 1000;

	//Font is added
	fdes = &font_winFreeSystem14x16;
	scale = 10;

	// Lights init
	uint8_t *spiled_mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	if(spiled_mem_base == NULL) {
		fprintf(stderr, "Peripheral init failed!");
		exit(1);
	}

	// Peripheral init
	led_line = (volatile uint32_t *) (spiled_mem_base + SPILED_REG_LED_LINE_o);
	rgb_led1 = (volatile uint32_t *) (spiled_mem_base + SPILED_REG_LED_RGB1_o);
	rgb_led2 = (volatile uint32_t *) (spiled_mem_base + SPILED_REG_LED_RGB2_o);
	knobs = (volatile uint32_t *) (spiled_mem_base + SPILED_REG_KNOBS_8BIT_o);

	//Other init
	menuInit();
}

void render(int* state){
	clearScreen();
	if(*state == RUNNING){
		if(stateSwitch){
			stateSwitch = true;
			renderCentralLine();
		}
		renderBall(&ball);
		renderPads(&pads);
		renderText(state);
		//if(score == max_score){ state = RESULT; }
	}
	else if(*state == READY){
		//resetBall(&ball);
		//renderText(state);
	}
	else if(*state == RESULT){
		//resetBall(&ball);
		//renderText(state);
	}
	else if(*state == MENU){
		renderMenu();
		//printf("Done!\n");
	}
	renderScreenData(parlcdMemBase);
}

void update(int *state){
	if(*state == RUNNING){
		int p1Offset = getPlayerOffset(1);
		int p2Offset = getPlayerOffset(2);
		updatePads(&pads, p1Offset, p2Offset);
		updateBall(&ball, &pads);
	}
	else if(*state == READY){
		// Get information from knobs to start 
		//WIP
		*state = RUNNING;
	}
	else if(*state == RESULT){
		// Score screen, win_sound, led and rgb animation 
		//WIP
		*state = READY;
	}
	else if(*state == MENU){
		updateMenu();
		if(menu.state == STARTED){
			*state = RUNNING;
		}
	}
}

int main(int argc, char *argv[]){
	setup();
	printf("Welcome to Pong!\n");
	while(true){
		// Main program loop
		update(&state);
		printf("State in main: %i\n", state);
		render(&state);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &loopDelay, NULL);
		printf("%X\n", *knobs);
	}

	printf("See you later!\n");
	return 0;
}
