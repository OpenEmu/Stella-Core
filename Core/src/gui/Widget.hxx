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
// $Id: Widget.hxx 2838 2014-01-17 23:34:03Z stephena $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "Dialog.hxx"

#ifndef WIDGET_HXX
#define WIDGET_HXX

#include <cassert>

#include "bspf.hxx"

#include "Array.hxx"
#include "Event.hxx"
#include "Font.hxx"
#include "FrameBuffer.hxx"
#include "GuiObject.hxx"
#include "OSystem.hxx"
#include "Rect.hxx"
#include "StellaKeys.hxx"

enum {
  WIDGET_ENABLED       = 1 << 0,
  WIDGET_INVISIBLE     = 1 << 1,
  WIDGET_HILITED       = 1 << 2,
  WIDGET_BORDER        = 1 << 3,
  WIDGET_CLEARBG       = 1 << 4,
  WIDGET_TRACK_MOUSE   = 1 << 5,
  WIDGET_RETAIN_FOCUS  = 1 << 6,
  WIDGET_WANTS_TAB     = 1 << 7,
  WIDGET_WANTS_RAWDATA = 1 << 8
};

/**
  This is the base class for all widgets.
  
  @author  Stephen Anthony
  @version $Id: Widget.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class Widget : public GuiObject
{
  friend class Dialog;

  public:
    Widget(GuiObject* boss, const GUI::Font& font, int x, int y, int w, int h);
    virtual ~Widget();

    virtual int getAbsX() const  { return _x + _boss->getChildX(); }
    virtual int getAbsY() const  { return _y + _boss->getChildY(); }

    virtual bool handleKeyDown(StellaKey key, StellaMod mod, char ascii) { return false; }
    virtual bool handleKeyUp(StellaKey key, StellaMod mod, char ascii)   { return false; }
    virtual void handleMouseDown(int x, int y, int button, int clickCount) {}
    virtual void handleMouseUp(int x, int y, int button, int clickCount) {}
    virtual void handleMouseEntered(int button) {}
    virtual void handleMouseLeft(int button) {}
    virtual void handleMouseMoved(int x, int y, int button) {}
    virtual void handleMouseWheel(int x, int y, int direction) {}
    virtual bool handleMouseClicks(int x, int y, int button) { return false; }
    virtual void handleJoyDown(int stick, int button) {}
    virtual void handleJoyUp(int stick, int button) {}
    virtual void handleJoyAxis(int stick, int axis, int value) {}
    virtual bool handleJoyHat(int stick, int hat, int value) { return false; }
    virtual bool handleEvent(Event::Type event) { return false; }

    void draw();
    void receivedFocus();
    void lostFocus();
    void addFocusWidget(Widget* w) { _focusList.push_back(w); }

    /** Set/clear WIDGET_ENABLED flag and immediately redraw */
    void setEnabled(bool e);

    void setFlags(int flags)    { _flags |= flags;  }
    void clearFlags(int flags)  { _flags &= ~flags; }
    int  getFlags() const       { return _flags;    }

    bool isEnabled() const   { return _flags & WIDGET_ENABLED;         }
    bool isVisible() const   { return !(_flags & WIDGET_INVISIBLE);    }
    bool wantsFocus() const  { return _flags & WIDGET_RETAIN_FOCUS;    }
    bool wantsTab() const    { return _flags & WIDGET_WANTS_TAB;       }
    bool wantsRaw() const    { return _flags & WIDGET_WANTS_RAWDATA;   }

    void setID(int id)  { _id = id;   }
    int  getID() const  { return _id; }

    virtual const GUI::Font& font() const { return _font; }

    void setTextColor(uInt32 color)   { _textcolor = color;   }
    void setTextColorHi(uInt32 color) { _textcolorhi = color; }
    void setBGColor(uInt32 color)     { _bgcolor = color;     }
    void setBGColorHi(uInt32 color)   { _bgcolorhi = color;   }

    virtual void loadConfig() {}

  protected:
    virtual void drawWidget(bool hilite) {}

    virtual void receivedFocusWidget() {}
    virtual void lostFocusWidget() {}

    virtual Widget* findWidget(int x, int y) { return this; }

    void releaseFocus() { assert(_boss); _boss->releaseFocus(); }

    // By default, delegate unhandled commands to the boss
    void handleCommand(CommandSender* sender, int cmd, int data, int id)
         { assert(_boss); _boss->handleCommand(sender, cmd, data, id); }

  protected:
    GuiObject* _boss;
    const GUI::Font& _font;
    Widget*    _next;
    int        _id;
    int        _flags;
    bool       _hasFocus;
    int        _fontWidth;
    int        _fontHeight;
    uInt32     _bgcolor;
    uInt32     _bgcolorhi;
    uInt32     _textcolor;
    uInt32     _textcolorhi;

  public:
    static Widget* findWidgetInChain(Widget* start, int x, int y);

    /** Determine if 'find' is in the chain pointed to by 'start' */
    static bool isWidgetInChain(Widget* start, Widget* find);

    /** Determine if 'find' is in the widget array */
    static bool isWidgetInChain(WidgetArray& list, Widget* find);

    /** Select either previous, current, or next widget in chain to have
        focus, and deselects all others */
    static Widget* setFocusForChain(GuiObject* boss, WidgetArray& arr,
                                    Widget* w, int direction);

    /** Sets all widgets in this chain to be dirty (must be redrawn) */
    static void setDirtyInChain(Widget* start);
};


