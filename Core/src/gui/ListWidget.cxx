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
// $Id: ListWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include <cctype>
#include <algorithm>

#include "OSystem.hxx"
#include "Widget.hxx"
#include "ScrollBarWidget.hxx"
#include "Dialog.hxx"
#include "FrameBuffer.hxx"
#include "ListWidget.hxx"
#include "bspf.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ListWidget::ListWidget(GuiObject* boss, const GUI::Font& font,
                       int x, int y, int w, int h, bool quickSelect)
  : EditableWidget(boss, font, x, y, 16, 16),
    _rows(0),
    _cols(0),
    _currentPos(0),
    _selectedItem(-1),
    _highlightedItem(-1),
    _editMode(false),
    _currentKeyDown(KBDK_UNKNOWN),
    _quickSelect(quickSelect),
    _quickSelectTime(0)
{
  _flags = WIDGET_ENABLED | WIDGET_CLEARBG | WIDGET_RETAIN_FOCUS;
  _bgcolor = kWidColor;
  _bgcolorhi = kWidColor;
  _textcolor = kTextColor;
  _textcolorhi = kTextColor;

  _cols = w / _fontWidth;
  _rows = h / _fontHeight;

  // Set real dimensions
  _w = w - kScrollBarWidth;
  _h = h + 2;

  // Create scrollbar and attach to the list
  _scrollBar = new ScrollBarWidget(boss, font, _x + _w, _y, kScrollBarWidth, _h);
  _scrollBar->setTarget(this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ListWidget::~ListWidget()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::setSelected(int item)
{
  if(item < -1 || item >= (int)_list.size())
    return;

  if(isEnabled())
  {
    if(_editMode)
      abortEditMode();

    _selectedItem = item;
    sendCommand(ListWidget::kSelectionChangedCmd, _selectedItem, _id);

    _currentPos = _selectedItem - _rows / 2;
    scrollToSelected();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::setSelected(const string& item)
{
  int selected = -1;
  if(!_list.isEmpty())
  {
    if(item == "")
      selected = 0;
    else
    {
      uInt32 itemToSelect = 0;
      StringList::const_iterator iter;
      for(iter = _list.begin(); iter != _list.end(); ++iter, ++itemToSelect)
      {
        if(item == *iter)
        {
          selected = itemToSelect;
          break;
        }
      }
      if(itemToSelect > _list.size() || selected == -1)
        selected = 0;
    }
  }
  setSelected(selected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::setHighlighted(int item)
{
  if(item < -1 || item >= (int)_list.size())
    return;

  if(isEnabled())
  {
    if(_editMode)
      abortEditMode();

    _highlightedItem = item;

    // Only scroll the list if we're about to pass the page boundary
    if(_currentPos == 0)
      _currentPos = _highlightedItem;
    else if(_highlightedItem == _currentPos + _rows)
      _currentPos += _rows;

    scrollToHighlighted();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& ListWidget::getSelectedString() const
{
  return (_selectedItem >= 0 && _selectedItem < (int)_list.size())
            ? _list[_selectedItem] : EmptyString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::scrollTo(int item)
{
  int size = _list.size();
  if (item >= size)
    item = size - 1;
  if (item < 0)
    item = 0;

  if(_currentPos != item)
  {
    _currentPos = item;
    scrollBarRecalc();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::recalc()
{
  int size = _list.size();

  if (_currentPos >= size)
    _currentPos = size - 1;
  if (_currentPos < 0)
    _currentPos = 0;

  if(_selectedItem < 0 || _selectedItem >= size)
    _selectedItem = 0;

  _editMode = false;

  _scrollBar->_numEntries     = _list.size();
  _scrollBar->_entriesPerPage = _rows;

  // Reset to normal data entry
  abortEditMode();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::scrollBarRecalc()
{
  _scrollBar->_currentPos = _currentPos;
  _scrollBar->recalc();
  sendCommand(ListWidget::kScrolledCmd, _currentPos, _id);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::handleMouseDown(int x, int y, int button, int clickCount)
{
  if (!isEnabled())
    return;

  // First check whether the selection changed
  int newSelectedItem;
  newSelectedItem = findItem(x, y);
  if (newSelectedItem >= (int)_list.size())
    return;

  if (_selectedItem != newSelectedItem)
  {
    if (_editMode)
      abortEditMode();
    _selectedItem = newSelectedItem;
    sendCommand(ListWidget::kSelectionChangedCmd, _selectedItem, _id);
    setDirty(); draw();
  }
	
  // TODO: Determine where inside the string the user clicked and place the
  // caret accordingly. See _editScrollOffset and EditTextWidget::handleMouseDown.
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::handleMouseUp(int x, int y, int button, int clickCount)
{
  // If this was a double click and the mouse is still over the selected item,
  // send the double click command
  if (clickCount == 2 && (_selectedItem == findItem(x, y)))
  {
    sendCommand(ListWidget::kDoubleClickedCmd, _selectedItem, _id);

    // Start edit mode
    if(_editable && !_editMode)
      startEditMode();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::handleMouseWheel(int x, int y, int direction)
{
  _scrollBar->handleMouseWheel(x, y, direction);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int ListWidget::findItem(int x, int y) const
{
  return (y - 1) / _fontHeight + _currentPos;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ListWidget::handleKeyDown(StellaKey key, StellaMod mod, char ascii)
{
  // Ignore all Alt-mod keys
  if(instance().eventHandler().kbdAlt(mod))
    return true;

  bool handled = true;
  int oldSelectedItem = _selectedItem;

  if (!_editMode && _quickSelect &&
      ((isalnum(ascii)) || isspace(ascii)))
  {
    // Quick selection mode: Go to first list item starting with this key
    // (or a substring accumulated from the last couple key presses).
    // Only works in a useful fashion if the list entries are sorted.
    // TODO: Maybe this should be off by default, and instead we add a
    // method "enableQuickSelect()" or so ?
    uInt64 time = instance().getTicks() / 1000;
    if (_quickSelectTime < time)
      _quickSelectStr = ascii;
    else
      _quickSelectStr += ascii;
    _quickSelectTime = time + _QUICK_SELECT_DELAY;

    // FIXME: This is bad slow code (it scans the list linearly each time a
    // key is pressed); it could be much faster. Only of importance if we have
    // quite big lists to deal with -- so for now we can live with this lazy
    // implementation :-)
    int newSelectedItem = 0;
    for (StringList::const_iterator i = _list.begin(); i != _list.end(); ++i)
    {
      if(BSPF_startsWithIgnoreCase(*i, _quickSelectStr))
      {
        _selectedItem = newSelectedItem;
        break;
      }
      newSelectedItem++;
    }
  }
  else if (_editMode)
  {
    // Class EditableWidget handles all text editing related key presses for us
    handled = EditableWidget::handleKeyDown(key, mod, ascii);
  }
  else
  {
    // not editmode
    switch(key)
    {
      case KBDK_SPACE:
        // Snap list back to currently highlighted line
        if(_highlightedItem >= 0)
        {
          _currentPos = _highlightedItem;
          scrollToHighlighted();
        }
        break;

      default:
        handled = false;
    }
  }

  if (_selectedItem != oldSelectedItem)
  {
    _scrollBar->draw();
    scrollToSelected();

    sendCommand(ListWidget::kSelectionChangedCmd, _selectedItem, _id);
  }

  _currentKeyDown = key;
  return handled;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ListWidget::handleKeyUp(StellaKey key, StellaMod mod, char ascii)
{
  if (key == _currentKeyDown)
    _currentKeyDown = KBDK_UNKNOWN;
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ListWidget::handleEvent(Event::Type e)
{
  if(!isEnabled() || _editMode)
    return false;

  bool handled = true;
  int oldSelectedItem = _selectedItem;

  switch(e)
  {
    case Event::UISelect:
      if (_selectedItem >= 0)
      {
        if (_editable)
          startEditMode();
        else
          sendCommand(ListWidget::kActivatedCmd, _selectedItem, _id);
      }
      break;

    case Event::UIUp:
      if (_selectedItem > 0)
        _selectedItem--;
      break;

    case Event::UIDown:
      if (_selectedItem < (int)_list.size() - 1)
        _selectedItem++;
      break;

    case Event::UIPgUp:
      _selectedItem -= _rows - 1;
      if (_selectedItem < 0)
        _selectedItem = 0;
      break;

    case Event::UIPgDown:
      _selectedItem += _rows - 1;
      if (_selectedItem >= (int)_list.size() )
        _selectedItem = _list.size() - 1;
      break;

    case Event::UIHome:
      _selectedItem = 0;
      break;

    case Event::UIEnd:
      _selectedItem = _list.size() - 1;
      break;

    case Event::UIPrevDir:
      sendCommand(ListWidget::kPrevDirCmd, _selectedItem, _id);
      break;

    default:
      handled = false;
  }

  if (_selectedItem != oldSelectedItem)
  {
    _scrollBar->draw();
    scrollToSelected();

    sendCommand(ListWidget::kSelectionChangedCmd, _selectedItem, _id);
  }

  return handled;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::lostFocusWidget()
{
  _editMode = false;

  // Reset to normal data entry
  abortEditMode();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  switch (cmd)
  {
    case kSetPositionCmd:
      if (_currentPos != (int)data)
      {
        _currentPos = data;
        setDirty(); draw();

        // Let boss know the list has scrolled
        sendCommand(ListWidget::kScrolledCmd, _currentPos, _id);
      }
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::scrollToCurrent(int item)
{
  // Only do something if the current item is not in our view port
  if (item < _currentPos)
  {
    // it's above our view
    _currentPos = item;
  }
  else if (item >= _currentPos + _rows )
  {
    // it's below our view
    _currentPos = item - _rows + 1;
  }

  if (_currentPos < 0 || _rows > (int)_list.size())
    _currentPos = 0;
  else if (_currentPos + _rows > (int)_list.size())
    _currentPos = _list.size() - _rows;

  int oldScrollPos = _scrollBar->_currentPos;
  _scrollBar->_currentPos = _currentPos;
  _scrollBar->recalc();

  setDirty(); draw();

  if(oldScrollPos != _currentPos)
    sendCommand(ListWidget::kScrolledCmd, _currentPos, _id);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::startEditMode()
{
  if (_editable && !_editMode && _selectedItem >= 0)
  {
    _editMode = true;
    setText(_list[_selectedItem]);

    // Widget gets raw data while editing
    EditableWidget::startEditMode();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::endEditMode()
{
  if (!_editMode)
    return;

  // Send a message that editing finished with a return/enter key press
  _editMode = false;
  _list[_selectedItem] = _editString;
  sendCommand(ListWidget::kDataChangedCmd, _selectedItem, _id);

  // Reset to normal data entry
  EditableWidget::endEditMode();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ListWidget::abortEditMode()
{
  // Undo any changes made
  _editMode = false;

  // Reset to normal data entry
  EditableWidget::abortEditMode();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt64 ListWidget::_QUICK_SELECT_DELAY = 300;
