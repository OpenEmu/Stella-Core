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
// $Id: InputDialog.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include "bspf.hxx"

#include "Array.hxx"
#include "OSystem.hxx"
#include "Joystick.hxx"
#include "Paddles.hxx"
#include "Settings.hxx"
#include "StringList.hxx"
#include "EventMappingWidget.hxx"
#include "EditTextWidget.hxx"
#include "PopUpWidget.hxx"
#include "TabWidget.hxx"
#include "Widget.hxx"

#include "InputDialog.hxx"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputDialog::InputDialog(OSystem* osystem, DialogContainer* parent,
                         const GUI::Font& font, int max_w, int max_h)
  : Dialog(osystem, parent, 0, 0, 0, 0)
{
  const int lineHeight   = font.getLineHeight(),
            fontWidth    = font.getMaxCharWidth(),
            buttonWidth  = font.getStringWidth("Defaults") + 20,
            buttonHeight = font.getLineHeight() + 4;
  const int vBorder = 4;
  int xpos, ypos, tabID;
  WidgetArray wid;
  StringList actions;

  // Set real dimensions
  _w = BSPF_min(50 * fontWidth + 10, max_w);
  _h = BSPF_min(14 * (lineHeight + 4) + 14, max_h);

  // The tab widget
  xpos = 2; ypos = vBorder;
  myTab = new TabWidget(this, font, xpos, ypos, _w - 2*xpos, _h - buttonHeight - 20);
  addTabWidget(myTab);

  // 1) Event mapper for emulation actions
  tabID = myTab->addTab("Emul. Events");
  instance().eventHandler().getActionList(kEmulationMode, actions);
  myEmulEventMapper = new EventMappingWidget(myTab, font, 2, 2,
                                             myTab->getWidth(),
                                             myTab->getHeight() - ypos,
                                             actions, kEmulationMode);
  myTab->setParentWidget(tabID, myEmulEventMapper);
  addToFocusList(myEmulEventMapper->getFocusList(), myTab, tabID);

  // 2) Event mapper for UI actions
  tabID = myTab->addTab("UI Events");
  actions.clear();
  instance().eventHandler().getActionList(kMenuMode, actions);
  myMenuEventMapper = new EventMappingWidget(myTab, font, 2, 2,
                                             myTab->getWidth(),
                                             myTab->getHeight() - ypos,
                                             actions, kMenuMode);
  myTab->setParentWidget(tabID, myMenuEventMapper);
  addToFocusList(myMenuEventMapper->getFocusList(), myTab, tabID);

  // 3) Devices & ports
  addDevicePortTab(font);

  // Finalize the tabs, and activate the first tab
  myTab->activateTabs();
  myTab->setActiveTab(0);

  // Add Defaults, OK and Cancel buttons
  wid.clear();
  ButtonWidget* b;
  b = new ButtonWidget(this, font, 10, _h - buttonHeight - 10,
                       buttonWidth, buttonHeight, "Defaults", kDefaultsCmd);
  wid.push_back(b);
  addOKCancelBGroup(wid, font);
  addBGroupToFocusList(wid);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputDialog::~InputDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::addDevicePortTab(const GUI::Font& font)
{
  const int lineHeight = font.getLineHeight(),
            fontWidth  = font.getMaxCharWidth(),
            fontHeight = font.getFontHeight();
  int xpos, ypos, lwidth, pwidth, tabID;
  WidgetArray wid;
  VariantList items;

  // Devices/ports
  tabID = myTab->addTab("Devices & Ports");

  // Stelladaptor mappings
  xpos = 5;  ypos = 5;
  lwidth = font.getStringWidth("Use mouse as a controller: ");
  pwidth = font.getStringWidth("Analog devices");

  items.clear();
  items.push_back("Left / Right", "lr");
  items.push_back("Right / Left", "rl");
  mySAPort = new PopUpWidget(myTab, font, xpos, ypos, pwidth, lineHeight, items,
                             "Stelladaptor port order: ", lwidth);
  wid.push_back(mySAPort);

  // Use mouse as controller
  ypos += lineHeight + 5;
  items.clear();
  items.push_back("Always", "always");
  items.push_back("Analog devices", "analog");
  items.push_back("Never", "never");
  myMouseControl = new PopUpWidget(myTab, font, xpos, ypos, pwidth, lineHeight, items,
                             "Use mouse as a controller: ", lwidth);
  wid.push_back(myMouseControl);

  // Add AtariVox serial port
  ypos += lineHeight + 5;
  lwidth = font.getStringWidth("AVox serial port: ");
  int fwidth = _w - xpos - lwidth - 20;
  new StaticTextWidget(myTab, font, xpos, ypos, lwidth, fontHeight,
                       "AVox serial port: ", kTextAlignLeft);
  myAVoxPort = new EditTextWidget(myTab, font, xpos+lwidth, ypos,
                                  fwidth, fontHeight, "");
  wid.push_back(myAVoxPort);

  lwidth = font.getStringWidth("Digital paddle sensitivity: ");
  pwidth = font.getMaxCharWidth() * 8;

  // Add joystick deadzone setting
  ypos += lineHeight + 8;
  myDeadzone = new SliderWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                "Joystick deadzone size: ", lwidth, kDeadzoneChanged);
  myDeadzone->setMinValue(0); myDeadzone->setMaxValue(29);
  xpos += myDeadzone->getWidth() + 5;
  myDeadzoneLabel = new StaticTextWidget(myTab, font, xpos, ypos+1, 5*fontWidth,
                                         lineHeight, "", kTextAlignLeft);
  myDeadzoneLabel->setFlags(WIDGET_CLEARBG);
  wid.push_back(myDeadzone);

  // Add paddle speed (digital emulation)
  xpos = 5;  ypos += lineHeight + 4;
  myDPaddleSpeed = new SliderWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                   "Digital paddle sensitivity: ",
                                   lwidth, kDPSpeedChanged);
  myDPaddleSpeed->setMinValue(1); myDPaddleSpeed->setMaxValue(10);
  xpos += myDPaddleSpeed->getWidth() + 5;
  myDPaddleLabel = new StaticTextWidget(myTab, font, xpos, ypos+1, 24, lineHeight,
                                        "", kTextAlignLeft);
  myDPaddleLabel->setFlags(WIDGET_CLEARBG);
  wid.push_back(myDPaddleSpeed);

  // Add paddle speed (mouse emulation)
  xpos = 5;  ypos += lineHeight + 4;
  myMPaddleSpeed = new SliderWidget(myTab, font, xpos, ypos, pwidth, lineHeight,
                                   "Mouse paddle sensitivity: ",
                                   lwidth, kMPSpeedChanged);
  myMPaddleSpeed->setMinValue(1); myMPaddleSpeed->setMaxValue(15);
  xpos += myMPaddleSpeed->getWidth() + 5;
  myMPaddleLabel = new StaticTextWidget(myTab, font, xpos, ypos+1, 24, lineHeight,
                                        "", kTextAlignLeft);
  myMPaddleSpeed->setFlags(WIDGET_CLEARBG);
  wid.push_back(myMPaddleSpeed);

  // Add 'allow all 4 directions' for joystick
  xpos = 10;  ypos += lineHeight + 12;
  myAllowAll4 = new CheckboxWidget(myTab, font, xpos, ypos,
                  "Allow all 4 directions on joystick");
  wid.push_back(myAllowAll4);

  // Grab mouse (in windowed mode)
  ypos += lineHeight + 4;
  myGrabMouse = new CheckboxWidget(myTab, font, xpos, ypos,
	                "Grab mouse in emulation mode");
  wid.push_back(myGrabMouse);
