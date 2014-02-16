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
// $Id: Launcher.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef LAUNCHER_HXX
#define LAUNCHER_HXX

class Properties;
class OSystem;
class FilesystemNode;

#include "DialogContainer.hxx"
#include "FrameBuffer.hxx"

/**
  The base dialog for the ROM launcher in Stella.

  @author  Stephen Anthony
  @version $Id: Launcher.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class Launcher : public DialogContainer
{
  public:
    /**
      Create a new menu stack
    */
    Launcher(OSystem* osystem);

    /**
      Destructor
    */
    virtual ~Launcher();

    /**
      Initialize the video subsystem wrt this class.
    */
    FBInitStatus initializeVideo();

    /**
      Wrapper for LauncherDialog::selectedRomMD5() method.
    */
    const string& selectedRomMD5();

    /**
      Wrapper for LauncherDialog::currentNode() method.
    */
    const FilesystemNode& currentNode() const;

    /**
      Wrapper for LauncherDialog::reload() method.
    */
    void reload();

  private:
    // The width and height of this dialog
    uInt32 myWidth;
    uInt32 myHeight;
};

#endif
