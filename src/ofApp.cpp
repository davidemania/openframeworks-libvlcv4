//************************************************************************************
//************************************************************************************

#include "ofApp.h"

//************************************************************************************

void ofApp::setup()
{
    player = new VlcVideoPlayer();
    string filename = ofFilePath::getAbsolutePath("test.mp4");
    filename = "file://" + filename;
    player->playMedia(filename.c_str());

    updated = false;
}

//************************************************************************************

void ofApp::update()
{

}

//************************************************************************************

void ofApp::draw()
{
        // Get the current video texture and bind it
        GLuint tex = player->getVideoFrame(&updated);
        glBindTexture(GL_TEXTURE_2D, tex);

        // How to draw this texture on screen (or on an ofFbo?)
        // .........
        // tex is created like this
        //
        // glBindTexture(GL_TEXTURE_2D, that->textures[i]);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//************************************************************************************

void ofApp::keyPressed(int key)
{

}

//************************************************************************************
//************************************************************************************
