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
// $Id: OSystemWin32.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef OSYSTEM_WIN32_HXX
#define OSYSTEM_WIN32_HXX

#include "OSystem.hxx"
#include "bspf.hxx"

/**
  This class defines Windows system specific settings.

  @author  Stephen Anthony
  @version $Id: OSystemWin32.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class OSystemWin32 : public OSystem
{
  public:
    /**
      Create a new Win32 operating system object
    */
    OSystemWin32();

    /**
      Destructor
    */
    virtual ~OSystemWin32();

  public:
    /**
      Returns the default paths for the snapshot directory.
    */
    string defaultSnapSaveDir();
    string defaultSnapLoadDir();

    /**
      Move window to given position.  The width and height are also
      required for the underlying function, but the window size itself
      absolutely must not be changed.
    */
    void setAppWindowPos(int x, int y, int w, int h);
};

#endif
