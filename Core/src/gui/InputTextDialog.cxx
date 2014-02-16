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
// $Id: InputTextDialog.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "bspf.hxx"

#include "Dialog.hxx"
#include "DialogContainer.hxx"
#include "EditTextWidget.hxx"
#include "GuiObject.hxx"
#include "OSystem.hxx"
#include "Widget.hxx"

#include "InputTextDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputTextDialog::InputTextDialog(GuiObject* boss, const GUI::Font& font,
                                 const StringList& labels)
  : Dialog(&boss->instance(), &boss->parent(), 0, 0, 16, 16),
    CommandSender(boss),
    myEnableCenter(false),
    myErrorFlag(false),
    myXOrig(0),
    myYOrig(0)
{
  initialize(font, font, labels);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputTextDialog::InputTextDialog(GuiObject* boss, const GUI::Font& lfont,
                                 const GUI::Font& nfont,
                                 const StringList& labels)
  : Dialog(&boss->instance(), &boss->parent(), 0, 0, 16, 16),
    CommandSender(boss),
    myEnableCenter(false),
    myErrorFlag(false),
    myXOrig(0),
    myYOrig(0)
{
  initialize(lfont, nfont, labels);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputTextDialog::~InputTextDialog()
{
  myInput.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::initialize(const GUI::Font& lfont, const GUI::Font& nfont,
                                 const StringList& labels)
{
  const int fontWidth  = lfont.getMaxCharWidth(),
            fontHeight = lfont.getFontHeight(),
            lineHeight = lfont.getLineHeight();
  unsigned int xpos, ypos, i, lwidth = 0, maxIdx = 0;
  WidgetArray wid;

  // Calculate real dimensions
  _w = fontWidth * 30;
  _h = lineHeight * 4 + labels.size() * (lineHeight + 5);

  // Determine longest label
  for(i = 0; i < labels.size(); ++i)
  {
    if(labels[i].length() > lwidth)
    {
      lwidth = labels[i].length();
      maxIdx = i;
    }
  }
  lwidth = lfont.getStringWidth(labels[maxIdx]);

  // Create editboxes for all labels
  ypos = lineHeight;
  for(i = 0; i < labels.size(); ++i)
  {
    xpos = 10;
    new StaticTextWidget(this, lfont, xpos, ypos,
                         lwidth, fontHeight,
                         labels[i], kTextAlignLeft);

    xpos += lwidth + fontWidth;
    EditTextWidget* w = new EditTextWidget(this, nfont, xpos, ypos,
                                           _w - xpos - 10, lineHeight, "");
    wid.push_back(w);

    myInput.push_back(w);
    ypos += lineHeight + 5;
  }

  xpos = 10;
  myTitle = new StaticTextWidget(this, lfont, xpos, ypos, _w - 2*xpos, fontHeight,
                                 "", kTextAlignCenter);
  myTitle->setTextColor(kTextColorEm);

  addToFocusList(wid);

  // Add OK and Cancel buttons
  wid.clear();
  addOKCancelBGroup(wid, lfont);
  addBGroupToFocusList(wid);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::show()
{
  myEnableCenter = true;
  open();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::show(uInt32 x, uInt32 y)
{
  myXOrig = x;
  myYOrig = y;
  myEnableCenter = false;
  open();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::center()
{
  if(!myEnableCenter)
  {
    // Make sure the menu is exactly where it should be, in case the image
    // offset has changed
    const GUI::Rect& image = instance().frameBuffer().imageRect();
    uInt32 x = image.x() + myXOrig;
    uInt32 y = image.y() + myYOrig;
    uInt32 tx = image.x() + image.width();
    uInt32 ty = image.y() + image.height();
    if(x + _w > tx) x -= (x + _w - tx);
    if(y + _h > ty) y -= (y + _h - ty);

    surface().setPos(x, y);
  }
  else
    Dialog::center();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setTitle(const string& title)
{
  myTitle->setLabel(title);
  myErrorFlag = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& InputTextDialog::getResult(int idx)
{
  if((unsigned int)idx < myInput.size())
    return myInput[idx]->getText();
  else
    return EmptyString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setText(const string& str, int idx)
{
  if((unsigned int)idx < myInput.size())
    myInput[idx]->setText(str);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setFocus(int idx)
{
  if((unsigned int)idx < myInput.size())
    Dialog::setFocus(getFocusList()[idx]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::handleCommand(CommandSender* sender, int cmd,
                                    int data, int id)
{
  switch(cmd)
  {
    case kOKCmd:
    case EditableWidget::kAcceptCmd:
    {
      // Send a signal to the calling class that a selection has been made
      // Since we aren't derived from a widget, we don't have a 'data' or 'id'
      if(myCmd)
        sendCommand(myCmd, 0, 0);

      // We don't close, but leave the parent to do it
      // If the data isn't valid, the parent may wait until it is
      break;
    }

    case EditableWidget::kChangedCmd:
      // Erase the invalid message once editing is restarted
      if(myErrorFlag)
      {
        myTitle->setLabel("");
        myErrorFlag = false;
      }
      break;

    case EditableWidget::kCancelCmd:
      Dialog::handleCommand(sender, kCloseCmd, data, id);
      break;


    default:
      Dialog::handleCommand(sender, cmd, data, id);
      break;
  }
}
