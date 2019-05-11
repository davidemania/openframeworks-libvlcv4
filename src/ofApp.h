//************************************************************************************
//************************************************************************************

#pragma once

#include "ofMain.h"
#include "vlcVideoPlayer.h"

//************************************************************************************

class ofApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);

    private:
        VlcVideoPlayer* player;

        bool updated;
};

//************************************************************************************
//************************************************************************************