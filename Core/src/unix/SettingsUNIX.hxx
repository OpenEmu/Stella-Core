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
// $Id: SettingsUNIX.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef SETTINGS_UNIX_HXX
#define SETTINGS_UNIX_HXX

class OSystem;

#include "bspf.hxx"

/**
  This class defines UNIX-like OS's (Linux) system specific settings.

  @author  Stephen Anthony
  @version $Id: SettingsUNIX.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class SettingsUNIX : public Settings
{
  public:
    /**
      Create a new UNIX settings object
    */
    SettingsUNIX(OSystem* osystem);

    /**
      Destructor
    */
    virtual ~SettingsUNIX();
};

#endif
