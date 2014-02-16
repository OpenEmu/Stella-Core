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
// $Id: RomAuditDialog.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "bspf.hxx"

#include "Launcher.hxx"
#include "LauncherFilterDialog.hxx"
#include "BrowserDialog.hxx"
#include "DialogContainer.hxx"
#include "EditTextWidget.hxx"
#include "ProgressDialog.hxx"
#include "FSNode.hxx"
#include "MessageBox.hxx"
#include "MD5.hxx"
#include "Props.hxx"
#include "PropsSet.hxx"
#include "Settings.hxx"
#include "RomAuditDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomAuditDialog::RomAuditDialog(OSystem* osystem, DialogContainer* parent,
                               const GUI::Font& font, int max_w, int max_h)
  : Dialog(osystem, parent, 0, 0, 0, 0),
    myBrowser(NULL),
    myConfirmMsg(NULL),
    myMaxWidth(max_w),
    myMaxHeight(max_h)
{
  const int vBorder = 8;

  const int lineHeight   = font.getLineHeight(),
            fontWidth    = font.getMaxCharWidth(),
            fontHeight   = font.getFontHeight(),
            buttonWidth  = font.getStringWidth("Audit path:") + 20,
            buttonHeight = font.getLineHeight() + 4,
            lwidth = font.getStringWidth("ROMs without properties (skipped): ");
  int xpos = vBorder, ypos = vBorder;
  WidgetArray wid;

  // Set real dimensions
  _w = 44 * fontWidth + 10;
  _h = 7 * (lineHeight + 4) + 10;

  // Audit path
  ButtonWidget* romButton = 
    new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                     "Audit path:", kChooseAuditDirCmd);
  wid.push_back(romButton);
  xpos += buttonWidth + 10;
  myRomPath = new EditTextWidget(this, font, xpos, ypos + 2,
                                 _w - xpos - 10, lineHeight, "");
  wid.push_back(myRomPath);

  // Show results of ROM audit
  xpos = vBorder + 10;  ypos += buttonHeight + 10;
  new StaticTextWidget(this, font, xpos, ypos, lwidth, fontHeight,
                       "ROMs with properties (renamed): ", kTextAlignLeft);
  myResults1 = new StaticTextWidget(this, font, xpos + lwidth, ypos,
                                    _w - lwidth - 20, fontHeight, "",
                                    kTextAlignLeft);
  myResults1->setFlags(WIDGET_CLEARBG);
  ypos += buttonHeight;
  new StaticTextWidget(this, font, xpos, ypos, lwidth, fontHeight,
                       "ROMs without properties (skipped): ", kTextAlignLeft);
  myResults2 = new StaticTextWidget(this, font, xpos + lwidth, ypos,
                                    _w - lwidth - 20, fontHeight, "",
                                    kTextAlignLeft);
  myResults2->setFlags(WIDGET_CLEARBG);

  ypos += buttonHeight + 8;
  new StaticTextWidget(this, font, xpos, ypos, _w - 20, fontHeight,
                       "(*) WARNING: operation cannot be undone",
                       kTextAlignLeft);

  // Add OK and Cancel buttons
  addOKCancelBGroup(wid, font, "Audit", "Done");
  addBGroupToFocusList(wid);

  // Create file browser dialog
  myBrowser = new BrowserDialog(this, font, myMaxWidth, myMaxHeight);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RomAuditDialog::~RomAuditDialog()
{
  delete myBrowser;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomAuditDialog::loadConfig()
{
  const string& currentdir =
    instance().launcher().currentNode().getShortPath();
  const string& path = currentdir == "" ?
    instance().settings().getString("romdir") : currentdir;

  myRomPath->setText(path);
  myResults1->setLabel("");
  myResults2->setLabel("");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomAuditDialog::auditRoms()
{
  const string& auditPath = myRomPath->getText();
  myResults1->setLabel("");
  myResults2->setLabel("");

  FilesystemNode node(auditPath);
  FSList files;
  files.reserve(2048);
  node.getChildren(files, FilesystemNode::kListFilesOnly);

  // Create a progress dialog box to show the progress of processing
  // the ROMs, since this is usually a time-consuming operation
  ProgressDialog progress(this, instance().font(),
                          "Auditing ROM files ...");
  progress.setRange(0, files.size() - 1, 5);

  // Create a entry for the GameList for each file
  Properties props;
  int renamed = 0, notfound = 0;
  for(unsigned int idx = 0; idx < files.size(); idx++)
  {
    string extension;
    if(files[idx].isFile() &&
       LauncherFilterDialog::isValidRomName(files[idx], extension))
    {
      // Calculate the MD5 so we can get the rest of the info
      // from the PropertiesSet (stella.pro)
      const string& md5 = MD5(files[idx]);
      instance().propSet().getMD5(md5, props);
      const string& name = props.get(Cartridge_Name);

      // Only rename the file if we found a valid properties entry
      if(name != "" && name != files[idx].getName())
      {
        const string& newfile = node.getPath() + name + "." + extension;

        if(files[idx].getPath() != newfile && files[idx].rename(newfile))
          renamed++;
      }
      else
        notfound++;
    }

    // Update the progress bar, indicating one more ROM has been processed
    progress.setProgress(idx);
  }
  progress.close();

  myResults1->setValue(renamed);
  myResults2->setValue(notfound);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RomAuditDialog::handleCommand(CommandSender* sender, int cmd,
                                   int data, int id)
{
  switch (cmd)
  {
    case kOKCmd:
      if(!myConfirmMsg)
      {
        StringList msg;
        msg.push_back("This operation cannot be undone.  Your ROMs");
        msg.push_back("will be modified, and as such there is a chance");
        msg.push_back("that files may be lost.  You are recommended");
        msg.push_back("to back up your files before proceeding.");
        msg.push_back("");
        msg.push_back("If you're sure you want to proceed with the");
        msg.push_back("audit, click 'OK', otherwise click 'Cancel'.");
        myConfirmMsg =
          new GUI::MessageBox(this, instance().font(), msg,
                              myMaxWidth, myMaxHeight,
                              kConfirmAuditCmd);
      }
      myConfirmMsg->show();
      break;

    case kConfirmAuditCmd:
      auditRoms();
      instance().launcher().reload();
      break;

    case kChooseAuditDirCmd:
      myBrowser->show("Select ROM directory to audit:", myRomPath->getText(),
                      BrowserDialog::Directories, kAuditDirChosenCmd);
      break;

    case kAuditDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myRomPath->setText(dir.getShortPath());
      myResults1->setLabel("");
      myResults2->setLabel("");
      break;
    }

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
