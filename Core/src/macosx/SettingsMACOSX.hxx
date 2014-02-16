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
// $Id: SettingsMACOSX.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef SETTINGS_MAC_OSX_HXX
#define SETTINGS_MAX_OSX_HXX

class OSystem;

#include "Settings.hxx"

/**
  This class defines Macintosh OSX system specific settings.

  @author  Mark Grebe
  @version $Id: SettingsMACOSX.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class SettingsMACOSX : public Settings
{
  public:
    /**
      Create a new UNIX settings object
    */
    SettingsMACOSX(OSystem* osystem);

    /**
      Destructor
    */
    virtual ~SettingsMACOSX();

  public:
	
    /**
      This method should be called to load the current settings from the 
	  standard Mac preferences.
    */
    void loadConfig();

    /**
      This method should be called to save the current settings to the
	  standard Mac preferences.
    */
    void saveConfig();
};

#endif
