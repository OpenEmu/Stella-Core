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
// $Id: ColorWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef COLOR_WIDGET_HXX
#define COLOR_WIDGET_HXX

class ColorDialog;
class GuiObject;

#include "Widget.hxx"
#include "Command.hxx"

/**
  Displays a color from the TIA palette.  This class will eventually
  be expanded with a TIA palette table, to set the color visually.
  
  @author  Stephen Anthony
  @version $Id: ColorWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class ColorWidget : public Widget, public CommandSender
{
  friend class ColorDialog;

  public:
    ColorWidget(GuiObject* boss, const GUI::Font& font,
                int x, int y, int w, int h, int cmd = 0);
    ~ColorWidget();

    void setColor(int color);
    int  getColor() const    { return _color;  }
	
  protected:
    void drawWidget(bool hilite);

  protected:
    int _color;
    int	_cmd;
};

#endif
