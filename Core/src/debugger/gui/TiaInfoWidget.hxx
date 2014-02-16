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
// $Id: TiaInfoWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef TIA_INFO_WIDGET_HXX
#define TIA_INFO_WIDGET_HXX

class GuiObject;
class EditTextWidget;

#include "Widget.hxx"
#include "Command.hxx"


class TiaInfoWidget : public Widget, public CommandSender
{
  public:
    TiaInfoWidget(GuiObject *boss, const GUI::Font& lfont, const GUI::Font& nfont,
                  int x, int y, int max_w);
    virtual ~TiaInfoWidget();

    void loadConfig();

  protected:
    void handleMouseDown(int x, int y, int button, int clickCount);
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

  private:
    EditTextWidget* myFrameCount;
    EditTextWidget* myFrameCycles;

    EditTextWidget* myScanlineCount;
    EditTextWidget* myScanlineCycles;
    EditTextWidget* myPixelPosition;
    EditTextWidget* myColorClocks;

    CheckboxWidget* myVSync;
    CheckboxWidget* myVBlank;
};

#endif
