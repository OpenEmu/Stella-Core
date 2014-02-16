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
// $Id: PopUpWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef POPUP_WIDGET_HXX
#define POPUP_WIDGET_HXX

class GUIObject;

#include "bspf.hxx"

#include "Array.hxx"
#include "Command.hxx"
#include "ContextMenu.hxx"
#include "StringList.hxx"
#include "Widget.hxx"


/**
 * Popup or dropdown widget which, when clicked, "pop up" a list of items and
 * lets the user pick on of them.
 *
 * Implementation wise, when the user selects an item, then a kPopUpItemSelectedCmd 
 * is broadcast, with data being equal to the tag value of the selected entry.
 */
class PopUpWidget : public Widget, public CommandSender
{
  public:
    PopUpWidget(GuiObject* boss, const GUI::Font& font,
                int x, int y, int w, int h, const VariantList& items,
                const string& label, int labelWidth = 0, int cmd = 0);
    ~PopUpWidget();

    /** Add the given items to the widget. */

    /** Various selection methods passed directly to the underlying menu
        See ContextMenu.hxx for more information. */
    void addItems(const VariantList& items) { myMenu->addItems(items);     }
    void setSelected(const Variant& tag,
                     const Variant& def = EmptyVariant)
                                          { myMenu->setSelected(tag, def); }
    void setSelectedIndex(int idx)        { myMenu->setSelectedIndex(idx); }
    void setSelectedMax()                 { myMenu->setSelectedMax();      }
    void clearSelection()                 { myMenu->clearSelection();      }

    int getSelected() const               { return myMenu->getSelected();     }
    const string& getSelectedName() const { return myMenu->getSelectedName(); }
    const Variant& getSelectedTag() const { return myMenu->getSelectedTag();  }

    bool wantsFocus()  { return true; }

  protected:
    void handleMouseDown(int x, int y, int button, int clickCount);
    void handleMouseWheel(int x, int y, int direction);
    bool handleEvent(Event::Type e);
    void handleCommand(CommandSender* sender, int cmd, int data, int id);
    void drawWidget(bool hilite);

  private:
    ContextMenu* myMenu;
    int myArrowsY;
    int myTextY;

    string _label;
    int    _labelWidth;
};

#endif
