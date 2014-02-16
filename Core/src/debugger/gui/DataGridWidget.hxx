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
// $Id: DataGridWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef DATA_GRID_WIDGET_HXX
#define DATA_GRID_WIDGET_HXX

class DataGridOpsWidget;
class ScrollBarWidget;

#include "Widget.hxx"
#include "Command.hxx"
#include "Debugger.hxx"
#include "EditableWidget.hxx"
#include "Array.hxx"
#include "Base.hxx"
#include "Rect.hxx"

/* DataGridWidget */
class DataGridWidget : public EditableWidget
{
  public:
    // Commands emitted by this commandsender
    enum {
      kItemDoubleClickedCmd = 'DGdb',
      kItemActivatedCmd     = 'DGac',
      kItemDataChangedCmd   = 'DGch',
      kSelectionChangedCmd  = 'DGsc'
    };

  public:
    DataGridWidget(GuiObject* boss, const GUI::Font& font,
                   int x, int y, int cols, int rows,
                   int colchars, int bits,
                   Common::Base::Format format = Common::Base::F_DEFAULT,
                   bool useScrollbar = false);
    virtual ~DataGridWidget();

    void setList(const IntArray& alist, const IntArray& vlist,
                 const BoolArray& changed);
    /** Convenience method for when the datagrid contains only one value */
    void setList(int a, int v, bool changed);
    void setList(int a, int v); // automatically calculate if changed

    void setHiliteList(const BoolArray& hilitelist);
    void setNumRows(int rows);

    /** Set value at current selection point */
    void setSelectedValue(int value);
    /** Set value at given position */
    void setValue(int position, int value);
    /** Set value at given position, manually specifying if the value changed */
    void setValue(int position, int value, bool changed);

    int getSelectedAddr() const   { return _addrList[_selectedItem]; }
    int getSelectedValue() const  { return _valueList[_selectedItem]; }

    void setRange(int lower, int upper);

    virtual void handleMouseDown(int x, int y, int button, int clickCount);
    virtual void handleMouseUp(int x, int y, int button, int clickCount);
    virtual void handleMouseWheel(int x, int y, int direction);
    virtual bool handleKeyDown(StellaKey key, StellaMod mod, char ascii);
    virtual bool handleKeyUp(StellaKey key, StellaMod mod, char ascii);
    virtual void handleCommand(CommandSender* sender, int cmd, int data, int id);

    virtual bool wantsFocus() { return true; }

    // Account for the extra width of embedded scrollbar
    virtual int getWidth() const;

    void startEditMode();
    void endEditMode();

    int colWidth() { return _colWidth; }

    void setOpsWidget(DataGridOpsWidget* w) { _opsWidget = w; }

  protected:
    void drawWidget(bool hilite);

    int findItem(int x, int y);

    void abortEditMode();

    GUI::Rect getEditRect() const;

    void receivedFocusWidget();
    void lostFocusWidget();

    bool tryInsertChar(char c, int pos);

  protected:
    int  _rows;
    int  _cols;
    int  _currentRow;
    int  _currentCol;
    int  _rowHeight;
    int  _colWidth;
    int  _bits;
    int  _lowerBound;
    int  _upperBound;

    Common::Base::Format _base;

    IntArray    _addrList;
    IntArray    _valueList;
    StringList  _valueStringList;
    BoolArray   _changedList;
    BoolArray   _hiliteList;

    bool      _editMode;
    int       _selectedItem;
    StellaKey _currentKeyDown;
    string    _backupString;

    DataGridOpsWidget* _opsWidget;
    ScrollBarWidget* _scrollBar;

  private:
    /** Common operations on the currently selected cell */
    void negateCell();
    void invertCell();
    void decrementCell();
    void incrementCell();
    void lshiftCell();
    void rshiftCell();
    void zeroCell();
};

#endif
