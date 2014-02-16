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
// $Id: EditableWidget.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef EDITABLE_WIDGET_HXX
#define EDITABLE_WIDGET_HXX

#include "Widget.hxx"
#include "Rect.hxx"

/**
 * Base class for widgets which need to edit text, like ListWidget and
 * EditTextWidget.
 */
class EditableWidget : public Widget, public CommandSender
{
  public:
    enum {
      kAcceptCmd  = 'EDac',
      kCancelCmd  = 'EDcl',
      kChangedCmd = 'EDch'
    };

  public:
    EditableWidget(GuiObject *boss, const GUI::Font& font,
                   int x, int y, int w, int h, const string& str = "");
    virtual ~EditableWidget();

    virtual void setText(const string& str, bool changed = false);
    virtual const string& getText() const { return _editString; }

    bool isEditable() const	 { return _editable; }
    void setEditable(bool editable);

    virtual bool handleKeyDown(StellaKey key, StellaMod mod, char ascii);

    // We only want to focus this widget when we can edit its contents
    virtual bool wantsFocus() { return _editable; }

  protected:
    virtual void startEditMode() { setFlags(WIDGET_WANTS_RAWDATA);   }
    virtual void endEditMode()   { clearFlags(WIDGET_WANTS_RAWDATA); }
    virtual void abortEditMode() { clearFlags(WIDGET_WANTS_RAWDATA); }

    virtual GUI::Rect getEditRect() const = 0;
    virtual int getCaretOffset() const;
    void drawCaret();
    bool setCaretPos(int newPos);
    bool adjustOffset();
	
    virtual bool tryInsertChar(char c, int pos);

  private:
    // Line editing
    bool specialKeys(StellaKey key, char ascii);
    bool killChar(int direction);
    bool killLine(int direction);
    bool killLastWord();
    bool moveWord(int direction);

    // Clipboard
    void copySelectedText();
    void pasteSelectedText();

  protected:
    bool   _editable;
    string _editString;

    bool  _caretVisible;
    int   _caretTime;
    int   _caretPos;

    bool  _caretInverse;

    int   _editScrollOffset;

    static string _clippedText;
};

#endif
