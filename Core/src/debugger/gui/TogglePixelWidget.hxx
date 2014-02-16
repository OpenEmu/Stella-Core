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
// $Id: TogglePixelWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef TOGGLE_PIXEL_WIDGET_HXX
#define TOGGLE_PIXEL_WIDGET_HXX

#include "ToggleWidget.hxx"

/* TogglePixelWidget */
class TogglePixelWidget : public ToggleWidget
{
  public:
    TogglePixelWidget(GuiObject* boss, const GUI::Font& font,
                      int x, int y, int cols, int rows);
    virtual ~TogglePixelWidget();

    void setColor(int color) {
      _pixelColor = (color >= 0 && color <= 255) ? color : kDlgColor;
    }
    void setBackgroundColor(int color) {
      _backgroundColor = (color >= 0 && color <= 255) ? color : kDlgColor;
    }
    void setState(const BoolArray& state);

    void setIntState(int value, bool swap);
    int  getIntState();

  protected:
    void drawWidget(bool hilite);

  private:
    int  _pixelColor, _backgroundColor;
    bool _swapBits;
};

#endif
