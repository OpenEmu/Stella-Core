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
// $Id: Menu.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "Dialog.hxx"
#include "OptionsDialog.hxx"
#include "bspf.hxx"
#include "Menu.hxx"

class Properties;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Menu::Menu(OSystem* osystem)
  : DialogContainer(osystem)
{
  // This dialog is overlaid on the main TIA screen; we make sure it will fit
  // If the TIA can use 1x mode, it implies that the overlay can be no larger
  // than 320x240
  // Otherwise we can use 2x mode, in which 640x420 is the minimum TIA size
  int dw = osystem->desktopWidth(), dh = osystem->desktopHeight();
  if(dw < 640 || dh < 480)
  {
    dw = 320;  dh = 240;
  }
  else
  {
    dw = 480;  dh = 380;
  }
  myBaseDialog = new OptionsDialog(myOSystem, this, 0, dw, dh, false);  // in game mode
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Menu::~Menu()
{
}
