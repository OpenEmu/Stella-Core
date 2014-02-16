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
// $Id: ProgressDialog.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "Widget.hxx"
#include "Dialog.hxx"
#include "DialogContainer.hxx"
#include "ProgressDialog.hxx"

#include "bspf.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgressDialog::ProgressDialog(GuiObject* boss, const GUI::Font& font,
                               const string& message)
  : Dialog(&boss->instance(), &boss->parent(), 0, 0, 16, 16),
    myMessage(NULL),
    mySlider(NULL),
    myStart(0),
    myFinish(0),
    myStep(0)
{
  const int fontWidth  = font.getMaxCharWidth(),
            fontHeight = font.getFontHeight(),
            lineHeight = font.getLineHeight();
  int xpos, ypos, lwidth;

  // Calculate real dimensions
  lwidth = font.getStringWidth(message);
  _w = lwidth + 2 * fontWidth;
  _h = lineHeight * 5;

  xpos = fontWidth; ypos = lineHeight;
  myMessage = new StaticTextWidget(this, font, xpos, ypos, lwidth, fontHeight,
                                   message, kTextAlignCenter);
  myMessage->setTextColor(kTextColorEm);

  xpos = fontWidth; ypos += 2 * lineHeight;
  mySlider = new SliderWidget(this, font, xpos, ypos, lwidth, lineHeight, "", 0, 0);
  mySlider->setMinValue(1);
  mySlider->setMaxValue(100);

  open();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgressDialog::~ProgressDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgressDialog::setMessage(const string& message)
{
  myMessage->setLabel(message);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgressDialog::setRange(int start, int finish, int step)
{
  myStart = start;
  myFinish = finish;
  myStep = (int)((step / 100.0) * (myFinish - myStart + 1));

  mySlider->setMinValue(myStart);
  mySlider->setMaxValue(myFinish);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgressDialog::setProgress(int progress)
{
  // Only increase the progress bar if we have arrived at a new step
  if(progress - mySlider->getValue() > myStep)
  {
    mySlider->setValue(progress);

    // Since this dialog is usually called in a tight loop that doesn't
    // yield, we need to manually tell the framebuffer that a redraw is
    // necessary
    // This isn't really an ideal solution, since all redrawing and
    // event handling is suspended until the dialog is closed
    instance().frameBuffer().update();
  }
}
