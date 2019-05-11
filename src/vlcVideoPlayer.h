//************************************************************************************
//************************************************************************************

#pragma once

#include "ofMain.h"

#include "libvlcv4/vlc.h"

//************************************************************************************

class VlcVideoPlayer
{
    public:
        VlcVideoPlayer();
        ~VlcVideoPlayer();

        bool playMedia(const char* url);
        void stop();
        GLuint getVideoFrame(bool* out_updated);
        static void resize(void* data, unsigned width, unsigned height);
        static bool setup(void* data);
        static void cleanup(void* data);
        static void swap(void* data);
        static bool make_current(void* data, bool enter);
        static void* get_proc_address(void* data, const char* current);

    private:
        //VLC objects
        libvlc_instance_t* vlc = nullptr;
        libvlc_media_player_t* mediaPlayer = nullptr;
        libvlc_media_t* media = nullptr;

        shared_ptr<ofAppBaseWindow> vlcWindow;
        
        //FBO data
        unsigned int width;
        unsigned int height;

        std::mutex textureLock;

        GLuint textures[3];
        GLuint fbos[3];

        size_t renderIndex;
        size_t swapIndex;
        size_t displayIndex;

        bool updated;
};

//************************************************************************************
//************************************************************************************
