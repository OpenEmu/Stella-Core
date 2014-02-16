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
// $Id: ComboDialog.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef COMBO_DIALOG_HXX
#define COMBO_DIALOG_HXX

class PopUpWidget;
class EditTextWidget;
class StaticTextWidget;
class VariantList;
class OSystem;

#include "Dialog.hxx"
#include "bspf.hxx"

class ComboDialog : public Dialog
{
  public:
    ComboDialog(GuiObject* boss, const GUI::Font& font, const VariantList& combolist);
    ~ComboDialog();

    /** Place the dialog onscreen and center it */
    void show(Event::Type event, const string& name);

  private:
    void loadConfig();
    void saveConfig();
    void setDefaults();

    virtual void handleCommand(CommandSender* sender, int cmd, int data, int id);

  private:
    Event::Type myComboEvent;

    StaticTextWidget* myComboName;
    PopUpWidget*      myEvents[8];
};

#endif