#ifndef WINDOWED_SUPPORT
  myGrabMouse->clearFlags(WIDGET_ENABLED);
#endif

  // Add items for virtual device ports
  addToFocusList(wid, myTab, tabID);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::loadConfig()
{
  // Left & right ports
  mySAPort->setSelected(instance().settings().getString("saport"), "lr");

  // Use mouse as a controller
  myMouseControl->setSelected(
    instance().settings().getString("usemouse"), "analog");

  // Joystick deadzone
  myDeadzone->setValue(instance().settings().getInt("joydeadzone"));
  myDeadzoneLabel->setValue(Joystick::deadzone());

  // Grab mouse
  myGrabMouse->setState(instance().settings().getBool("grabmouse"));

  // Paddle speed (digital and mouse)
  myDPaddleSpeed->setValue(instance().settings().getInt("dsense"));
  myDPaddleLabel->setLabel(instance().settings().getString("dsense"));
  myMPaddleSpeed->setValue(instance().settings().getInt("msense"));
  myMPaddleLabel->setLabel(instance().settings().getString("msense"));

  // AtariVox serial port
  myAVoxPort->setText(instance().settings().getString("avoxport"));

  // Allow all 4 joystick directions
  myAllowAll4->setState(instance().settings().getBool("joyallow4"));

  myTab->loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::saveConfig()
{
  // Left & right ports
  instance().eventHandler().mapStelladaptors(mySAPort->getSelectedTag().toString());

  // Use mouse as a controller
  const string& usemouse = myMouseControl->getSelectedTag().toString();
  instance().settings().setValue("usemouse", usemouse);
  instance().eventHandler().setMouseControllerMode(usemouse);

  // Joystick deadzone
  int deadzone = myDeadzone->getValue();
  instance().settings().setValue("joydeadzone", deadzone);
  Joystick::setDeadZone(deadzone);

  // Grab mouse	 
  instance().settings().setValue("grabmouse", myGrabMouse->getState());	 
  instance().frameBuffer().setCursorState();

  // Paddle speed (digital and mouse)
  int sensitivity = myDPaddleSpeed->getValue();
  instance().settings().setValue("dsense", sensitivity);
  Paddles::setDigitalSensitivity(sensitivity);
  sensitivity = myMPaddleSpeed->getValue();
  instance().settings().setValue("msense", sensitivity);
  Paddles::setMouseSensitivity(sensitivity);

  // AtariVox serial port
  instance().settings().setValue("avoxport", myAVoxPort->getText());

  // Allow all 4 joystick directions
  bool allowall4 = myAllowAll4->getState();
  instance().settings().setValue("joyallow4", allowall4);
  instance().eventHandler().allowAllDirections(allowall4);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::setDefaults()
{
  switch(myTab->getActiveTab())
  {
    case 0:  // Emulation events
      myEmulEventMapper->setDefaults();
      break;

    case 1:  // UI events
      myMenuEventMapper->setDefaults();
      break;

    case 2:  // Virtual devices
    {
      // Left & right ports
      mySAPort->setSelected("lr");

      // Use mouse as a controller
      myMouseControl->setSelected("analog");

      // Joystick deadzone
      myDeadzone->setValue(0);
      myDeadzoneLabel->setValue(3200);

      // Grab mouse
      myGrabMouse->setState(true);

      // Paddle speed (digital and mouse)
      myDPaddleSpeed->setValue(5);
      myDPaddleLabel->setLabel("5");
      myMPaddleSpeed->setValue(6);
      myMPaddleLabel->setLabel("6");

      // AtariVox serial port
      myAVoxPort->setText("");

      // Allow all 4 joystick directions
      myAllowAll4->setState(false);

      break;
    }

    default:
      break;
  }

  _dirty = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::handleKeyDown(StellaKey key, StellaMod mod, char ascii)
{
  // Remap key events in remap mode, otherwise pass to parent dialog
  if(myEmulEventMapper->remapMode())
    myEmulEventMapper->handleKeyDown(key, mod, ascii);
  else if(myMenuEventMapper->remapMode())
    myMenuEventMapper->handleKeyDown(key, mod, ascii);
  else
    Dialog::handleKeyDown(key, mod, ascii);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::handleJoyDown(int stick, int button)
{
  // Remap joystick buttons in remap mode, otherwise pass to parent dialog
  if(myEmulEventMapper->remapMode())
    myEmulEventMapper->handleJoyDown(stick, button);
  else if(myMenuEventMapper->remapMode())
    myMenuEventMapper->handleJoyDown(stick, button);
  else
    Dialog::handleJoyDown(stick, button);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::handleJoyAxis(int stick, int axis, int value)
{
  // Remap joystick axis in remap mode, otherwise pass to parent dialog
  if(myEmulEventMapper->remapMode())
    myEmulEventMapper->handleJoyAxis(stick, axis, value);
  else if(myMenuEventMapper->remapMode())
    myMenuEventMapper->handleJoyAxis(stick, axis, value);
  else
    Dialog::handleJoyAxis(stick, axis, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool InputDialog::handleJoyHat(int stick, int hat, int value)
{
  // Remap joystick hat in remap mode, otherwise pass to parent dialog
  if(myEmulEventMapper->remapMode())
    return myEmulEventMapper->handleJoyHat(stick, hat, value);
  else if(myMenuEventMapper->remapMode())
    return myMenuEventMapper->handleJoyHat(stick, hat, value);
  else
    return Dialog::handleJoyHat(stick, hat, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputDialog::handleCommand(CommandSender* sender, int cmd,
                                int data, int id)
{
  switch(cmd)
  {
    case kOKCmd:
      saveConfig();
      close();
      break;

    case kCloseCmd:
      // Revert changes made to event mapping
      close();
      break;

    case kDefaultsCmd:
      setDefaults();
      break;

    case kDeadzoneChanged:
      myDeadzoneLabel->setValue(3200 + 1000*myDeadzone->getValue());
      break;

    case kDPSpeedChanged:
      myDPaddleLabel->setValue(myDPaddleSpeed->getValue());
      break;

    case kMPSpeedChanged:
      myMPaddleLabel->setValue(myMPaddleSpeed->getValue());
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
  }
}
