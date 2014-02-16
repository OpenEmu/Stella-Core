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
// $Id: CartF8SCWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef CARTRIDGEF8SC_WIDGET_HXX
#define CARTRIDGEF8SC_WIDGET_HXX

class CartridgeF8SC;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeF8SCWidget : public CartDebugWidget
{
  public:
    CartridgeF8SCWidget(GuiObject* boss, const GUI::Font& lfont,
                        const GUI::Font& nfont,
                        int x, int y, int w, int h,
                        CartridgeF8SC& cart);
    virtual ~CartridgeF8SCWidget() { }

    void loadConfig();
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

    string bankState();

  private:
    CartridgeF8SC& myCart;
    PopUpWidget* myBank;

    enum { kBankChanged = 'bkCH' };
};

#endif
