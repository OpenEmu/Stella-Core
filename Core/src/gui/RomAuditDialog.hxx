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
// $Id: RomAuditDialog.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef ROM_AUDIT_DIALOG_HXX
#define ROM_AUDIT_DIALOG_HXX

class OSystem;
class GuiObject;
class DialogContainer;
class BrowserDialog;
class EditTextWidget;
class StaticTextWidget;

#include "Dialog.hxx"
#include "Command.hxx"
#include "FSNode.hxx"
#include "MessageBox.hxx"

class RomAuditDialog : public Dialog
{
  public:
    RomAuditDialog(OSystem* osystem, DialogContainer* parent,
                   const GUI::Font& font, int max_w, int max_h);
    ~RomAuditDialog();

    void handleCommand(CommandSender* sender, int cmd, int data, int id);

  private:
    void loadConfig();
    void auditRoms();
    void openBrowser(const string& title, const string& startpath,
                     FilesystemNode::ListMode mode, int cmd);

  private:
    enum {
      kChooseAuditDirCmd = 'RAsl', // audit dir select
      kAuditDirChosenCmd = 'RAch', // audit dir changed
      kConfirmAuditCmd   = 'RAcf'  // confirm rom audit
    };

    // ROM audit path
    EditTextWidget* myRomPath;

    // Show the results of the ROM audit
    StaticTextWidget* myResults1;
    StaticTextWidget* myResults2;

    // Select a new ROM audit path
    BrowserDialog* myBrowser;

    // Show a message about the dangers of using this function
    GUI::MessageBox* myConfirmMsg;

    // Maximum width and height for this dialog
    int myMaxWidth, myMaxHeight;
};

#endif
