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
// $Id: ContextMenu.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "Dialog.hxx"
#include "DialogContainer.hxx"
#include "ScrollBarWidget.hxx"
#include "ContextMenu.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ContextMenu::ContextMenu(GuiObject* boss, const GUI::Font& font,
                         const VariantList& items, int cmd)
  : Dialog(&boss->instance(), &boss->parent(), 0, 0, 16, 16),
    CommandSender(boss),
    _rowHeight(font.getLineHeight()),
    _firstEntry(0),
    _numEntries(0),
    _selectedOffset(0),
    _selectedItem(-1),
    _showScroll(false),
    _isScrolling(false),
    _scrollUpColor(kColor),
    _scrollDnColor(kColor),
    _font(font),
    _cmd(cmd),
    _xorig(0),
    _yorig(0)
{
  addItems(items);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ContextMenu::~ContextMenu()
{
  _entries.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::addItems(const VariantList& items)
{
  _entries.clear();
  _entries = items;

  // Resize to largest string
  int maxwidth = 0;
  for(unsigned int i = 0; i < _entries.size(); ++i)
  {
    int length = _font.getStringWidth(_entries[i].first);
    if(length > maxwidth)
      maxwidth = length;
  }

  _x = _y = 0;
  _w = maxwidth + 10;
  _h = 1;  // recalculate this in ::recalc()

  _scrollUpColor = _firstEntry > 0 ? kScrollColor : kColor;
  _scrollDnColor = (_firstEntry + _numEntries < (int)_entries.size()) ?
      kScrollColor : kColor;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::show(uInt32 x, uInt32 y, int item)
{
  _xorig = x;
  _yorig = y;

  recalc(instance().frameBuffer().imageRect());
  open();
  setSelectedIndex(item);
  moveToSelected();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::center()
{
  // Make sure the menu is exactly where it should be, in case the image
  // offset has changed
  const GUI::Rect& image = instance().frameBuffer().imageRect();
  recalc(image);
  uInt32 x = image.x() + _xorig;
  uInt32 y = image.y() + _yorig;
  uInt32 tx = image.x() + image.width();
  uInt32 ty = image.y() + image.height();
  if(x + _w > tx) x -= (x + _w - tx);
  if(y + _h > ty) y -= (y + _h - ty);

  surface().setPos(x, y);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::recalc(const GUI::Rect& image)
{
  // Now is the time to adjust the height
  // If it's higher than the screen, we need to scroll through
  int maxentries = BSPF_min(18, (image.height() - 4) / _rowHeight);
  if((int)_entries.size() > maxentries)
  {
    // We show two less than the max, so we have room for two scroll buttons
    _numEntries = maxentries - 2;
    _h = maxentries * _rowHeight + 4;
    _showScroll = true;
  }
  else
  {
    _numEntries = _entries.size();
    _h = _entries.size() * _rowHeight + 4;
    _showScroll = false;
  }
  _isScrolling = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::setSelectedIndex(int idx)
{
  if(idx >= 0 && idx < (int)_entries.size())
    _selectedItem = idx;
  else
    _selectedItem = -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::setSelected(const Variant& tag, const Variant& defaultTag)
{
  if(tag != "")  // indicates that the defaultTag should be used instead
  {
    for(unsigned int item = 0; item < _entries.size(); ++item)
    { 
      if(BSPF_equalsIgnoreCase(_entries[item].second.toString(), tag.toString()))
      {
        setSelectedIndex(item);
        return;
      }
    }
  }

  // If we get this far, the value wasn't found; use the default value
  for(unsigned int item = 0; item < _entries.size(); ++item)
  {
    if(BSPF_equalsIgnoreCase(_entries[item].second.toString(), defaultTag.toString()))
    {
      setSelectedIndex(item);
      return;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::setSelectedMax()
{
  setSelectedIndex(_entries.size() - 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::clearSelection()
{
  _selectedItem = -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int ContextMenu::getSelected() const
{
  return _selectedItem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& ContextMenu::getSelectedName() const
{
  return (_selectedItem >= 0) ? _entries[_selectedItem].first : EmptyString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Variant& ContextMenu::getSelectedTag() const
{
  return (_selectedItem >= 0) ? _entries[_selectedItem].second : EmptyVariant;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::sendSelectionUp()
{
  if(isVisible() || _selectedItem <= 0)
    return false;

  _selectedItem--;
  sendCommand(_cmd ? _cmd : ContextMenu::kItemSelectedCmd, _selectedItem, -1);
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::sendSelectionDown()
{
  if(isVisible() || _selectedItem >= (int)_entries.size() - 1)
    return false;

  _selectedItem++;
  sendCommand(_cmd ? _cmd : ContextMenu::kItemSelectedCmd, _selectedItem, -1);
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::sendSelectionFirst()
{
  if(isVisible())
    return false;

  _selectedItem = 0;
  sendCommand(_cmd ? _cmd : ContextMenu::kItemSelectedCmd, _selectedItem, -1);
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::sendSelectionLast()
{
  if(isVisible())
    return false;

  _selectedItem = _entries.size() - 1;
  sendCommand(_cmd ? _cmd : ContextMenu::kItemSelectedCmd, _selectedItem, -1);
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleMouseDown(int x, int y, int button, int clickCount)
{
  // Compute over which item the mouse is...
  int item = findItem(x, y);

  // Only do a selection when the left button is in the dialog
  if(button == 1)
  {
    if(item != -1)
    {
      _isScrolling = _showScroll && ((item == 0) || (item == _numEntries+1));
      sendSelection();
    }
    else
      close();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleMouseMoved(int x, int y, int button)
{
  // Compute over which item the mouse is...
  int item = findItem(x, y);
  if(item == -1)
    return;

  // ...and update the selection accordingly
  drawCurrentSelection(item);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::handleMouseClicks(int x, int y, int button)
{
  // Let continuous mouse clicks come through, as the scroll buttons need them
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleMouseWheel(int x, int y, int direction)
{
  // Wheel events are only relevant in scroll mode
  if(_showScroll)
  {
    if(direction < 0)
      scrollUp(ScrollBarWidget::getWheelLines());
    else if(direction > 0)
      scrollDown(ScrollBarWidget::getWheelLines());
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleKeyDown(StellaKey key, StellaMod mod, char ascii)
{
  handleEvent(instance().eventHandler().eventForKey(key, kMenuMode));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleJoyDown(int stick, int button)
{
  handleEvent(instance().eventHandler().eventForJoyButton(stick, button, kMenuMode));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleJoyAxis(int stick, int axis, int value)
{
  if(value != 0)  // we don't care about 'axis up' events
    handleEvent(instance().eventHandler().eventForJoyAxis(stick, axis, value, kMenuMode));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool ContextMenu::handleJoyHat(int stick, int hat, int value)
{
  handleEvent(instance().eventHandler().eventForJoyHat(stick, hat, value, kMenuMode));
  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::handleEvent(Event::Type e)
{
  switch(e)
  {
    case Event::UISelect:
      sendSelection();
      break;
    case Event::UIUp:
    case Event::UILeft:
      moveUp();
      break;
    case Event::UIDown:
    case Event::UIRight:
      moveDown();
      break;
    case Event::UIPgUp:
      movePgUp();
      break;
    case Event::UIPgDown:
      movePgDown();
      break;
    case Event::UIHome:
      moveToFirst();
      break;
    case Event::UIEnd:
      moveToLast();
      break;
    case Event::UICancel:
      close();
      break;
    default:
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int ContextMenu::findItem(int x, int y) const
{
  if(x >= 0 && x < _w && y >= 0 && y < _h)
    return (y - 4) / _rowHeight;

  return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::drawCurrentSelection(int item)
{
  // Change selection
  _selectedOffset = item;
  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::sendSelection()
{
  // Select the correct item when scrolling; we have to take into account
  // that the viewable items are no longer 1-to-1 with the entries
  int item = _firstEntry + _selectedOffset;

  if(_showScroll)
  {
    if(_selectedOffset == 0)  // scroll up
      return scrollUp();
    else if(_selectedOffset == _numEntries+1) // scroll down
      return scrollDown();
    else if(_isScrolling)
      return;
    else
      item--;
  }

  // We remove the dialog when the user has selected an item
  // Make sure the dialog is removed before sending any commands,
  // since one consequence of sending a command may be to add another
  // dialog/menu
  close();

  // Send any command associated with the selection
  _selectedItem = item;
  sendCommand(_cmd ? _cmd : ContextMenu::kItemSelectedCmd, _selectedItem, -1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::moveUp()
{
  if(_showScroll)
  {
    // Reaching the top of the list means we have to scroll up, but keep the
    // current item offset
    // Otherwise, the offset should decrease by 1
    if(_selectedOffset == 1)
      scrollUp();
    else if(_selectedOffset > 1)
      drawCurrentSelection(_selectedOffset-1);
  }
  else
  {
    if(_selectedOffset > 0)
      drawCurrentSelection(_selectedOffset-1);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::moveDown()
{
  if(_showScroll)
  {
    // Reaching the bottom of the list means we have to scroll down, but keep the
    // current item offset
    // Otherwise, the offset should increase by 1
    if(_selectedOffset == _numEntries)
      scrollDown();
    else if(_selectedOffset < (int)_entries.size())
      drawCurrentSelection(_selectedOffset+1);    
  }
  else
  {
    if(_selectedOffset < (int)_entries.size() - 1)
      drawCurrentSelection(_selectedOffset+1);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::movePgUp()
{
  if(_firstEntry == 0)
    moveToFirst();
  else
    scrollUp(_numEntries);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::movePgDown()
{
  if(_firstEntry == (int)(_entries.size() - _numEntries))
    moveToLast();
  else
    scrollDown(_numEntries);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::moveToFirst()
{
  _firstEntry = 0;
  _scrollUpColor = kColor;
  _scrollDnColor = kScrollColor;

  drawCurrentSelection(_firstEntry + (_showScroll ? 1 : 0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::moveToLast()
{
  _firstEntry = _entries.size() - _numEntries;
  _scrollUpColor = kScrollColor;
  _scrollDnColor = kColor;

  drawCurrentSelection(_numEntries - (_showScroll ? 0 : 1));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::moveToSelected()
{
  if(_selectedItem < 0 || _selectedItem >= (int)_entries.size())
    return;

  // First jump immediately to the item
  _firstEntry = _selectedItem;
  int offset = 0;

  // Now check if we've gone past the current 'window' size, and scale
  // back accordingly
  int max_offset = _entries.size() - _numEntries;
  if(_firstEntry > max_offset)
  {
    offset = _firstEntry - max_offset;
    _firstEntry -= offset;
  }

  _scrollUpColor = _firstEntry > 0 ? kScrollColor : kColor;
  _scrollDnColor = _firstEntry < max_offset ? kScrollColor : kColor;

  drawCurrentSelection(offset + (_showScroll ? 1 : 0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::scrollUp(int distance)
{
  if(_firstEntry == 0)
    return;

  _firstEntry = BSPF_max(_firstEntry - distance, 0);
  _scrollUpColor = _firstEntry > 0 ? kScrollColor : kColor;
  _scrollDnColor = kScrollColor;

  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::scrollDown(int distance)
{
  int max_offset = _entries.size() - _numEntries;
  if(_firstEntry == max_offset)
    return;

  _firstEntry = BSPF_min(_firstEntry + distance, max_offset);
  _scrollUpColor = kScrollColor;
  _scrollDnColor = _firstEntry < max_offset ? kScrollColor : kColor;

  setDirty();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ContextMenu::drawDialog()
{
  static uInt32 up_arrow[8] = {
    0x00011000,
    0x00011000,
    0x00111100,
    0x00111100,
    0x01111110,
    0x01111110,
    0x11111111,
    0x11111111
  };
  static uInt32 down_arrow[8] = {
    0x11111111,
    0x11111111,
    0x01111110,
    0x01111110,
    0x00111100,
    0x00111100,
    0x00011000,
    0x00011000
  };

  // Normally we add widgets and let Dialog::draw() take care of this
  // logic.  But for some reason, this Dialog was written differently
  // by the ScummVM guys, so I'm not going to mess with it.
  FBSurface& s = surface();

  if(_dirty)
  {
    // Draw menu border and background
    s.fillRect(_x+1, _y+1, _w-2, _h-2, kWidColor);
    s.box(_x, _y, _w, _h, kColor, kShadowColor);

    // Draw the entries, taking scroll buttons into account
    int x = _x + 2, y = _y + 2, w = _w - 4;

    // Show top scroll area
    int offset = _selectedOffset;
    if(_showScroll)
    {
      s.hLine(x, y+_rowHeight-1, w+2, kShadowColor);
      s.drawBitmap(up_arrow, ((_w-_x)>>1)-4, (_rowHeight>>1)+y-4, _scrollUpColor, 8);
      y += _rowHeight;
      offset--;
    }

    for(int i = _firstEntry, current = 0; i < _firstEntry + _numEntries; ++i, ++current)
    {
      bool hilite = offset == current;
      if(hilite) s.fillRect(x, y, w, _rowHeight, kTextColorHi);
      s.drawString(_font, _entries[i].first, x + 1, y + 2, w,
                   !hilite ? kTextColor : kWidColor);
      y += _rowHeight;
    }

    // Show bottom scroll area
    if(_showScroll)
    {
      s.hLine(x, y, w+2, kShadowColor);
      s.drawBitmap(down_arrow, ((_w-_x)>>1)-4, (_rowHeight>>1)+y-4, _scrollDnColor, 8);
    }

    s.addDirtyRect(_x, _y, _w, _h);
    _dirty = false;
  }

  // Commit surface changes to screen
  s.update();
}
