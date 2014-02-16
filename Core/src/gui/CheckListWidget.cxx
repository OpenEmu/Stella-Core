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
// $Id: CheckListWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "CheckListWidget.hxx"
#include "Widget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CheckListWidget::CheckListWidget(GuiObject* boss, const GUI::Font& font,
                                 int x, int y, int w, int h)
  : ListWidget(boss, font, x, y, w, h, false)  // disable quick select
{
  int ypos = _y + 2;

  // rowheight is determined by largest item on a line,
  // possibly meaning that number of rows will change
  _fontHeight = BSPF_max(_fontHeight, CheckboxWidget::boxSize());
  _rows = h / _fontHeight;

  // Create a CheckboxWidget for each row in the list
  CheckboxWidget* t;
  for(int i = 0; i < _rows; ++i)
  {
    t = new CheckboxWidget(boss, font, _x + 2, ypos, "", kCheckActionCmd);
    t->setTextColor(kTextColor);
    t->setTarget(this);
    t->setID(i);
    ypos += _fontHeight;

    _checkList.push_back(t);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CheckListWidget::~CheckListWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CheckListWidget::setList(const StringList& list, const BoolArray& state)
{
  _list = list;
  _stateList = state;

  assert(_list.size() == _stateList.size());

  // Enable all checkboxes
  for(int i = 0; i < _rows; ++i)
    _checkList[i]->setFlags(WIDGET_ENABLED);

  // Then turn off any extras
  if((int)_stateList.size() < _rows)
    for(int i = _stateList.size(); i < _rows; ++i)
      _checkList[i]->clearFlags(WIDGET_ENABLED);

  ListWidget::recalc();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CheckListWidget::setLine(int line, const string& str, const bool& state)
{
  if(line >= (int)_list.size())
    return;

  _list[line]      = str;
  _stateList[line] = state;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CheckListWidget::drawWidget(bool hilite)
{
//cerr << "CheckListWidget::drawWidget\n";
  FBSurface& s = _boss->dialog().surface();
  int i, pos, len = _list.size();
  string buffer;
  int deltax;

  // Draw a thin frame around the list and to separate columns
  s.hLine(_x, _y, _x + _w - 1, kColor);
  s.hLine(_x, _y + _h - 1, _x + _w - 1, kShadowColor);
  s.vLine(_x, _y, _y + _h - 1, kColor);

  s.vLine(_x + CheckboxWidget::boxSize() + 5, _y, _y + _h - 1, kColor);

  // Draw the list items
  for (i = 0, pos = _currentPos; i < _rows && pos < len; i++, pos++)
  {
    // Draw checkboxes for correct lines (takes scrolling into account)
    _checkList[i]->setState(_stateList[pos]);
    _checkList[i]->setDirty();
    _checkList[i]->draw();

    const int y = _y + 2 + _fontHeight * i + 2;

    GUI::Rect r(getEditRect());

    // Draw the selected item inverted, on a highlighted background.
    if (_selectedItem == pos)
    {
      if (_hasFocus && !_editMode)
        s.fillRect(_x + r.left - 3, _y + 1 + _fontHeight * i,
                   _w - r.left, _fontHeight, kTextColorHi);
      else
        s.frameRect(_x + r.left - 3, _y + 1 + _fontHeight * i,
                    _w - r.left, _fontHeight, kTextColorHi);
    }

    if (_selectedItem == pos && _editMode)
    {
      buffer = _editString;
      adjustOffset();
      deltax = -_editScrollOffset;

      s.drawString(_font, buffer, _x + r.left, y, r.width(), kTextColor,
                   kTextAlignLeft, deltax, false);
    }
    else
    {
      buffer = _list[pos];
      deltax = 0;
      s.drawString(_font, buffer, _x + r.left, y, r.width(), kTextColor);
    }
  }

  // Only draw the caret while editing, and if it's in the current viewport
  if(_editMode && (_selectedItem >= _scrollBar->_currentPos) &&
    (_selectedItem < _scrollBar->_currentPos + _rows))
    drawCaret();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GUI::Rect CheckListWidget::getEditRect() const
{
  GUI::Rect r(2, 1, _w, _fontHeight);
  const int yoffset = (_selectedItem - _currentPos) * _fontHeight,
            xoffset = CheckboxWidget::boxSize() + 10;
  r.top    += yoffset;
  r.bottom += yoffset;
  r.left  += xoffset;
  r.right -= xoffset - 15;
	
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CheckListWidget::getState(int line)
{
  if(line >= 0 && line < (int)_stateList.size())
    return _stateList[line];
  else
    return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CheckListWidget::handleEvent(Event::Type e)
{
  switch(e)
  {
    case Event::UISelect:
      // Simulate a mouse button click
      _checkList[ListWidget::getSelected()]->handleMouseUp(0, 0, 1, 0);
      return true;
      break;

    default:
      return ListWidget::handleEvent(e);
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CheckListWidget::handleCommand(CommandSender* sender, int cmd,
                                    int data, int id)
{
  switch(cmd)
  {
    case kCheckActionCmd:
    {
      // Figure out which line has been checked
      int line = _currentPos + id;
      _stateList[line] = bool(data);

      // Let the boss know about it
      sendCommand(kListItemChecked, line, _id);
      break;
    }

    default:
      ListWidget::handleCommand(sender, cmd, data, id);
      break;
  }
}
