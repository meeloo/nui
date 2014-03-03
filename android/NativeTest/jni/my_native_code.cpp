#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/window.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>


#include "nui.h"
#include "nuiInit.h"

#include "nuiAndroidBridge.h"
#include "nuiNativeResource.h"

nuiAndroidBridge* gpBridge = NULL;

/**
  * Our saved state data.
  */
struct saved_state 
{
  float angle;
  int32_t x;
  int32_t y;
};

/**
  * Shared state for our app.
  */
struct engine
{
  struct android_app* app;
  
  ASensorManager* sensorManager;
  const ASensor* accelerometerSensor;
  ASensorEventQueue* sensorEventQueue;
  
  int animating;
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  int32_t width;
  int32_t height;
  struct saved_state state;
};

/**
  * Initialize an EGL context for the current display.
  */
static int engine_init_display(struct engine* engine) 
{
  LOGI("Init opengl context");
  // initialize OpenGL ES and EGL
  
  /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
  const EGLint attribs[] = 
  {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    //EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint w, h, dummy, format;
  EGLint numConfigs;
  EGLConfig configs[10];
  EGLSurface surface;
  EGLContext context;
  
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  
  eglInitialize(display, 0, 0);
  
  /* Here, the application chooses the configuration it desires. In this
    * sample, we have a very simplified selection process, where we pick
    * the first EGLConfig that matches our criteria */
  eglChooseConfig(display, attribs, configs, 10, &numConfigs);
  LOGI("Init opengl num configs: %d", numConfigs);
  
  /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
  eglGetConfigAttrib(display, configs[0], EGL_NATIVE_VISUAL_ID, &format);
  LOGI("Init opengl config format: %d", format);
  
  ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);
  
  surface = eglCreateWindowSurface(display, configs[0], engine->app->window, NULL);
  LOGI("Init opengl surface: %p", surface);

  EGLint attribs2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
  context = eglCreateContext(display, configs[0], NULL, attribs2);
  LOGI("Init opengl context: %p", context);
  
  if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) 
  {
    LOGI("Unable to eglMakeCurrent");
    return -1;
  }
  
  eglQuerySurface(display, surface, EGL_WIDTH, &w);
  eglQuerySurface(display, surface, EGL_HEIGHT, &h);
  
  engine->display = display;
  engine->context = context;
  engine->surface = surface;
  engine->width = w;
  engine->height = h;
  engine->state.angle = 0;
  
  nuiCheckForGLErrorsReal();
  // Initialize GL state.
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  nuiCheckForGLErrorsReal();
  glEnable(GL_CULL_FACE);
  nuiCheckForGLErrorsReal();
  glShadeModel(GL_SMOOTH);
  nuiCheckForGLErrorsReal();
  glDisable(GL_DEPTH_TEST);
  nuiCheckForGLErrorsReal();
  
  // Create the NUI bridge which also serves as the main window/widget tree:
  gpBridge->Init();
  nuiCheckForGLErrorsReal();
  

  return 0;
}

/**
  * Just the current frame in the display.
  */
static void engine_draw_frame(struct engine* engine) 
{ 
  if (engine->display == NULL) 
  {
    // No display.
    return;
  }
  
  // Just fill the screen with a color.
  nuiCheckForGLErrorsReal();
  glClearColor(((float)engine->state.x)/engine->width, engine->state.angle, ((float)engine->state.y)/engine->height, 1);
  nuiCheckForGLErrorsReal();
  glClearColor(0, 0, 0, 0);
  nuiCheckForGLErrorsReal();
  glClear(GL_COLOR_BUFFER_BIT);
  nuiCheckForGLErrorsReal();
  
  gpBridge->Display();
  
  eglSwapBuffers(engine->display, engine->surface);
}

/**
  * Tear down the EGL context currently associated with the display.
  */
static void engine_term_display(struct engine* engine) 
{
  if (engine->display != EGL_NO_DISPLAY) 
  {
    eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (engine->context != EGL_NO_CONTEXT) 
    {
      eglDestroyContext(engine->display, engine->context);
    }
    if (engine->surface != EGL_NO_SURFACE) 
    {
      eglDestroySurface(engine->display, engine->surface);
    }
    eglTerminate(engine->display);
  }
  engine->animating = 0;
  engine->display = EGL_NO_DISPLAY;
  engine->context = EGL_NO_CONTEXT;
  engine->surface = EGL_NO_SURFACE;
  
  
  
  // Exit the application
  // First destroy the NUI bridge / widget tree:
  LOGI("delete android bridge");
  gpBridge->Release();
  LOGI("delete android bridge OK");
  
  // Shutdown the basic NUI services:
  LOGI("nuiUninit");
  nuiUninit();
  LOGI("nuiUninit OK");
}

