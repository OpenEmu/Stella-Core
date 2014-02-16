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
// $Id: TogglePixelWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "Widget.hxx"
#include "Dialog.hxx"
#include "Debugger.hxx"
#include "FrameBuffer.hxx"
#include "TogglePixelWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TogglePixelWidget::TogglePixelWidget(GuiObject* boss, const GUI::Font& font,
                                     int x, int y, int cols, int rows)
  : ToggleWidget(boss, font, x, y, cols, rows),
    _pixelColor(0),
    _backgroundColor(kDlgColor)
{
  _rowHeight = _colWidth = font.getLineHeight();

  // Calculate real dimensions
  _w = _colWidth  * cols + 1;
  _h = _rowHeight * rows + 1;

  // Changed state isn't used, but we still need to fill it
  while((int)_changedList.size() < rows * cols)
    _changedList.push_back(false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TogglePixelWidget::~TogglePixelWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TogglePixelWidget::setState(const BoolArray& state)
{
  _stateList.clear();
  _stateList = state;

  setDirty(); draw();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TogglePixelWidget::setIntState(int value, bool swap)
{
  unsigned int size = _rows * _cols;
  _swapBits = swap;

  // Create array of required size
  BoolArray b;
  while(b.size() < size)
    b.push_back(false);

  // Bits in an int increase from right to left, but a BoolArray
  // is scanned from left to right.
  //
  //   Swap off means treat the above as normal (ie, contruct the
  //   BoolArray as we read the int from right to left).
  //
  //   Swap on means reverse of swap off!  Sorry if this is
  //   confusing.
  for(unsigned int i = 0; i < size; ++i)
  {
    bool bitIsSet = value & (1 << i);
    if(_swapBits)
      b[i] = bitIsSet;
    else
      b[size-i-1] = bitIsSet;
  }

  setState(b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int TogglePixelWidget::getIntState()
{
  // Construct int based on current state and swap
  unsigned int value = 0, size = _stateList.size();

  for(unsigned int i = 0; i < size; ++i)
  {
    if(_stateList[i])
    {
      if(_swapBits)
        value |= 1 << i;
      else
        value |= 1 << (size-i-1);
    }
  }

  return value;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TogglePixelWidget::drawWidget(bool hilite)
{
//cerr << "TogglePixelWidget::drawWidget\n";
  FBSurface& s = dialog().surface();
  int row, col;

  // Draw the internal grid and labels
  int linewidth = _cols * _colWidth;
  for (row = 0; row <= _rows; row++)
    s.hLine(_x, _y + (row * _rowHeight), _x + linewidth, kColor);
  int lineheight = _rows * _rowHeight;
  for (col = 0; col <= _cols; col++)
    s.vLine(_x + (col * _colWidth), _y, _y + lineheight, kColor);

  // Draw the pixels
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

      // Either draw the pixel in given color, or erase (show background)
      s.fillRect(x - 3, y - 1, _colWidth-1, _rowHeight-1,
                 _stateList[pos] ? _pixelColor : _backgroundColor);
    }
  }
}
