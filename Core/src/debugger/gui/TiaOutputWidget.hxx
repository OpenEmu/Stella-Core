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
// $Id: TiaOutputWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef TIA_OUTPUT_WIDGET_HXX
#define TIA_OUTPUT_WIDGET_HXX

class GuiObject;
class ContextMenu;
class TiaZoomWidget;

#include "Widget.hxx"
#include "Command.hxx"


class TiaOutputWidget : public Widget, public CommandSender
{
  public:
    TiaOutputWidget(GuiObject *boss, const GUI::Font& font,
                    int x, int y, int w, int h);
    virtual ~TiaOutputWidget();

    void loadConfig();
    void setZoomWidget(TiaZoomWidget* w) { myZoom = w; }

// Eventually, these methods will enable access to the onscreen TIA image
// For example, clicking an area may cause an action
// (fill to this scanline, etc).
/*
    virtual void handleMouseUp(int x, int y, int button, int clickCount);
    virtual void handleMouseWheel(int x, int y, int direction);
    virtual bool handleKeyDown(StellaKey key, StellaMod mod, char ascii);
    virtual bool handleKeyUp(StellaKey key, StellaMod mod, char ascii);
*/

  protected:
    void handleMouseDown(int x, int y, int button, int clickCount);
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

    void drawWidget(bool hilite);
    bool wantsFocus() { return false; }

  private:
    ContextMenu*   myMenu;
    TiaZoomWidget* myZoom;

    int myClickX, myClickY;

    // Create this buffer once, instead of allocating it each time the
    // TIA image is redrawn
    uInt32 myLineBuffer[320];
};

#endif
