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
// $Id: RectList.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef RECTLIST_HXX
#define RECTLIST_HXX

#include <SDL.h>

class RectList
{
  public:
    RectList(Uint32 size = 256);
    ~RectList();

    void add(SDL_Rect* rect);

    SDL_Rect* rects();
    Uint32 numRects();
    void start();
    void print(int boundWidth, int boundHeight);

  private:
    Uint32 currentSize, currentRect;

    SDL_Rect* rectArray;
};

#endif
