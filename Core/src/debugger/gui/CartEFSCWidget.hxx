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
// $Id: CartEFSCWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef CARTRIDGEEFSC_WIDGET_HXX
#define CARTRIDGEEFSC_WIDGET_HXX

class CartridgeEFSC;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeEFSCWidget : public CartDebugWidget
{
  public:
    CartridgeEFSCWidget(GuiObject* boss, const GUI::Font& lfont,
                        const GUI::Font& nfont,
                        int x, int y, int w, int h,
                        CartridgeEFSC& cart);
    virtual ~CartridgeEFSCWidget() { }

    void loadConfig();
    void handleCommand(CommandSender* sender, int cmd, int data, int id);

    string bankState();

  private:
    CartridgeEFSC& myCart;
    PopUpWidget* myBank;

    enum { kBankChanged = 'bkCH' };
};

#endif
