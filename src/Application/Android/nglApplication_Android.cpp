/*
  NGL - C++ cross-platform framework for OpenGL based applications
  Copyright (C) 2000-2003 NGL Team

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "nui.h"
#include "nglApplication.h"
#include "nglTimer.h"
#include "nglWindow.h"
#include <math.h>


using namespace std;


// #define NGL_APP_ENONE  0 (in nglApplication.h)
// Other defines in ./ngl_unix.h

#define NGL_APP_EXOPEN   1

const nglChar* gpApplicationErrorTable[] =
{
/*  0 */ _T("No error"),
/*  1 */ _T("Couldn't open X display"),
  NULL
};


/*
 * Life cycle
 */

nglApplication::nglApplication()
{
  mExitReq = false;
  mExitCode = 0;
  mUseIdle = false;
  mLastIdleCall = 0.0f;

  // nglApplication is a kernel's client, just as plugin instances
  IncRef();
}

nglApplication::~nglApplication()
{
}

void nglApplication::Quit(int Code)
{
  mExitReq = true;
  mExitCode = Code;
}

// Application Callbacks:

int32_t nglApplication::engine_handle_input(struct android_app* app, AInputEvent* event) 
{
  nglApplication* engine = (nglApplication*)app->userData;
  return engine->HandleInput(app, event);
}

int32_t nglApplication::HandleInput(struct android_app* app, AInputEvent* event) 
{
  if (mpWindow)
    return mpWindow->OnHandleInput(app, event);
  return 0;
}

/**
  * Process the next main command.
  */
void nglApplication::engine_handle_cmd(struct android_app* app, int32_t cmd) 
{
  nglApplication* engine = (nglApplication*)app->userData;
  engine->HandleCmd(app, cmd);
}

void nglApplication::HandleCmd(struct android_app* app, int32_t cmd) 
{
  switch (cmd)
  {
    // case APP_CMD_SAVE_STATE:
    //   // The system has asked us to save our current state.  Do so.
    //   mpAndroidApp->savedState = malloc(sizeof(struct saved_state));
    //   *((struct saved_state*)mpAndroidApp->savedState) = engine->state;
    //   mpAndroidApp->savedStateSize = sizeof(struct saved_state);
    //   break;
    //#FIXME
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      if (mpAndroidApp->window != NULL) 
      {

        if (mpWindow)
        {
          if (!mpWindow->OnSysInit(app))
          {
            NGL_LOG("window", NGL_LOG_ERROR, "Error while creating nglWindow");
          }
          mWindowInited = true;
          LOGI("System window init ok");
        }

              LOGI("System window init done");
      }
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      mpWindow->OnTermDisplay();
      break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
      // if (engine->accelerometerSensor != NULL) 
      // {
      //   ASensorEventQueue_enableSensor(engine->sensorEventQueue,
      //                                  engine->accelerometerSensor);
      //   // We'd like to get 60 events per second (in us).
      //   ASensorEventQueue_setEventRate(engine->sensorEventQueue,
      //                                  engine->accelerometerSensor, (1000L/60)*1000);
      // }
      break;
    case APP_CMD_LOST_FOCUS:
      // When our app loses focus, we stop monitoring the accelerometer.
      // This is to avoid consuming battery while not being used.
      // if (engine->accelerometerSensor != NULL) 
      // {
      //   ASensorEventQueue_disableSensor(engine->sensorEventQueue,
      //                                   engine->accelerometerSensor);
      // }
      // // Also stop animating.
      // engine->animating = 1;
      // engine_draw_frame(engine);
      break;
    case APP_CMD_WINDOW_RESIZED:
      {
        NGL_OUT("Android window resized");
        if (mpWindow)
          mpWindow->OnUpdateConfig(app);
      }
      break;

    case APP_CMD_CONFIG_CHANGED:
      {
        NGL_OUT("Android app config changed");
        if (mpWindow)
          mpWindow->OnUpdateConfig(app);
      }
      break;
  
  }
}



/*
 * Application entry point
 */

int nglApplication::android_main(struct android_app* state)
{
  SysInit(state);
  //#TODO: WTF should we do with saved states?
  // if (mpAndroidState->savedState != NULL) 
  // {
  //   // We are starting with a previous saved state; restore from it.
  //   mpAndroidState = *(struct saved_state*)mpAndroidState->savedState;
  // }

  //ANativeActivity_setWindowFlags(mpAndroidState->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN);
  ANativeActivity_setWindowFlags(GetAndroidApp()->activity, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
  ANativeActivity_setWindowFlags(GetAndroidApp()->activity, AWINDOW_FLAG_LAYOUT_INSET_DECOR, 0);
  ANativeActivity_setWindowFlags(GetAndroidApp()->activity, AWINDOW_FLAG_LAYOUT_IN_SCREEN, 0);

  //GetAndroidApp()->activity->callbacks->onContentRectChanged = onContentRectChanged;


  GetAndroidApp()->userData = this;
  GetAndroidApp()->onAppCmd = nglApplication::engine_handle_cmd;
  GetAndroidApp()->onInputEvent = nglApplication::engine_handle_input;
    
  CallOnInit();           // Call user OnInit() call back
  SysLoop();              // Run event pump till application ending
  CallOnExit(mExitCode);  // Call user OnExit() call back

  Exit(mExitCode);
  return mExitCode;
}


/*
 * Event management
 */

// void nglApplication::AddTimer (nglTimer* pTimer)
// {
//   mTimers.push_front (pTimer);
// }

// void nglApplication::DelTimer (nglTimer* pTimer)
// {
//   mTimers.remove (pTimer);
// }

#define DBG_EVENT(x)

int nglApplication::SysLoop()
{  
  bool firstwindowinit = false;
  while (1) 
  {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;
    
    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) 
    {
      
      // Process this event.
      if (source != NULL) 
      {
        source->process(mpAndroidApp, source);
      }

      // Check if we are exiting.
      if (mpAndroidApp->destroyRequested != 0) 
      {
        CallOnWillExit();
        CallOnExit(mExitCode);
        return mExitCode;
      }
    }
    
    nglTimer::DispatchTimers();
    
    // Drawing is throttled to the screen update rate, so there
    // is no need to do timing here.
    if (mpWindow)
    {
      if (!firstwindowinit)
      {
        mpWindow->CallOnCreation();
        mpWindow->CallOnPaint();
        firstwindowinit = true;
      }

      mpWindow->CallOnPaint();
    }
  }

  return mExitCode;
}

