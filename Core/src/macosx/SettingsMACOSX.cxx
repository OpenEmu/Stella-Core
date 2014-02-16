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
// $Id: SettingsMACOSX.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "bspf.hxx"
#include "Settings.hxx"
#include "SettingsMACOSX.hxx"

extern "C" {
  void prefsSetString(const char* key, const char* value);
  void prefsGetString(const char* key, char* value, int size);
  void prefsSave(void);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsMACOSX::SettingsMACOSX(OSystem* osystem)
  : Settings(osystem)
{
  setInternal("video", "gl");        // Use opengl mode by default
  setInternal("gl_lib", "libGL.so"); // Try this one first, then let the system decide
  setInternal("gl_vsync", "true");   // OSX almost always supports vsync; let's use it
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SettingsMACOSX::~SettingsMACOSX()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SettingsMACOSX::loadConfig()
{
  string key, value;
  char cvalue[4096];
  
  // Read key/value pairs from the plist file
  const SettingsArray& settings = getInternalSettings();
  for(unsigned int i = 0; i < settings.size(); ++i)
  {
    prefsGetString(settings[i].key.c_str(), cvalue, 4090);
    if(cvalue[0] != 0)
      setInternal(settings[i].key, cvalue, i, true);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SettingsMACOSX::saveConfig()
{
  // Write out each of the key and value pairs
  const SettingsArray& settings = getInternalSettings();
  for(unsigned int i = 0; i < settings.size(); ++i)
    prefsSetString(settings[i].key.c_str(), settings[i].value.toCString());

  prefsSave();
}
