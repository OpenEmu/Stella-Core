//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2014 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: DialogContainer.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "Dialog.hxx"
#include "Stack.hxx"
#include "EventHandler.hxx"
#include "Joystick.hxx"
#include "bspf.hxx"
#include "DialogContainer.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DialogContainer::DialogContainer(OSystem* osystem)
  : myOSystem(osystem),
    myBaseDialog(NULL),
    myTime(0)
{
  reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DialogContainer::~DialogContainer()
{
  if(myBaseDialog)
    delete myBaseDialog;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::updateTime(uInt64 time)
{
  // We only need millisecond precision
  myTime = time / 1000;

  if(myDialogStack.empty())
    return;

  // Check for pending continuous events and send them to the active dialog box
  Dialog* activeDialog = myDialogStack.top();

  // Key still pressed
  if(myCurrentKeyDown.keycode != 0 && myKeyRepeatTime < myTime)
  {
    activeDialog->handleKeyDown(myCurrentKeyDown.keycode, myCurrentKeyDown.flags,
                                myCurrentKeyDown.ascii);
    myKeyRepeatTime = myTime + kRepeatSustainDelay;
  }

  // Mouse button still pressed
  if(myCurrentMouseDown.button != -1 && myClickRepeatTime < myTime)
  {
    activeDialog->handleMouseDown(myCurrentMouseDown.x - activeDialog->_x,
                                  myCurrentMouseDown.y - activeDialog->_y,
                                  myCurrentMouseDown.button, 1);
    myClickRepeatTime = myTime + kRepeatSustainDelay;
  }

  // Joystick button still pressed
  if(myCurrentButtonDown.stick != -1 && myButtonRepeatTime < myTime)
  {
    activeDialog->handleJoyDown(myCurrentButtonDown.stick, myCurrentButtonDown.button);
    myButtonRepeatTime = myTime + kRepeatSustainDelay;
  }

  // Joystick axis still pressed
  if(myCurrentAxisDown.stick != -1 && myAxisRepeatTime < myTime)
  {
    activeDialog->handleJoyAxis(myCurrentAxisDown.stick, myCurrentAxisDown.axis,
                                myCurrentAxisDown.value);
    myAxisRepeatTime = myTime + kRepeatSustainDelay;
  }

  // Joystick hat still pressed
  if(myCurrentHatDown.stick != -1 && myHatRepeatTime < myTime)
  {
    activeDialog->handleJoyHat(myCurrentHatDown.stick, myCurrentHatDown.hat,
                                myCurrentHatDown.value);
    myHatRepeatTime = myTime + kRepeatSustainDelay;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::draw(bool full)
{
  // Draw all the dialogs on the stack when we want a full refresh
  if(full)
  {
    for(int i = 0; i < myDialogStack.size(); i++)
    {
      myDialogStack[i]->center();
      myDialogStack[i]->setDirty();
      myDialogStack[i]->drawDialog();
    }
  }
  else if(!myDialogStack.empty())
    myDialogStack.top()->drawDialog();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::addDialog(Dialog* d)
{
  const GUI::Rect& screen = myOSystem->frameBuffer().screenRect();
  assert(d->getWidth() <= screen.width() && d->getHeight() <= screen.height());

  myDialogStack.push(d);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::removeDialog()
{
  if(!myDialogStack.empty())
    myDialogStack.pop();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::reStack()
{
  // Pop all items from the stack, and then add the base menu
  while(!myDialogStack.empty())
    myDialogStack.top()->close(false);  // don't force a refresh

  myBaseDialog->open(false);  // don't force a refresh

  myOSystem->frameBuffer().refresh();

  // Reset all continuous events
  reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleKeyEvent(StellaKey key, StellaMod mod,
                                     char ascii, bool state)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();
  if(state)
  {
    myCurrentKeyDown.keycode = key;
    myCurrentKeyDown.flags   = mod;
    myCurrentKeyDown.ascii   = ascii;
    myKeyRepeatTime = myTime + kRepeatInitialDelay;

    activeDialog->handleKeyDown(key, mod, ascii);
  }
  else
  {
    activeDialog->handleKeyUp(key, mod, ascii);

    // Only stop firing events if it's the current key
    if (key == myCurrentKeyDown.keycode)
      myCurrentKeyDown.keycode = KBDK_UNKNOWN;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleMouseMotionEvent(int x, int y, int button)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();
  activeDialog->surface().translateCoords(x, y);
  activeDialog->handleMouseMoved(x - activeDialog->_x,
                                 y - activeDialog->_y,
                                 button);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleMouseButtonEvent(MouseButton b, int x, int y, uInt8 state)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();
  activeDialog->surface().translateCoords(x, y);

  int button = (b == EVENT_LBUTTONDOWN || b == EVENT_LBUTTONUP) ? 1 : 2;
  switch(b)
  {
    case EVENT_LBUTTONDOWN:
    case EVENT_RBUTTONDOWN:
      // If more than two clicks have been recorded, we start over
      if(myLastClick.count == 2)
      {
        myLastClick.x = myLastClick.y = 0;
        myLastClick.time = 0;
        myLastClick.count = 0;
      }

      if(myLastClick.count && (myTime < myLastClick.time + kDoubleClickDelay)
         && BSPF_abs(myLastClick.x - x) < 3
         && BSPF_abs(myLastClick.y - y) < 3)
      {
        myLastClick.count++;
      }
      else
      {
        myLastClick.x = x;
        myLastClick.y = y;
        myLastClick.count = 1;
      }
      myLastClick.time = myTime;

      // Now account for repeated mouse events (click and hold), but only
      // if the dialog wants them
      if(activeDialog->handleMouseClicks(x - activeDialog->_x, y - activeDialog->_y,
                                         button))
      {
        myCurrentMouseDown.x = x;
        myCurrentMouseDown.y = y;
        myCurrentMouseDown.button = button;
        myClickRepeatTime = myTime + kRepeatInitialDelay;
      }
      else
        myCurrentMouseDown.button = -1;

      activeDialog->handleMouseDown(x - activeDialog->_x, y - activeDialog->_y,
                                    button, myLastClick.count);
      break;

    case EVENT_LBUTTONUP:
    case EVENT_RBUTTONUP:
      activeDialog->handleMouseUp(x - activeDialog->_x, y - activeDialog->_y,
                                  button, myLastClick.count);

      if(button == myCurrentMouseDown.button)
        myCurrentMouseDown.button = -1;
      break;

    case EVENT_WHEELUP:
      activeDialog->handleMouseWheel(x - activeDialog->_x, y - activeDialog->_y, -1);
      break;

    case EVENT_WHEELDOWN:
      activeDialog->handleMouseWheel(x - activeDialog->_x, y - activeDialog->_y, 1);
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleJoyEvent(int stick, int button, uInt8 state)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();

  if(state == 1)
  {
    myCurrentButtonDown.stick  = stick;
    myCurrentButtonDown.button = button;
    myButtonRepeatTime = myTime + kRepeatInitialDelay;

    activeDialog->handleJoyDown(stick, button);
  }
  else
  {
    // Only stop firing events if it's the current button
    if(stick == myCurrentButtonDown.stick)
      myCurrentButtonDown.stick = myCurrentButtonDown.button = -1;

    activeDialog->handleJoyUp(stick, button);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleJoyAxisEvent(int stick, int axis, int value)
{
  if(myDialogStack.empty())
    return;

  // Only stop firing events if it's the current stick
  if(myCurrentAxisDown.stick == stick && value == 0)
  {
    myCurrentAxisDown.stick = myCurrentAxisDown.axis = -1;
  }
  else if(value != 0)  // never repeat the 'off' event
  {
    // Now account for repeated axis events (press and hold)
    myCurrentAxisDown.stick = stick;
    myCurrentAxisDown.axis  = axis;
    myCurrentAxisDown.value = value;
    myAxisRepeatTime = myTime + kRepeatInitialDelay;
  }
  myDialogStack.top()->handleJoyAxis(stick, axis, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::handleJoyHatEvent(int stick, int hat, JoyHat value)
{
  if(myDialogStack.empty())
    return;

  // Only stop firing events if it's the current stick
  if(myCurrentHatDown.stick == stick && value == EVENT_HATCENTER)
  {
    myCurrentHatDown.stick = myCurrentHatDown.hat = -1;
  }
  else if(value != EVENT_HATCENTER)  // never repeat the 'center' direction
  {
    // Now account for repeated hat events (press and hold)
    myCurrentHatDown.stick = stick;
    myCurrentHatDown.hat  = hat;
    myCurrentHatDown.value = value;
    myHatRepeatTime = myTime + kRepeatInitialDelay;
  }
  myDialogStack.top()->handleJoyHat(stick, hat, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DialogContainer::reset()
{
  myCurrentKeyDown.keycode = KBDK_UNKNOWN;
  myCurrentMouseDown.button = -1;
  myLastClick.x = myLastClick.y = 0;
  myLastClick.time = 0;
  myLastClick.count = 0;

  myCurrentButtonDown.stick = myCurrentButtonDown.button = -1;
  myCurrentAxisDown.stick = myCurrentAxisDown.axis = -1;
  myCurrentHatDown.stick = myCurrentHatDown.hat = -1;
}
