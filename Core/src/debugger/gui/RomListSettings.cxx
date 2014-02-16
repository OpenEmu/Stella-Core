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
// $Id: RomListSettings.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "Dialog.hxx"
#include "DialogContainer.hxx"
#include "RomListWidget.hxx"
#include "RomListSettings.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomListSettings::RomListSettings(GuiObject* boss, const GUI::Font& font)
  : Dialog(&boss->instance(), &boss->parent(), 0, 0, 16, 16),
    CommandSender(boss),
    _xorig(0),
    _yorig(0)
{
  const int buttonWidth  = font.getStringWidth("RunTo PC @ current line") + 20,
            buttonHeight = font.getLineHeight() + 4;
  int xpos = 8, ypos = 8;
  WidgetArray wid;

  // Set PC to current line
  ButtonWidget* setPC = 
    new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                     "Set PC @ current line", RomListWidget::kSetPCCmd);
  wid.push_back(setPC);

  // RunTo PC on current line
  ypos += buttonHeight + 4;
  ButtonWidget* runtoPC = 
    new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                     "RunTo PC @ current line", RomListWidget::kRuntoPCCmd);
  wid.push_back(runtoPC);

  // Re-disassemble
  ypos += buttonHeight + 4;
  ButtonWidget* disasm = 
    new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                     "Re-disassemble", RomListWidget::kDisassembleCmd);
  wid.push_back(disasm);

  // Settings for Distella
  xpos += 4;  ypos += buttonHeight + 8;
  myShowTentative = new CheckboxWidget(this, font, xpos, ypos,
                        "Show tentative code", RomListWidget::kTentativeCodeCmd);
  wid.push_back(myShowTentative);
  ypos += buttonHeight + 4;
  myShowAddresses = new CheckboxWidget(this, font, xpos, ypos,
                        "Show PC addresses", RomListWidget::kPCAddressesCmd);
  wid.push_back(myShowAddresses);
  ypos += buttonHeight + 4;
  myShowGFXBinary = new CheckboxWidget(this, font, xpos, ypos,
                        "Show GFX as binary", RomListWidget::kGfxAsBinaryCmd);
  wid.push_back(myShowGFXBinary);
  ypos += buttonHeight + 4;
  myUseRelocation = new CheckboxWidget(this, font, xpos, ypos,
                        "Use address relocation", RomListWidget::kAddrRelocationCmd);
  wid.push_back(myUseRelocation);

  // Set real dimensions
  _w = buttonWidth + 20;
  _h = ypos + buttonHeight + 8;

  addToFocusList(wid);

  // We don't have a close/cancel button, but we still want the cancel
  // event to be processed
  processCancelWithoutWidget(true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomListSettings::~RomListSettings()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomListSettings::show(uInt32 x, uInt32 y, int data)
{
  _xorig = x;
  _yorig = y;
  _item = data;

  open();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomListSettings::center()
{
  // Make sure the menu is exactly where it should be, in case the image
  // offset has changed
  const GUI::Rect& image = instance().frameBuffer().imageRect();
  uInt32 x = image.x() + _xorig;
  uInt32 y = image.y() + _yorig;
  uInt32 tx = image.x() + image.width();
  uInt32 ty = image.y() + image.height();
  if(x + _w > tx) x -= (x + _w - tx);
  if(y + _h > ty) y -= (y + _h - ty);

  surface().setPos(x, y);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomListSettings::loadConfig()
{
  myShowTentative->setState(instance().settings().getBool("dis.resolve"));
  myShowAddresses->setState(instance().settings().getBool("dis.showaddr"));
  myShowGFXBinary->setState(instance().settings().getString("dis.gfxformat") == "2");
  myUseRelocation->setState(instance().settings().getBool("dis.relocate"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomListSettings::handleMouseDown(int x, int y, int button, int clickCount)
{
  // Close dialog if mouse click is outside it (simulates a context menu)
  // Otherwise let the base dialog class process it
  if(x >= 0 && x < _w && y >= 0 && y < _h)
    Dialog::handleMouseDown(x, y, button, clickCount);
  else
    close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomListSettings::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  // We remove the dialog when the user has selected an item
  // Make sure the dialog is removed before sending any commands,
  // since one consequence of sending a command may be to add another
  // dialog/menu
  close();

  switch(cmd)
  {
    case RomListWidget::kSetPCCmd:
    case RomListWidget::kRuntoPCCmd:
    {
      sendCommand(cmd, _item, -1);
      break;
    }
    case RomListWidget::kDisassembleCmd:
    {
      sendCommand(cmd, -1, -1);
      break;
    }
    case RomListWidget::kTentativeCodeCmd:
    {
      sendCommand(cmd, myShowTentative->getState(), -1);
      break;
    }
    case RomListWidget::kPCAddressesCmd:
    {
      sendCommand(cmd, myShowAddresses->getState(), -1);
      break;
    }
    case RomListWidget::kGfxAsBinaryCmd:
    {
      sendCommand(cmd, myShowGFXBinary->getState(), -1);
      break;
    }
    case RomListWidget::kAddrRelocationCmd:
    {
      sendCommand(cmd, myUseRelocation->getState(), -1);
      break;
    }
  }
}
