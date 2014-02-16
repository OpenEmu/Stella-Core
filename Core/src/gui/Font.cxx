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
// $Id: Font.cxx 2838 2014-01-17 23:34:03Z stephena $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "Font.hxx"

namespace GUI {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Font::Font(FontDesc desc)
  : myFontDesc(desc)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Font::getCharWidth(uInt8 chr) const
{
  // If no width table is specified, return the maximum width
  if(!myFontDesc.width)
    return myFontDesc.maxwidth;

  // If this character is not included in the font, use the default char.
  if(chr < myFontDesc.firstchar || myFontDesc.firstchar + myFontDesc.size < chr)
  {
    if(chr == ' ')
      return myFontDesc.maxwidth / 2;
    chr = myFontDesc.defaultchar;
  }

  return myFontDesc.width[chr - myFontDesc.firstchar];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Font::getStringWidth(const string& str) const
{
  // If no width table is specified, use the maximum width
  if(!myFontDesc.width)
    return myFontDesc.maxwidth * str.size();
  else
  {
    int space = 0;

    for(unsigned int i = 0; i < str.size(); ++i)
      space += getCharWidth(str[i]);

    return space;
  }
}

}  // namespace GUI
