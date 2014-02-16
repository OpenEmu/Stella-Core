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
// $Id: RomInfoWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef ROM_INFO_WIDGET_HXX
#define ROM_INFO_WIDGET_HXX

#include <fstream>

#include "Props.hxx"
#include "Widget.hxx"
#include "Command.hxx"
#include "StringList.hxx"
#include "bspf.hxx"


class RomInfoWidget : public Widget
{
  public:
    RomInfoWidget(GuiObject *boss, const GUI::Font& font,
                  int x, int y, int w, int h);
    virtual ~RomInfoWidget();

    void setProperties(const Properties& props);
    void clearProperties();
    void loadConfig();

  protected:
    void drawWidget(bool hilite);

  private:
    void parseProperties();

  private:
    // Surface id and pointer holding the scaled PNG image
    FBSurface* mySurface;
    int mySurfaceID;

    // How much to zoom the PNG image
    int myZoomLevel;

    // Whether the surface should be redrawn by drawWidget()
    bool mySurfaceIsValid;

    // Some ROM properties info, as well as 'tEXt' chunks from the PNG image
    StringList myRomInfo;

    // The properties for the currently selected ROM
    Properties myProperties;

    // Indicates if the current properties should actually be used
    bool myHaveProperties;

    // Indicates if an error occurred in creating/displaying the surface
    string mySurfaceErrorMsg;
};

#endif
