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
// $Id: TiaZoomWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef TIA_ZOOM_WIDGET_HXX
#define TIA_ZOOM_WIDGET_HXX

class GuiObject;
class ContextMenu;

#include "Widget.hxx"
#include "Command.hxx"


class TiaZoomWidget : public Widget, public CommandSender
{
  public:
    TiaZoomWidget(GuiObject *boss, const GUI::Font& font,
                  int x, int y, int w, int h);
    virtual ~TiaZoomWidget();

    void loadConfig();
    void setPos(int x, int y);

  protected:
    void handleMouseDown(int x, int y, int button, int clickCount);
    void handleMouseUp(int x, int y, int button, int clickCount);
    void handleMouseWheel(int x, int y, int direction);
    void handleMouseMoved(int x, int y, int button);
    void handleMouseLeft(int button);
    bool handleEvent(Event::Type event);
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

    void drawWidget(bool hilite);
    bool wantsFocus() { return true; }

  private:
    void zoom(int level);
    void recalc();

  private:
    ContextMenu* myMenu;

    int myZoomLevel;
    int myNumCols, myNumRows;
    int myXOff, myYOff;

    bool myMouseMoving;
    int myXClick, myYClick;
};

#endif
