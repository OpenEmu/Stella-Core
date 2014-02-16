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
// $Id: TiaOutputWidget.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#include <sstream>

#include "OSystem.hxx"
#include "FrameBuffer.hxx"
#include "Widget.hxx"
#include "GuiObject.hxx"
#include "ContextMenu.hxx"
#include "TiaZoomWidget.hxx"
#include "Debugger.hxx"
#include "DebuggerParser.hxx"
#include "TIADebug.hxx"
#include "TIA.hxx"

#include "TiaOutputWidget.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TiaOutputWidget::TiaOutputWidget(GuiObject* boss, const GUI::Font& font,
                                 int x, int y, int w, int h)
  : Widget(boss, font, x, y, w, h),
    CommandSender(boss),
    myMenu(NULL),
    myZoom(NULL)
{
  // Create context menu for commands
  VariantList l;
  l.push_back("Fill to scanline", "scanline");
  l.push_back("Set breakpoint", "bp");
  l.push_back("Set zoom position", "zoom");
  l.push_back("Toggle fixed debug colors (from beam pos)", "fixed");
  myMenu = new ContextMenu(this, font, l);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TiaOutputWidget::~TiaOutputWidget()
{
  delete myMenu;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaOutputWidget::loadConfig()
{
  setDirty(); draw();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaOutputWidget::handleMouseDown(int x, int y, int button, int clickCount)
{
  // Grab right mouse button for command context menu
  if(button == 2)
  {
    myClickX = x;
    myClickY = y;

    // Add menu at current x,y mouse location
    myMenu->show(x + getAbsX(), y + getAbsY());
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaOutputWidget::handleCommand(CommandSender* sender, int cmd, int data, int id)
{
  int ystart = atoi(instance().console().properties().get(Display_YStart).c_str());

  switch(cmd)
  {
    case ContextMenu::kItemSelectedCmd:
    {
      const string& rmb = myMenu->getSelectedTag().toString();

      if(rmb == "scanline")
      {
        ostringstream command;
        int lines = myClickY + ystart;
        if(instance().console().tia().partialFrame())
          lines -= instance().console().tia().scanlines();
        if(lines > 0)
        {
          command << "scanline #" << lines;
          instance().debugger().parser().run(command.str());
        }
      }
      else if(rmb == "bp")
      {
        ostringstream command;
        int scanline = myClickY + ystart;
        command << "breakif _scan==#" << scanline;
        instance().debugger().parser().run(command.str());
      }
      else if(rmb == "zoom")
      {
        if(myZoom)
          myZoom->setPos(myClickX, myClickY);
      }
      else if(rmb == "fixed")
      {
        instance().console().tia().toggleFixedColors();
      }
      break;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TiaOutputWidget::drawWidget(bool hilite)
{
//cerr << "TiaOutputWidget::drawWidget\n";
  FBSurface& s = dialog().surface();

  const uInt32 width  = instance().console().tia().width(),
               height = instance().console().tia().height();

  // Get current scanline position
  // This determines where the frame greying should start, and where a
  // scanline 'pointer' should be drawn
  uInt16 scanx, scany, scanoffset;
  bool visible = instance().console().tia().scanlinePos(scanx, scany);
  scanoffset = width * scany + scanx;

  for(uInt32 y = 0, i = 0; y < height; ++y)
  {
    uInt32* line_ptr = myLineBuffer;
    for(uInt32 x = 0; x < width; ++x, ++i)
    {
      uInt8 shift = i > scanoffset ? 1 : 0;
      uInt32 pixel = instance().frameBuffer().tiaPixel(i, shift);
      *line_ptr++ = pixel;
      *line_ptr++ = pixel;
    }
    s.drawPixels(myLineBuffer, _x, _y+y, width << 1);
  }

  // Show electron beam position
  if(visible && scanx < width && scany+2u < height)
  {
    s.fillRect(_x+(scanx<<1), _y+scany, 3, 3, kBtnTextColor);
  }
}