/* StaticTextWidget */
class StaticTextWidget : public Widget
{
  public:
    StaticTextWidget(GuiObject* boss, const GUI::Font& font,
                     int x, int y, int w, int h,
                     const string& text, TextAlignment align);
    void setValue(int value);
    void setLabel(const string& label);
    void setAlign(TextAlignment align)  { _align = align; }
    const string& getLabel() const      { return _label; }

  protected:
    void drawWidget(bool hilite);

  protected:
    string        _label;
    bool          _editable;
    TextAlignment _align;
};


/* ButtonWidget */
class ButtonWidget : public StaticTextWidget, public CommandSender
{
  public:
    ButtonWidget(GuiObject* boss, const GUI::Font& font,
                 int x, int y, int w, int h,
                 const string& label, int cmd = 0);

    void setCmd(int cmd)  { _cmd = cmd; }
    int getCmd() const    { return _cmd; }

    virtual void handleMouseUp(int x, int y, int button, int clickCount);
    virtual void handleMouseEntered(int button);
    virtual void handleMouseLeft(int button);
    virtual bool handleEvent(Event::Type event);

  protected:
    void drawWidget(bool hilite);

  protected:
    int    _cmd;
};


/* CheckboxWidget */
class CheckboxWidget : public ButtonWidget
{
  public:
    enum FillType {
      Normal, Inactive, Circle
    };

  public:
    CheckboxWidget(GuiObject* boss, const GUI::Font& font, int x, int y,
                   const string& label, int cmd = 0);

    void handleMouseUp(int x, int y, int button, int clickCount);
    virtual void handleMouseEntered(int button)	{}
    virtual void handleMouseLeft(int button)	{}

    void setEditable(bool editable);
    void setFill(FillType type);

    void setState(bool state);
    void toggleState()     { setState(!_state); }
    bool getState() const  { return _state;     }

    static int boxSize() { return 14; }  // box is square

  protected:
    void drawWidget(bool hilite);

  protected:
    bool _state;
    bool _holdFocus;
    bool _drawBox;

    uInt32* _img;
    uInt32  _fillColor;

  private:
    int _boxY;
    int _textY;
};


/* SliderWidget */
class SliderWidget : public ButtonWidget
{
  public:
    SliderWidget(GuiObject *boss, const GUI::Font& font,
                 int x, int y, int w, int h, const string& label = "",
                 int labelWidth = 0, int cmd = 0);

    void setValue(int value);
    int getValue() const      { return _value; }

    void setMinValue(int value);
    int  getMinValue() const      { return _valueMin; }
    void setMaxValue(int value);
    int  getMaxValue() const      { return _valueMax; }
    void setStepValue(int value);
    int  getStepValue() const     { return _stepValue; }

    virtual void handleMouseMoved(int x, int y, int button);
    virtual void handleMouseDown(int x, int y, int button, int clickCount);
    virtual void handleMouseUp(int x, int y, int button, int clickCount);
    virtual void handleMouseWheel(int x, int y, int direction);
    virtual bool handleEvent(Event::Type event);

  protected:
    void drawWidget(bool hilite);

    int valueToPos(int value);
    int posToValue(int pos);

  protected:
    int  _value, _stepValue;
    int  _valueMin, _valueMax;
    bool _isDragging;
    int  _labelWidth;
};

#endif
