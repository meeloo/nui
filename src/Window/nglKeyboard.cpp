/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */



#include "nui.h"
#define __nglKeyboard_cpp__
#undef __nglKeyboard_h__
#include "nglKeyboard.h"


/* Key name table
 */
const nglChar *gpKeyName[NGL_KEY_MAX + 1] =
{
  _T(""),             // NK_NONE
  "Esc",          // NK_ESC
  "F1",           // NK_F1
  "F2",           // NK_F2
  "F3",           // NK_F3
  "F4",           // NK_F4
  "F5",           // NK_F5
  "F6",           // NK_F6
  "F7",           // NK_F7
  "F8",           // NK_F8
  "F9",           // NK_F9
  "F10",          // NK_F10
  "F11",          // NK_F11
  "F12",          // NK_F12
  "SysReq",       // NK_SYSREQ
  "Scroll Lock",  // NK_SCRLOCK
  "Pause",        // NK_PAUSE
  "`",            // NK_GRAVE
  "1",            // NK_1
  "2",            // NK_2
  "3",            // NK_3
  "4",            // NK_4
  "5",            // NK_5
  "6",            // NK_6
  "7",            // NK_7
  "8",            // NK_8
  "9",            // NK_9
  "0",            // NK_0
  "-",            // NK_MINUS
  "=",            // NK_EQUAL
  "BackSpace",    // NK_BACKSPACE
  "Ins",          // NK_INSERT
  "Home",         // NK_HOME
  "Pg Up",        // NK_PAGEUP
  "Num Lock",     // NK_PAD_LOCK
  "Pad /",        // NK_PAD_SLASH
  "Pad *",        // NK_PAD_ASTERISK
  "Pad -",        // NK_PAD_MINUS
  "Tab",          // NK_TAB
  "Q",            // NK_Q
  "W",            // NK_W
  "E",            // NK_E
  "R",            // NK_R
  "T",            // NK_T
  "Y",            // NK_Y
  "U",            // NK_U
  "I",            // NK_I
  "O",            // NK_O
  "P",            // NK_P
  "[",            // NK_LBRACKET
  "]",            // NK_RBRACKET
  "Enter",        // NK_ENTER
  "Del",          // NK_DELETE
  "End",          // NK_END
  "Pg Down",      // NK_PAGEDOWN
  "Pad 7",        // NK_PAD_7
  "Pad 8",        // NK_PAD_8
  "Pad 9",        // NK_PAD_9
  "Pad +",        // NK_PAD_PLUS
  "Caps Lock",    // NK_CAPSLOCK
  "A",            // NK_A
  "S",            // NK_S
  "D",            // NK_D
  "F",            // NK_F
  "G",            // NK_G
  "H",            // NK_H
  "J",            // NK_J
  "K",            // NK_K
  "L",            // NK_L
  ";",            // NK_SEMICOLON
  "'",            // NK_APOSTROPHE
  "#",            // NK_NUMBERSIGN
  "Pad 4",        // NK_PAD_4
  "Pad 5",        // NK_PAD_5
  "Pad 6",        // NK_PAD_6
  "Left SHIFTKEY",      // NK_LSHIFT
  "\\",           // NK_BACKSLASH
  "Z",            // NK_Z
  "X",            // NK_X
  "C",            // NK_C
  "V",            // NK_V
  "B",            // NK_B
  "N",            // NK_N
  "M",            // NK_M
  "),",           // NK_COMMA
  ".",            // NK_PERIOD
  "/",            // NK_SLASH
  "Right SHIFTKEY",      // NK_RSHIFT
  "Up",           // NK_UP
  "Pad 1",        // NK_PAD_1
  "Pad 2",        // NK_PAD_2
  "Pad 3",        // NK_PAD_3
  "Pad Enter",    // NK_PAD_ENTER
  "Left CTRLKEY",       // NK_LCTRL
  "Left ALTKEY",        // NK_LALT
  "Space",        // NK_SPACE
  "Right ALTKEY",        // NK_RALT
  "Right CTRLKEY",       // NK_RCTRL
  "Left",         // NK_LEFT
  "Down",         // NK_DOWN
  "Right",        // NK_RIGHT
  "Pad 0",        // NK_PAD_0
  "Pad .",        // NK_PAD_PERIOD
#ifdef _CARBON_
  "Left CMDKEY",       // NK_LMETA
  "Right CMDKEY",       // NK_RMETA
#elif defined _WIN32_
  "Left WINKEY",       // NK_LMETA
  "Right WINKEY",       // NK_RMETA
#else
  "Left METAKEY",       // NK_LMETA
  "Right METAKEY",       // NK_RMETA
#endif
  "Menu"          // NK_MENU
};


const nglChar* GetKeyName(nglKeyCode Key)
{
  if (Key > NGL_KEY_MAX)
    Key = NK_NONE;
  
  return gpKeyName[Key];
}
