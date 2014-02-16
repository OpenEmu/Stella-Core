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
// $Id: UIDialog.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include <sstream>

#include "bspf.hxx"

#include "Dialog.hxx"
#include "DebuggerDialog.hxx"
#include "OSystem.hxx"
#include "ListWidget.hxx"
#include "PopUpWidget.hxx"
#include "ScrollBarWidget.hxx"
#include "Settings.hxx"
#include "StringList.hxx"
#include "TabWidget.hxx"
#include "Widget.hxx"

#include "UIDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UIDialog::UIDialog(OSystem* osystem, DialogContainer* parent,
                   const GUI::Font& font)
  : Dialog(osystem, parent, 0, 0, 0, 0)
{
  const GUI::Font& ifont = instance().infoFont();
  const int lineHeight   = font.getLineHeight(),
            fontWidth    = font.getMaxCharWidth(),
            fontHeight   = font.getFontHeight(),
            buttonWidth  = font.getStringWidth("Defaults") + 20,
            buttonHeight = font.getLineHeight() + 4;
  const int vBorder = 5;
  int xpos, ypos, tabID;
  int lwidth, pwidth = font.getStringWidth("Standard");
  WidgetArray wid;
  VariantList items;
  ButtonWidget* b;

  // Set real dimensions
  _w = 37 * fontWidth + 10;
  _h = 11 * (lineHeight + 4) + 10;

  // The tab widget
  xpos = ypos = vBorder;
  myTab = new TabWidget(this, font, xpos, ypos, _w - 2*xpos, _h - buttonHeight - 20);
  addTabWidget(myTab);

  //////////////////////////////////////////////////////////
  // 1) Launcher options
  wid.clear();
  tabID = myTab->addTab(" Launcher ");
  lwidth = font.getStringWidth("Exit to Launcher: ");

  // Launcher width and height
  myLauncherWidthSlider = new SliderWidget(myTab, font, xpos, ypos, pwidth,
                                           lineHeight, "Launcher Width: ",
                                           lwidth, kLWidthChanged);
  if(instance().desktopWidth() < 640)
  {
    myLauncherWidthSlider->setMinValue(320);
    myLauncherWidthSlider->setMaxValue(instance().desktopWidth());
  }
  else
  {
    myLauncherWidthSlider->setMinValue(640);
    myLauncherWidthSlider->setMaxValue(1920);
  }
  myLauncherWidthSlider->setStepValue(10);
  wid.push_back(myLauncherWidthSlider);
  myLauncherWidthLabel =
      new StaticTextWidget(myTab, font,
                           xpos + myLauncherWidthSlider->getWidth() + 4,
                           ypos + 1, 4*fontWidth, fontHeight, "", kTextAlignLeft);
  myLauncherWidthLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  myLauncherHeightSlider = new SliderWidget(myTab, font, xpos, ypos, pwidth,
                                            lineHeight, "Launcher Height: ",
                                            lwidth, kLHeightChanged);
  if(instance().desktopHeight() < 480)
  {
    myLauncherHeightSlider->setMinValue(240);
    myLauncherHeightSlider->setMaxValue(instance().desktopHeight());
  }
  else
  {
    myLauncherHeightSlider->setMinValue(480);
    myLauncherHeightSlider->setMaxValue(1200);
  }
  myLauncherHeightSlider->setStepValue(10);
  wid.push_back(myLauncherHeightSlider);
  myLauncherHeightLabel =
      new StaticTextWidget(myTab, font,
                           xpos + myLauncherHeightSlider->getWidth() + 4,
                           ypos + 1, 4*fontWidth, fontHeight, "", kTextAlignLeft);
  myLauncherHeightLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  // Launcher font
  pwidth = font.getStringWidth("2x (1000x760)");
  items.clear();
  items.push_back("Small",  "small");
  items.push_back("Medium", "medium");
  items.push_back("Large",  "large");
  myLauncherFontPopup =
    new PopUpWidget(myTab, font, xpos, ypos+1, pwidth, lineHeight, items,
                    "Launcher Font: ", lwidth);
  wid.push_back(myLauncherFontPopup);
  ypos += lineHeight + 4;

  // ROM launcher info/snapshot viewer
  items.clear();
  items.push_back("Off", "0");
  items.push_back("1x (640x480) ", "1");
  items.push_back("2x (1000x760)", "2");
  myRomViewerPopup =
    new PopUpWidget(myTab, font, xpos, ypos+1, pwidth, lineHeight, items,
                    "ROM Info viewer: ", lwidth);
  wid.push_back(myRomViewerPopup);
  ypos += lineHeight + 4;

  // Exit to Launcher
  pwidth = font.getStringWidth("If in use");
  items.clear();
  items.push_back("If in use", "0");
  items.push_back("Always", "1");
  myLauncherExitPopup =
    new PopUpWidget(myTab, font, xpos, ypos+1, pwidth, lineHeight, items,
                    "Exit to Launcher: ", lwidth);
  wid.push_back(myLauncherExitPopup);
  ypos += lineHeight + 4;

  // Add message concerning usage
  xpos = vBorder; ypos += 1*(lineHeight + 4);
  lwidth = ifont.getStringWidth("(*) Changes require application restart");
  new StaticTextWidget(myTab, ifont, xpos, ypos, BSPF_min(lwidth, _w-20), fontHeight,
                       "(*) Changes require application restart",
                       kTextAlignLeft);

  // Add items for tab 0
  addToFocusList(wid, myTab, tabID);

  //////////////////////////////////////////////////////////
  // 2) Debugger options
  wid.clear();
  tabID = myTab->addTab(" Debugger ");
  lwidth = font.getStringWidth("Debugger Height: ");
  xpos = ypos = vBorder;

  // Debugger width and height
  myDebuggerWidthSlider = new SliderWidget(myTab, font, xpos, ypos, pwidth,
                                           lineHeight, "Debugger Width: ",
                                           lwidth, kDWidthChanged);
  myDebuggerWidthSlider->setMinValue(DebuggerDialog::kSmallFontMinW);
  myDebuggerWidthSlider->setMaxValue(osystem->desktopWidth());
  myDebuggerWidthSlider->setStepValue(10);
  wid.push_back(myDebuggerWidthSlider);
  myDebuggerWidthLabel =
      new StaticTextWidget(myTab, font,
                           xpos + myDebuggerWidthSlider->getWidth() + 4,
                           ypos + 1, 4*fontWidth, fontHeight, "", kTextAlignLeft);
  myDebuggerWidthLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  myDebuggerHeightSlider = new SliderWidget(myTab, font, xpos, ypos, pwidth,
                                            lineHeight, "Debugger Height: ",
                                            lwidth, kDHeightChanged);
  myDebuggerHeightSlider->setMinValue(DebuggerDialog::kSmallFontMinH);
  myDebuggerHeightSlider->setMaxValue(osystem->desktopHeight());
  myDebuggerHeightSlider->setStepValue(10);
  wid.push_back(myDebuggerHeightSlider);
  myDebuggerHeightLabel =
      new StaticTextWidget(myTab, font,
                           xpos + myDebuggerHeightSlider->getWidth() + 4,
                           ypos + 1, 4*fontWidth, fontHeight, "", kTextAlignLeft);
  myDebuggerHeightLabel->setFlags(WIDGET_CLEARBG);

  // Add minimum window size buttons for different fonts
  int fbwidth = font.getStringWidth("Set window size for medium font") + 20;
  xpos = (_w - fbwidth - 2*vBorder)/2;  ypos += 2*lineHeight + 4;
  b = new ButtonWidget(myTab, font, xpos, ypos, fbwidth, buttonHeight,
      "Set window size for small font", kDSmallSize);
  wid.push_back(b);
  ypos += b->getHeight() + 4;
  b = new ButtonWidget(myTab, font, xpos, ypos, fbwidth, buttonHeight,
      "Set window size for medium font", kDMediumSize);
  wid.push_back(b);
  ypos += b->getHeight() + 4;
  b = new ButtonWidget(myTab, font, xpos, ypos, fbwidth, buttonHeight,
      "Set window size for large font", kDLargeSize);
  wid.push_back(b);
  ypos += b->getHeight() + 12;

  // Font style (bold label vs. text, etc)
  lwidth = font.getStringWidth("Font Style: ");
  pwidth = font.getStringWidth("Bold non-labels only");
  xpos = vBorder;
  items.clear();
  items.push_back("All Normal font", "0");
  items.push_back("Bold labels only", "1");
  items.push_back("Bold non-labels only", "2");
  items.push_back("All Bold font", "3");
  myDebuggerFontStyle =
    new PopUpWidget(myTab, font, xpos, ypos+1, pwidth, lineHeight, items,
                    "Font Style: ", lwidth);
  wid.push_back(myDebuggerFontStyle);

  // Debugger is only realistically available in windowed modes 800x600 or greater
  // (and when it's actually been compiled into the app)
  bool debuggerAvailable = 
#if defined(DEBUGGER_SUPPORT) && defined(WINDOWED_SUPPORT)
    (instance().desktopWidth() >= 800 && instance().desktopHeight() >= 600);
#else
  false;
#endif
  if(!debuggerAvailable)
  {
    myDebuggerWidthSlider->clearFlags(WIDGET_ENABLED);
    myDebuggerWidthLabel->clearFlags(WIDGET_ENABLED);
    myDebuggerHeightSlider->clearFlags(WIDGET_ENABLED);
    myDebuggerHeightLabel->clearFlags(WIDGET_ENABLED);
  }

  // Add items for tab 1
  addToFocusList(wid, myTab, tabID);

  //////////////////////////////////////////////////////////
  // 3) Misc. options
  wid.clear();
  tabID = myTab->addTab(" Misc. ");
  lwidth = font.getStringWidth("Mouse wheel scroll: ");
  pwidth = font.getStringWidth("Standard");
  xpos = ypos = vBorder;

  // UI Palette
  ypos += 1;
  items.clear();
  items.push_back("Standard", "1");
  items.push_back("Classic", "2");
  myPalettePopup = new PopUpWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                   items, "Interface Palette: ", lwidth);
  wid.push_back(myPalettePopup);
  ypos += lineHeight + 4;

  // Delay between quick-selecting characters in ListWidget
  items.clear();
  items.push_back("300 ms", "300");
  items.push_back("400 ms", "400");
  items.push_back("500 ms", "500");
  items.push_back("600 ms", "600");
  items.push_back("700 ms", "700");
  items.push_back("800 ms", "800");
  items.push_back("900 ms", "900");
  items.push_back("1 sec", "1000");
  myListDelayPopup = new PopUpWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                     items, "List quick delay: ", lwidth);
  wid.push_back(myListDelayPopup);
  ypos += lineHeight + 4;

  // Number of lines a mouse wheel will scroll
  items.clear();
  items.push_back("1 line", "1");
  items.push_back("2 lines", "2");
  items.push_back("3 lines", "3");
  items.push_back("4 lines", "4");
  items.push_back("5 lines", "5");
  items.push_back("6 lines", "6");
  items.push_back("7 lines", "7");
  items.push_back("8 lines", "8");
  items.push_back("9 lines", "9");
  items.push_back("10 lines", "10");
  myWheelLinesPopup = new PopUpWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                      items, "Mouse wheel scroll: ", lwidth);
  wid.push_back(myWheelLinesPopup);
  ypos += lineHeight + 4;

  // Add items for tab 2
  addToFocusList(wid, myTab, tabID);

  // Activate the first tab
  myTab->setActiveTab(0);

  // Add Defaults, OK and Cancel buttons
  wid.clear();
  b = new ButtonWidget(this, font, 10, _h - buttonHeight - 10,
                       buttonWidth, buttonHeight, "Defaults", kDefaultsCmd);
  wid.push_back(b);
  addOKCancelBGroup(wid, font);
  addBGroupToFocusList(wid);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UIDialog::~UIDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UIDialog::loadConfig()
{
  // Launcher size
  const GUI::Size& ls = instance().settings().getSize("launcherres");
  int w = ls.w, h = ls.h;

  w = BSPF_max(w, 320);
  h = BSPF_max(h, 240);
  w = BSPF_min(w, 1920);
  h = BSPF_min(h, 1200);

  myLauncherWidthSlider->setValue(w);
  myLauncherWidthLabel->setValue(w);
  myLauncherHeightSlider->setValue(h);
  myLauncherHeightLabel->setValue(h);

  // Launcher font
  const string& font = instance().settings().getString("launcherfont");
  myLauncherFontPopup->setSelected(font, "medium");

  // ROM launcher info viewer
  const string& viewer = instance().settings().getString("romviewer");
  myRomViewerPopup->setSelected(viewer, "0");

  // Exit to launcher
  bool exitlauncher = instance().settings().getBool("exitlauncher");
  myLauncherExitPopup->setSelected(exitlauncher ? "1" : "0", "0");

#ifdef DEBUGGER_SUPPORT
  // Debugger size
  const GUI::Size& ds = instance().settings().getSize("dbg.res");
  w = ds.w, h = ds.h;
  w = BSPF_max(w, (int)DebuggerDialog::kSmallFontMinW);
  h = BSPF_max(h, (int)DebuggerDialog::kSmallFontMinH);
  w = BSPF_min(w, (int)instance().desktopWidth());
  h = BSPF_min(h, (int)instance().desktopHeight());

  myDebuggerWidthSlider->setValue(w);
  myDebuggerWidthLabel->setValue(w);
  myDebuggerHeightSlider->setValue(h);
  myDebuggerHeightLabel->setValue(h);

  // Debugger font style
  int style = instance().settings().getInt("dbg.fontstyle");
  myDebuggerFontStyle->setSelected(style, "0");
#endif

  // UI palette
  const string& pal = instance().settings().getString("uipalette");
  myPalettePopup->setSelected(pal, "1");

  // Listwidget quick delay
  const string& delay = instance().settings().getString("listdelay");
  myListDelayPopup->setSelected(delay, "300");

  // Mouse wheel lines
  const string& mw = instance().settings().getString("mwheel");
  myWheelLinesPopup->setSelected(mw, "1");

  myTab->loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UIDialog::saveConfig()
{
  // Launcher size
  instance().settings().setValue("launcherres", 
    GUI::Size(myLauncherWidthSlider->getValue(),
              myLauncherHeightSlider->getValue()));

  // Launcher font
  instance().settings().setValue("launcherfont",
    myLauncherFontPopup->getSelectedTag().toString());

  // ROM launcher info viewer
  instance().settings().setValue("romviewer",
    myRomViewerPopup->getSelectedTag().toString());

  // Exit to Launcher
  instance().settings().setValue("exitlauncher",
    myLauncherExitPopup->getSelectedTag().toString());

  // Debugger size
  instance().settings().setValue("dbg.res",
    GUI::Size(myDebuggerWidthSlider->getValue(),
              myDebuggerHeightSlider->getValue()));

  // Debugger font style
  instance().settings().setValue("dbg.fontstyle",
    myDebuggerFontStyle->getSelectedTag().toString());

  // UI palette
  instance().settings().setValue("uipalette",
    myPalettePopup->getSelectedTag().toString());

  // Listwidget quick delay
  instance().settings().setValue("listdelay",
    myListDelayPopup->getSelectedTag().toString());
  ListWidget::setQuickSelectDelay(myListDelayPopup->getSelectedTag().toInt());

  // Mouse wheel lines
  instance().settings().setValue("mwheel",
    myWheelLinesPopup->getSelectedTag().toString());
  ScrollBarWidget::setWheelLines(myWheelLinesPopup->getSelectedTag().toInt());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UIDialog::setDefaults()
{
  switch(myTab->getActiveTab())
  {
    case 0:  // Launcher options
    {
      int w = BSPF_min(instance().desktopWidth(), 640u);
      int h = BSPF_min(instance().desktopHeight(), 480u);
      myLauncherWidthSlider->setValue(w);
      myLauncherWidthLabel->setValue(w);
      myLauncherHeightSlider->setValue(h);
      myLauncherHeightLabel->setValue(h);
      myLauncherFontPopup->setSelected("medium", "");
      myRomViewerPopup->setSelected("0", "");
      myLauncherExitPopup->setSelected("0", "");
      break;
    }

    case 1:  // Debugger options
    {
      int w = BSPF_min(instance().desktopWidth(), (uInt32)DebuggerDialog::kMediumFontMinW);
      int h = BSPF_min(instance().desktopHeight(), (uInt32)DebuggerDialog::kMediumFontMinH);
      myDebuggerWidthSlider->setValue(w);
      myDebuggerWidthLabel->setValue(w);
      myDebuggerHeightSlider->setValue(h);
      myDebuggerHeightLabel->setValue(h);
      myDebuggerFontStyle->setSelected("0");
      break;
    }

    case 2:  // Misc. options
      myPalettePopup->setSelected("1");
      myListDelayPopup->setSelected("300");
      myWheelLinesPopup->setSelected("4");
      break;

    default:
      break;
  }

  _dirty = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UIDialog::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  switch(cmd)
  {
    case kLWidthChanged:
      myLauncherWidthLabel->setValue(myLauncherWidthSlider->getValue());
      break;

    case kLHeightChanged:
      myLauncherHeightLabel->setValue(myLauncherHeightSlider->getValue());
      break;

    case kDWidthChanged:
      myDebuggerWidthLabel->setValue(myDebuggerWidthSlider->getValue());
      break;

    case kDHeightChanged:
      myDebuggerHeightLabel->setValue(myDebuggerHeightSlider->getValue());
      break;

    case kDSmallSize:
      myDebuggerWidthSlider->setValue(DebuggerDialog::kSmallFontMinW);
      myDebuggerWidthLabel->setValue(DebuggerDialog::kSmallFontMinW);
      myDebuggerHeightSlider->setValue(DebuggerDialog::kSmallFontMinH);
      myDebuggerHeightLabel->setValue(DebuggerDialog::kSmallFontMinH);
      break;

    case kDMediumSize:
      myDebuggerWidthSlider->setValue(DebuggerDialog::kMediumFontMinW);
      myDebuggerWidthLabel->setValue(DebuggerDialog::kMediumFontMinW);
      myDebuggerHeightSlider->setValue(DebuggerDialog::kMediumFontMinH);
      myDebuggerHeightLabel->setValue(DebuggerDialog::kMediumFontMinH);
      break;

    case kDLargeSize:
      myDebuggerWidthSlider->setValue(DebuggerDialog::kLargeFontMinW);
      myDebuggerWidthLabel->setValue(DebuggerDialog::kLargeFontMinW);
      myDebuggerHeightSlider->setValue(DebuggerDialog::kLargeFontMinH);
      myDebuggerHeightLabel->setValue(DebuggerDialog::kLargeFontMinH);
      break;

    case kOKCmd:
      saveConfig();
      close();
      instance().setUIPalette();
      break;

    case kDefaultsCmd:
      setDefaults();
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
