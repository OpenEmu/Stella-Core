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
// $Id: mainSDL.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include <SDL.h>
#include <cstdlib>

#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "EventHandler.hxx"
#include "FrameBuffer.hxx"
#include "PropsSet.hxx"
#include "Sound.hxx"
#include "Settings.hxx"
#include "FSNode.hxx"
#include "OSystem.hxx"
#include "System.hxx"

#if defined(UNIX)
  #include "SettingsUNIX.hxx"
  #include "OSystemUNIX.hxx"
#elif defined(WIN32)
  #include "SettingsWin32.hxx"
  #include "OSystemWin32.hxx"
#elif defined(MAC_OSX)
  #include "SettingsMACOSX.hxx"
  #include "OSystemMACOSX.hxx"
  extern "C" {
    int stellaMain(int argc, char* argv[]);
  }
#else
  #error Unsupported platform!
#endif

#ifdef DEBUGGER_SUPPORT
  #include "Debugger.hxx"
#endif

#ifdef CHEATCODE_SUPPORT
  #include "CheatManager.hxx"
#endif

// Pointer to the main parent osystem object or the null pointer
OSystem* theOSystem = (OSystem*) NULL;

// Does general Cleanup in case any operation failed (or at end of program)
int Cleanup()
{
  theOSystem->logMessage("Cleanup from mainSDL", 2);
  theOSystem->saveConfig();

  if(theOSystem)
    delete theOSystem;

  if(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)
    SDL_Quit();

  return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if defined(MAC_OSX)
int stellaMain(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  ios_base::sync_with_stdio(false);

  // Create the parent OSystem object and settings
#if defined(UNIX)
  theOSystem = new OSystemUNIX();
  SettingsUNIX settings(theOSystem);
#elif defined(WIN32)
  theOSystem = new OSystemWin32();
  SettingsWin32 settings(theOSystem);
#elif defined(MAC_OSX)
  theOSystem = new OSystemMACOSX();
  SettingsMACOSX settings(theOSystem);
#else
  #error Unsupported platform!
#endif

  theOSystem->loadConfig();
  theOSystem->logMessage("Loading config options ...", 2);

  // Take care of commandline arguments
  theOSystem->logMessage("Loading commandline arguments ...", 2);
  string romfile = theOSystem->settings().loadCommandLine(argc, argv);

  // Finally, make sure the settings are valid
  // We do it once here, so the rest of the program can assume valid settings
  theOSystem->logMessage("Validating config options ...", 2);
  theOSystem->settings().validate();

  // Create the full OSystem after the settings, since settings are
  // probably needed for defaults
  theOSystem->logMessage("Creating the OSystem ...", 2);
  if(!theOSystem->create())
  {
    theOSystem->logMessage("ERROR: Couldn't create OSystem", 0);
    return Cleanup();
  }

  // Check to see if the user requested info about a specific ROM,
  // or the list of internal ROMs
  // If so, show the information and immediately exit
  if(theOSystem->settings().getBool("listrominfo"))
  {
    theOSystem->logMessage("Showing output from 'listrominfo' ...", 2);
    theOSystem->propSet().print();
    return Cleanup();
  }
  else if(theOSystem->settings().getBool("rominfo"))
  {
    theOSystem->logMessage("Showing output from 'rominfo' ...", 2);
    FilesystemNode romnode(romfile);
    theOSystem->logMessage(theOSystem->getROMInfo(romnode), 0);

    return Cleanup();
  }
  else if(theOSystem->settings().getBool("help"))
  {
    theOSystem->logMessage("Displaying usage", 2);
    theOSystem->settings().usage();
    return Cleanup();
  }

#ifdef BSPF_UNIX
  // Nvidia cards under UNIX don't currently support SDL_GL_SWAP_CONTROL
  // So we need to do it with an Nvidia-specific environment variable
  // This also means the setting can only be changed by restarting Stella
  // This functionality should really be integrated into SDL directly
  if(theOSystem->settings().getBool("gl_vsync"))
    putenv((char*)"__GL_SYNC_TO_VBLANK=1");
#endif

  //// Main loop ////
  // First we check if a ROM is specified on the commandline.  If so, and if
  //   the ROM actually exists, use it to create a new console.
  // Next we check if a directory is specified on the commandline.  If so,
  //   open the rom launcher in that directory.
  // If not, use the built-in ROM launcher.  In this case, we enter 'launcher'
  //   mode and let the main event loop take care of opening a new console/ROM.
  FilesystemNode romnode(romfile);
  if(romfile == "" || romnode.isDirectory())
  {
    theOSystem->logMessage("Attempting to use ROM launcher ...", 2);
    bool launcherOpened = romfile != "" ?
      theOSystem->createLauncher(romnode.getPath()) : theOSystem->createLauncher();
    if(!launcherOpened)
    {
      theOSystem->logMessage("Launcher could not be started, showing usage", 2);
      theOSystem->settings().usage();
      return Cleanup();
    }
  }
  else
  {
    const string& result = theOSystem->createConsole(romnode);
    if(result != EmptyString)
      return Cleanup();

    if(theOSystem->settings().getBool("takesnapshot"))
    {
      theOSystem->logMessage("Taking snapshots with 'takesnapshot' ...", 2);
      for(int i = 0; i < 30; ++i)  theOSystem->frameBuffer().update();
      theOSystem->eventHandler().takeSnapshot();
      return Cleanup();
    }

#ifdef DEBUGGER_SUPPORT
    // Set up any breakpoint that was on the command line
    // (and remove the key from the settings, so they won't get set again)
    const string& initBreak = theOSystem->settings().getString("break");
    if(initBreak != "")
    {
      Debugger& dbg = theOSystem->debugger();
      int bp = dbg.stringToValue(initBreak);
      dbg.setBreakPoint(bp, true);
      theOSystem->settings().setValue("break", "");
    }
#endif
  }

  // Swallow any spurious events in the queue
  // These are normally caused by joystick/mouse jitter
  SDL_Event event;
  while(SDL_PollEvent(&event)) /* swallow event */ ;

  // Start the main loop, and don't exit until the user issues a QUIT command
  theOSystem->logMessage("Starting main loop ...", 2);
  theOSystem->mainLoop();
  theOSystem->logMessage("Finished main loop ...", 2);

  // Cleanup time ...
  return Cleanup();
}
