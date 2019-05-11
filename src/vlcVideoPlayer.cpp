//************************************************************************************
//************************************************************************************

#include "vlcVideoPlayer.h"

#include "GLFW/glfw3.h"

extern shared_ptr<ofAppBaseWindow> mainWindow;

//************************************************************************************

VlcVideoPlayer::VlcVideoPlayer()
{
    ofLogNotice() << "Creating VLC context";

    const char *args[] = { "--verbose=4" };

    vlc = libvlc_new(sizeof(args) / sizeof(*args), args);

    width = 0;
    height = 0;
    
    renderIndex = 0;
    swapIndex = 1;
    displayIndex = 2;
    
    updated = false;
    
    ofGLFWWindowSettings settings;
    settings.setSize(640, 480); // will only use this window for its GL context
    settings.visible = false;
    settings.shareContextWith = mainWindow;
    vlcWindow = ofCreateWindow(settings);

    // VLC opengl context needs to be shared with SDL context
    // SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    // m_ctx = SDL_GL_CreateContext(window);
}

//************************************************************************************
//************************************************************************************

VlcVideoPlayer::~VlcVideoPlayer()
{
    stop();

    if(vlc)
    {
        libvlc_release(vlc);
    }
}

//************************************************************************************
//************************************************************************************

bool VlcVideoPlayer::playMedia(const char* url)
{
    media = libvlc_media_new_location (vlc, url);
    if(!media)
    {
        ofLogError() << "unable to create media %s " << url;
        return false;
    }

    mediaPlayer = libvlc_media_player_new_from_media (media);
    if(!mediaPlayer)
    {
        ofLogError() << "unable to create media player";
        libvlc_media_release(media);
        return false;
    }

    // Define the opengl rendering callbacks
    libvlc_video_set_output_callbacks(mediaPlayer,
                                      libvlc_video_engine_opengl,
                                      setup,
                                      cleanup,
                                      resize,
                                      swap,
                                      make_current,
                                      get_proc_address,
                                      this);

    // Play the video
    libvlc_media_player_play (mediaPlayer);

    return true;
}

//************************************************************************************
//************************************************************************************

void VlcVideoPlayer::stop()
{
    if(mediaPlayer)
    {
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }

    if(media)
    {
        libvlc_media_release(media);
        media = nullptr;
    }
}

//************************************************************************************
// return the texture to be displayed
//************************************************************************************

GLuint VlcVideoPlayer::getVideoFrame(bool* out_updated)
{
    std::lock_guard<std::mutex> lock(textureLock);
    
    if(out_updated)
    {
        *out_updated = updated;
    }

    if(updated)
    {
        std::swap(swapIndex, displayIndex);
        updated = false;
    }

    return textures[displayIndex];
}

//************************************************************************************
// this callback will create the surfaces and FBO used by VLC to perform its rendering
// called by libvlc when video size changes (upon new file load) [ static ]
//************************************************************************************

void VlcVideoPlayer::resize(void* data, unsigned width, unsigned height)
{
    VlcVideoPlayer* that = static_cast<VlcVideoPlayer*>(data);

    if(width != that->width || height != that->height)
    {
        cleanup(data);
    }

    glGenTextures(3, that->textures);
    glGenFramebuffers(3, that->fbos);

    for (int i = 0; i < 3; i++)
    {
        // initialize textures 0 to 2
        glBindTexture(GL_TEXTURE_2D, that->textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // and framebuffers
        glBindFramebuffer(GL_FRAMEBUFFER, that->fbos[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, that->textures[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        return;
    }

    that->width = width;
    that->height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, that->fbos[that->renderIndex]);
}

//************************************************************************************
// This callback is called during initialisation.
// [ static ]
//************************************************************************************

bool VlcVideoPlayer::setup(void* data)
{
    VlcVideoPlayer* that = static_cast<VlcVideoPlayer*>(data);

    that->width = 0;
    that->height = 0;

    return true;
}

//************************************************************************************
// This callback is called to release the texture and FBO created in resize
// [ static ]
//************************************************************************************
 void VlcVideoPlayer::cleanup(void* data)
{
    VlcVideoPlayer* that = static_cast<VlcVideoPlayer*>(data);

    if(that->width == 0 && that->height == 0)
    {
        return;
    }

    glDeleteTextures(3, that->textures);
    glDeleteFramebuffers(3, that->fbos);
}

//************************************************************************************
// This callback is called after VLC performs drawing calls
// [ static ]
//************************************************************************************

void VlcVideoPlayer::swap(void* data)
{
    VlcVideoPlayer* that = static_cast<VlcVideoPlayer*>(data);

    std::lock_guard<std::mutex> lock(that->textureLock);
    that->updated = true;
    std::swap(that->swapIndex, that->renderIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, that->fbos[that->renderIndex]);
}

//************************************************************************************
// This callback is called to set the OpenGL context
// [ static ]
//************************************************************************************

bool VlcVideoPlayer::make_current(void* data, bool enter)
{
    VlcVideoPlayer* that = static_cast<VlcVideoPlayer*>(data);

    if(enter)
    {
        // return SDL_GL_MakeCurrent(that->m_win, that->m_ctx) == 0;
        // return 0; //glXMakeCurrent(that->m_win, that->m_ctx) == 0;
        ofAppGLFWWindow* win = dynamic_cast<ofAppGLFWWindow*>(that->vlcWindow.get());
        glfwMakeContextCurrent(win->getGLFWWindow());
        return true;
    }
    else
    {
        ofAppGLFWWindow* win = dynamic_cast<ofAppGLFWWindow*>(mainWindow.get());
        glfwMakeContextCurrent(win->getGLFWWindow());
        return 0; //glXMakeCurrent(that->m_win, 0) == 0;
    }
}

//************************************************************************************
// This callback is called by VLC to get OpenGL functions.
// [ static ]
//************************************************************************************

void* VlcVideoPlayer::get_proc_address(void* /*data*/, const char* current)
{
    // return SDL_GL_GetProcAddress(current);
    return (void*)glfwGetProcAddress(current);
}

//************************************************************************************
//************************************************************************************
