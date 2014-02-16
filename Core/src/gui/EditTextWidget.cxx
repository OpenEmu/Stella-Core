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
// $Id: EditTextWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include <sstream>

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "Dialog.hxx"
#include "EditTextWidget.hxx"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EditTextWidget::EditTextWidget(GuiObject* boss, const GUI::Font& font,
                               int x, int y, int w, int h, const string& text)
  : EditableWidget(boss, font, x, y, w, h + 2, text),
    _editable(true),
    _changed(false)
{
  _flags = WIDGET_ENABLED | WIDGET_CLEARBG | WIDGET_RETAIN_FOCUS;

  startEditMode();  // We're always in edit mode
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::setText(const string& str, bool changed)
{
  EditableWidget::setText(str, changed);
  _backupString = str;
  _changed = changed;

  setDirty(); draw();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::handleMouseDown(int x, int y, int button, int clickCount)
{
  if(!_editable)
    return;

  x += _editScrollOffset;

  int width = 0;
  unsigned int i;

  for (i = 0; i < _editString.size(); ++i)
  {
    width += _font.getCharWidth(_editString[i]);
    if (width >= x)
      break;
  }

  if (setCaretPos(i))
  {
    setDirty(); draw();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::drawWidget(bool hilite)
{
//cerr << "EditTextWidget::drawWidget\n";
  FBSurface& s = _boss->dialog().surface();

  // Highlight changes
  if(_changed)
    s.fillRect(_x, _y, _w, _h, kDbgChangedColor);

  // Draw a thin frame around us.
  s.hLine(_x, _y, _x + _w - 1, kColor);
  s.hLine(_x, _y + _h - 1, _x +_w - 1, kShadowColor);
  s.vLine(_x, _y, _y + _h - 1, kColor);
  s.vLine(_x + _w - 1, _y, _y + _h - 1, kShadowColor);

  // Draw the text
  adjustOffset();
  s.drawString(_font, _editString, _x + 2, _y + 2, getEditRect().width(),
               !_changed ? _textcolor : kDbgChangedTextColor,
               kTextAlignLeft, -_editScrollOffset, false);

  // Draw the caret 
  drawCaret();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GUI::Rect EditTextWidget::getEditRect() const
{
  GUI::Rect r(2, 1, _w - 2, _h);
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::lostFocusWidget()
{
  // If we loose focus, 'commit' the user changes
  _backupString = _editString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::startEditMode()
{
  EditableWidget::startEditMode();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::endEditMode()
{
  // Editing is always enabled
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EditTextWidget::abortEditMode()
{
  // Editing is always enabled
  setText(_backupString);
}