float RemapCoords(struct android_app* app, int& x, int& y)
{
  float density = AConfiguration_getDensity(app->config);
  float scale = ToNearest(density / 160);
  //NGL_OUT("Remap %d x %d", x, y);
  x /= scale;
  y /= scale;
  //NGL_OUT("To %d x %d (%f)", x, y, scale);
  return scale;
}


/**
  * Process the next input event.
  */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) 
{
  struct engine* engine = (struct engine*)app->userData;
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) 
  {
    int32_t id = AInputEvent_getDeviceId(event);
    int32_t src = AInputEvent_getSource(event);
    int32_t flags = AMotionEvent_getFlags(event);
    size_t count = AMotionEvent_getPointerCount(event);

    for (int i = 0; i < count; i++)
    {
      int PointerId  = AMotionEvent_getPointerId( event, i );
      int x = AMotionEvent_getX(event, i);
      int y = AMotionEvent_getY(event, i);
      RemapCoords(app, x, y);

      NGL_OUT("Event[%d] deviceid = %d  id = %d src = %d  flags = %d  count = %d (%d x %d)", i, id, PointerId, src, flags, (int)count, x, y);

      int action = AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction( event );
      if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN)
      {
        nuiAndroidBridge::androidMouse(PointerId, 0, 0, x, y);
      }
      else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP)
      {
        nuiAndroidBridge::androidMouse(PointerId, 0, 1, x, y);      
      }
      else if (action == AMOTION_EVENT_ACTION_MOVE || action == AMOTION_EVENT_ACTION_POINTER_UP)
      {
        nuiAndroidBridge::androidMotion(PointerId, x, y);
      }
    }
    return 1;
  }
  return 0;
}

int MyGetStatusBarSize(int density)
{
  switch ( density )
   {
  case 160:
      return 25;
      break;
  case 120:
      return 19;
      break;
  case 240:
      return 38;
      break;
  case 320:
      return 50;
      break;
  default:
      return 25;
      break;
  }

  return -1;
}

void UpdateConfig(struct android_app* app)
{
  int w = 0;
  int h = 0;
  int s1 = ANativeWindow_getWidth(app->window);
  int s2 = ANativeWindow_getHeight(app->window);

  int32_t orientation = AConfiguration_getOrientation(app->config);
  switch(orientation)
  {
  case ACONFIGURATION_ORIENTATION_LAND:
    NGL_OUT("Orientation changed to Landscape");
    w = MAX(s1, s2);
    h = MIN(s1, s2);
    break;
  case ACONFIGURATION_ORIENTATION_PORT:
    NGL_OUT("Orientation changed to Portrait");
    w = MIN(s1, s2);
    h = MAX(s1, s2);
    break;
  case ACONFIGURATION_ORIENTATION_SQUARE:
    NGL_OUT("Orientation changed to Square (WTF?)");
    w = MAX(s1, s2);
    h = MIN(s1, s2);
    break;
  case ACONFIGURATION_ORIENTATION_ANY:
    NGL_ASSERT(false);
    break;
  default:
    NGL_ASSERT(false);
    break;
  }
  float density = AConfiguration_getDensity(app->config);
  float scale = ToNearest(density / 160);
  w /= scale;
  h /= scale;
  nuiAndroidBridge::androidRescale(scale);
  nuiAndroidBridge::androidResize(w, h);
  nuiAndroidBridge::androidSetStatusBarSize(MyGetStatusBarSize(density));
}


