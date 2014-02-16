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
// $Id: ToggleBitWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "Widget.hxx"
#include "Dialog.hxx"
#include "Debugger.hxx"
#include "FrameBuffer.hxx"
#include "StringList.hxx"
#include "ToggleBitWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ToggleBitWidget::ToggleBitWidget(GuiObject* boss, const GUI::Font& font,
                                 int x, int y, int cols, int rows, int colchars)
  : ToggleWidget(boss, font, x, y, cols, rows)
{
  _rowHeight = font.getLineHeight();
  _colWidth  = colchars * font.getMaxCharWidth() + 8;

  // Make sure all lists contain some default values
  int size = _rows * _cols;
  while(size--)
  {
    _offList.push_back("0");
    _onList.push_back("1");
    _stateList.push_back(false);
    _changedList.push_back(false);
  }

  // Calculate real dimensions
  _w = _colWidth  * cols + 1;
  _h = _rowHeight * rows + 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ToggleBitWidget::~ToggleBitWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleBitWidget::setList(const StringList& off, const StringList& on)
{
  _offList.clear();
  _offList = off;
  _onList.clear();
  _onList = on;

  int size = _offList.size();  // assume _onList is the same size
  assert(size == _rows * _cols);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleBitWidget::setState(const BoolArray& state, const BoolArray& changed)
{
  _stateList.clear();
  _stateList = state;
  _changedList.clear();
  _changedList = changed;

  setDirty(); draw();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ToggleBitWidget::drawWidget(bool hilite)
{
//cerr << "ToggleBitWidget::drawWidget\n";
  FBSurface& s = dialog().surface();
  int row, col;
  string buffer;

  // Draw the internal grid and labels
  int linewidth = _cols * _colWidth;
  for (row = 0; row <= _rows; row++)
    s.hLine(_x, _y + (row * _rowHeight), _x + linewidth, kColor);
  int lineheight = _rows * _rowHeight;
  for (col = 0; col <= _cols; col++)
    s.vLine(_x + (col * _colWidth), _y, _y + lineheight, kColor);

  // Draw the list items
  for (row = 0; row < _rows; row++)
  {
    for (col = 0; col < _cols; col++)
    {
      int x = _x + 4 + (col * _colWidth);
      int y = _y + 2 + (row * _rowHeight);
      int pos = row*_cols + col;

      // Draw the selected item inverted, on a highlighted background.
      if (_currentRow == row && _currentCol == col && _hasFocus)
        s.fillRect(x - 4, y - 2, _colWidth+1, _rowHeight+1, kTextColorHi);

      if(_stateList[pos])
        buffer = _onList[pos];
      else
        buffer = _offList[pos];

      // Highlight changes
      if(_changedList[pos])
      {
        s.fillRect(x - 3, y - 1, _colWidth-1, _rowHeight-1, kDbgChangedColor);
        s.drawString(_font, buffer, x, y, _colWidth, kDbgChangedTextColor);
      }
      else
        s.drawString(_font, buffer, x, y, _colWidth, kTextColor);
    }
  }
}
