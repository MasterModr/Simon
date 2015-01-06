#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "gfx.h"
#include "draw.h"

#include "normal_bin.h"
#include "red_bin.h"
#include "yellow_bin.h"
#include "green_bin.h"
#include "blue_bin.h"
#include "sred_bin.h"
#include "syellow_bin.h"
#include "sgreen_bin.h"
#include "sblue_bin.h"
#include "correct_bin.h"
#include "wrong_bin.h"



int main()
{
  
	
  // Initializations
  srvInit();        // services
  aptInit();        // applets
  hidInit(NULL);    // input
  gfxInit();        // graphics
  gfxSet3D(false);  // stereoscopy (true == on / false == off)
  CSND_initialize(NULL);
  u32 kDown;        // keys down
  u32 kHeld;        // keys pressed
  u32 kUp;          // keys up
  u8* fbTopLeft;    // top left screen's framebuffer
  u8* fbTopRight;   // top right screen's framebuffer
  u8* fbBottom;     // bottom screen's framebuffer
  
  u8 *sred = linearAlloc(sred_bin_size);
  u8 *syellow = linearAlloc(syellow_bin_size);
  u8 *sgreen = linearAlloc(sgreen_bin_size);
  u8 *sblue = linearAlloc(sblue_bin_size);  
  
  memcpy(sred, sred_bin, sred_bin_size);
  memcpy(syellow, syellow_bin, syellow_bin_size);
  memcpy(sgreen, sgreen_bin, sgreen_bin_size);
  memcpy(sblue, sblue_bin, sblue_bin_size);
  
  
  int Level = 0; //What level are we on
  int HighScore = 1; //Current High Score
  int Touching = 0; //Is player touching screen
  int Playing = 0; //Is sound playing
  int ScreenState = 0; //What screen image should be displayed
  int arraypos = 0; //Position holder for reading through array
  int array[1000]; //random pattern array
  int checkpattern[1000]; //pattern to check against for inputs
  int inputs[1000]; //holds new inputs
  int ai = 1; //is the ai running
  int resetpattern = 1; //reset random pattern
  int max = 5; //maximum number for random + 1
  int min = 1; //minimum number for random
  int waittime = 0; //how long should we wait before running next function
  int checkpos = 0; //position holder for check array
  int checkpat = 0; //should we check the pattern
  int newai = 0; //should a new game be cued.
  int apress = 0; //was a pressed
  
  // Main loop
  while (aptMainLoop())
  {
	  
	//resets random pattern when activated
	if(resetpattern == 1)
	{
		u32 counter = 0;
		while(counter < Level)
		{
			array[counter] = (rand()%(max-min) + min);
			counter++;
		}
		array[counter] = 9;
		resetpattern = 0;
	}
	
	//check current input against pattern
	if(checkpat == 1)
	{
		if(checkpattern[checkpos] == inputs[checkpos]){
			checkpos++;
			if(checkpattern[checkpos] == 9)
			{
				ScreenState = 5;
				newai = 1;
				waittime = 120;
				checkpos = 0;
				if(Level > HighScore){
				HighScore = Level;
				}
			}
			checkpat = 0;
		}
		else{
			
			if(apress == 0){
				ScreenState = 6;
			}
			else{
				apress = 0;
			}
			newai = 1;
			Level = 1;
			resetpattern = 1;
			waittime = 120;
			checkpat = 0;
			checkpos = 0;
		}
	}
	
	//runs if a new game is triggered after waiting
	if(newai == 1 && waittime == 0)
	{
		newai = 0;
		ai = 1;
	}

	if(newai == 1 && waittime < 60)
	{
		Playing = 0;
	    CSND_setchannel_playbackstate(0x8, 0);
	    CSND_sharedmemtype0_cmdupdatestate(0);
	}


    // Wait for next frame
    gspWaitForVBlank();

    // Read which buttons are currently pressed or not
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();

    // If START button is pressed, break loop and quit
    if (kDown & KEY_START){
      break;
    }
	
	if (kDown & KEY_A){
		inputs[checkpos] = 5;
		checkpat = 1;
		apress = 1;
	}
    
    // Reset framebuffers
    fbTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    fbTopRight = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
    fbBottom = gfxGetFramebuffer(GFX_BOTTOM, 0, NULL, NULL);
    memset(fbTopLeft, 0, 240 * 400 * 3);
    memset(fbTopRight, 0, 240 * 400 * 3);
    memset(fbBottom, 0, 240 * 320 * 3);


    /** Your code starts here **/
    //get touch position
	touchPosition myTouchPosition;
    hidTouchRead(&myTouchPosition);
    u16 posX = myTouchPosition.px;
    u16 posY = myTouchPosition.py;
	
	//setup framebuffers
	u8* ft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
    u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_BOTTOM, NULL, NULL);
	memset(ft, 0, 240*400*3);
    memset(fb, 0, 240*320*3);
	
	//setup top menu text
	char a[100];
	sprintf(a,"Simon 1.0");
	drawString(a, 0, 0, 255, 255, 255, ft, GFX_LEFT);
	sprintf(a,"By: Mastermodr94");
	drawString(a, 0, 8, 255, 255, 255, ft, GFX_LEFT);
	sprintf(a,"Press A for New Game");
	drawString(a, 0, 24, 255, 255, 255, ft, GFX_LEFT);
	sprintf(a,"Current High Score: %u", HighScore -1);
	drawString(a, 0, 40, 255, 255, 255, ft, GFX_LEFT);
	
	
	
	if((posX >= 1 && posY >= 1) && (posX <= 320 && posY <= 240)){
    Touching = 1;
    }else{
    Touching = 0;
    }
	
	if(Touching == 1 && Playing == 0  && waittime == 0){
		if(((posX > 70) && (posX < 150) )&& ((posY > 25) && (posY < 110)))
		{
			ScreenState = 1;
			Playing = 1;
			CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)syellow, NULL, syellow_bin_size, 2, 0);
			inputs[checkpos] = 1;
			checkpat = 1;
		}
		else {
		if(((posX > 160) && (posX < 250) )&& ((posY > 25) && (posY < 110)))
		{
			ScreenState = 2;
			Playing = 1;
			CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sred, NULL, sred_bin_size, 2, 0);
			inputs[checkpos] = 2;
			checkpat = 1;
		}
		else {
		if(((posX > 70) && (posX < 150) )&& ((posY > 125) && (posY < 215)))
		{
			ScreenState = 3;
			Playing = 1;
			CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sblue, NULL, sblue_bin_size, 2, 0);
			inputs[checkpos] = 3;
			checkpat = 1;
		}
		else {
		if(((posX > 160) && (posX < 250) )&& ((posY > 125) && (posY < 215)))
		{
			ScreenState = 4;
			Playing = 1;
			CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sgreen, NULL, sgreen_bin_size, 2, 0);
			inputs[checkpos] = 4;
			checkpat = 1;
		}else {
			ScreenState = 0;
		}}}}
	}
	if(Touching == 0  && waittime == 0)
	{
		ScreenState = 0;
	}
	if(Playing == 1 && Touching == 0 && waittime < 10){
	    Playing = 0;
	    CSND_setchannel_playbackstate(0x8, 0);
	    CSND_sharedmemtype0_cmdupdatestate(0);
	}
	
	if(ai == 1 && waittime == 0)
	{
		int randnumber = array[arraypos];
		arraypos++;
		if(randnumber != 9){
			ScreenState = (randnumber);\
			Playing = 1;
			if(randnumber == 1){
				CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)syellow, NULL, syellow_bin_size, 2, 0);
			}
			if(randnumber == 2){
			    CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sred, NULL, sred_bin_size, 2, 0);
			}
			if(randnumber == 3){
				CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sblue, NULL, sblue_bin_size, 2, 0);
			}
			if(randnumber == 4){
				CSND_playsound(0x8, CSND_LOOP_ENABLE, CSND_ENCODING_PCM16, 44100, (u32*)sgreen, NULL, sgreen_bin_size, 2, 0);
			}
			waittime = 60;
		}
		else{
			
			int check = 0;
			while(check <= arraypos)
			{
				checkpattern[check] = array[check];
				check++;
			}
			arraypos = 0;
			Level += 1;
			resetpattern = 1;
			ai = 0;
		}
	}
	
	
	
	
	
	//display proper screen image
	switch(ScreenState)
	{
		case 0: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)normal_bin, 240, 320, 0, 0);
			break;
		}
		case 1: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)yellow_bin, 240, 320, 0, 0);
			break;
		}
		case 2: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)red_bin, 240, 320, 0, 0);
			break;
		}
		case 3: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)blue_bin, 240, 320, 0, 0);
			break;
		}
		case 4: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)green_bin, 240, 320, 0, 0);
			break;
		}
		case 5: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)correct_bin, 240, 320, 0, 0);
			break;
		}
		case 6: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)wrong_bin, 240, 320, 0, 0);
			break;
		}
		default: {
			gfxDrawSprite(GFX_BOTTOM, GFX_LEFT, (u8*)normal_bin, 240, 320, 0, 0);
			break;
		}
	}
	
	//if waiting then decrease waiting by 1
	if(waittime > 0)
	{
		waittime -= 1;
	}

    /** End of your code **/
	
    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxSwapBuffers();
  }

  // Exit
  CSND_shutdown();
  gfxExit();
  hidExit();
  aptExit();
  srvExit();

  // Return to hbmenu
  return 0;
}