/**
  * Process the next main command.
  */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) 
{
  struct engine* engine = (struct engine*)app->userData;
  switch (cmd)
  {
    case APP_CMD_SAVE_STATE:
      // The system has asked us to save our current state.  Do so.
      engine->app->savedState = malloc(sizeof(struct saved_state));
      *((struct saved_state*)engine->app->savedState) = engine->state;
      engine->app->savedStateSize = sizeof(struct saved_state);
      break;
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      if (engine->app->window != NULL) 
      {
        nuiCheckForGLErrorsReal();
        engine_init_display(engine);
        nuiCheckForGLErrorsReal();

        UpdateConfig(app);

        LOGI("Init window");

        nuiGrid* pGrid = new nuiGrid(3, 6);
        gpBridge->AddChild(pGrid);
        for (int i = 0; i < 3; i++)
        {
          pGrid->SetColumnExpand(i, nuiExpandShrinkAndGrow);
          for (int j = 0; j < 6; j++)
          {
            if (!i)
              pGrid->SetRowExpand(j, nuiExpandShrinkAndGrow);

            nuiButton* pButton = new nuiButton();
            nuiImage* pImage = new nuiImage("rsrc://Logo.png");
            pImage->SetFillRule(nuiCenter);
            pImage->SetPosition(nuiFill);
            pImage->SetFixedAspectRatio(true);
            pButton->AddChild(pImage);
            pButton->SetUserSize(150, 150);
            //pButton->SetPosition(nuiFill);
            pGrid->SetCell(i, j , pButton);
          }
        }

        nuiLabel* pLabel = new nuiLabel("Prout!", nuiFont::GetFont(16));
        pLabel->SetTextColor("white");
        gpBridge->AddChild(pLabel);

        engine_draw_frame(engine);
        engine->animating = 1;
      }
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      engine_term_display(engine);
      break;
    case APP_CMD_GAINED_FOCUS:
      // When our app gains focus, we start monitoring the accelerometer.
      if (engine->accelerometerSensor != NULL) 
      {
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                                       engine->accelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                                       engine->accelerometerSensor, (1000L/60)*1000);
      }
      break;
    case APP_CMD_LOST_FOCUS:
      // When our app loses focus, we stop monitoring the accelerometer.
      // This is to avoid consuming battery while not being used.
      if (engine->accelerometerSensor != NULL) 
      {
        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                                        engine->accelerometerSensor);
      }
      // Also stop animating.
      engine->animating = 1;
      engine_draw_frame(engine);
      break;
    case APP_CMD_WINDOW_RESIZED:
      {
        NGL_OUT("Android window resized");
        UpdateConfig(app);
      }
      break;

    case APP_CMD_CONFIG_CHANGED:
      {
        NGL_OUT("Android app config changed");
        UpdateConfig(app);
      }
      break;
  
  }
}

void onContentRectChanged(ANativeActivity* activity, const ARect* rect)
{
  ARect r = *rect;
  NGL_OUT("Window rect %d %d %d %d", r.left, r.top, r.right - r.left, r.bottom - r.top);
}


/**
  * This is the main entry point of a native application that is using
  * android_native_app_glue.  It runs in its own thread, with its own
  * event loop for receiving input events and doing other things.
  */
void android_main(struct android_app* state) 
{ 
  LOGI("nuiInit");
  nuiInit(state);
  LOGI("nuiInit OK");
  
  struct engine engine;
  
  // Make sure glue isn't stripped.
  app_dummy();
  
  //ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN);
  ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN, AWINDOW_FLAG_FULLSCREEN);
  ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_LAYOUT_INSET_DECOR, 0);
  ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_LAYOUT_IN_SCREEN, 0);

  state->activity->callbacks->onContentRectChanged = onContentRectChanged;


  memset(&engine, 0, sizeof(engine));
  state->userData = &engine;
  state->onAppCmd = engine_handle_cmd;
  state->onInputEvent = engine_handle_input;
  engine.app = state;
  
  // Prepare to monitor accelerometer
  engine.sensorManager = ASensorManager_getInstance();
  engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
                                                               ASENSOR_TYPE_ACCELEROMETER);
  engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
                                                            state->looper, LOOPER_ID_USER, NULL, NULL);
  
  if (state->savedState != NULL) 
  {
    // We are starting with a previous saved state; restore from it.
    engine.state = *(struct saved_state*)state->savedState;
  }
  
    // Create the NUI bridge which also serves as the main window/widget tree:
  LOGI("create Android Bridge");
  gpBridge = new nuiAndroidBridge();
  gpBridge->Acquire();
  LOGI("create Android Bridge OK");
  
  // loop waiting for stuff to do.

  
  while (1) 
  {
    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;
    
    // If not animating, we will block forever waiting for events.
    // If animating, we loop until all events are read, then continue
    // to draw the next frame of animation.
    while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                                  (void**)&source)) >= 0) 
    {
      
      // Process this event.
      if (source != NULL) 
      {
        source->process(state, source);
      }
      
      // If a sensor has data, process it now.
      if (ident == LOOPER_ID_USER) 
      {
        if (engine.accelerometerSensor != NULL) 
        {
          ASensorEvent event;
          while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                                             &event, 1) > 0) 
          {
//            LOGI("accelerometer: x=%f y=%f z=%f",
//                 event.acceleration.x, event.acceleration.y,
//                 event.acceleration.z);
          }
        }
      }
      
      // Check if we are exiting.
      if (state->destroyRequested != 0) 
      {
        engine_term_display(&engine);
        return;
      }
    }
    
    if (engine.animating) 
    {
      // Done with events; draw next animation frame.
      engine.state.angle += .01f;
      if (engine.state.angle > 1) 
      {
        engine.state.angle = 0;
      }
      
      // Drawing is throttled to the screen update rate, so there
      // is no need to do timing here.
      engine_draw_frame(&engine);
    }
  }
}

