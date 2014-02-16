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
// $Id: GameList.hxx 2838 2014-01-17 23:34:03Z stephena $
//
//   Based on code from KStella - Stella frontend
//   Copyright (C) 2003-2005 Stephen Anthony
//============================================================================

#ifndef GAME_LIST_HXX
#define GAME_LIST_HXX

#include <vector>
#include "bspf.hxx"

/**
  Holds the list of game info for the ROM launcher.
*/
class GameList
{
  public:
    GameList();
    ~GameList();

    const string& name(uInt32 i) const
      { return i < myArray.size() ? myArray[i]._name : EmptyString; }
    const string& path(uInt32 i) const
      { return i < myArray.size() ? myArray[i]._path : EmptyString; }
    const string& md5(uInt32 i) const
      { return i < myArray.size() ? myArray[i]._md5 : EmptyString; }
    const bool isDir(uInt32 i) const
      { return i < myArray.size() ? myArray[i]._isdir: false; }

    void setMd5(uInt32 i, const string& md5)
      { myArray[i]._md5 = md5; }

    int size() const { return myArray.size(); }
    void clear() { myArray.clear(); }

    void appendGame(const string& name, const string& path, const string& md5,
                    bool isDir = false);
    void sortByName();

  private:
    struct Entry {
      string _name;
      string _path;
      string _md5;
      bool   _isdir;

      Entry(string name, string path, string md5, bool isdir)
      {
        _name = name;  _path = path;  _md5 = md5;  _isdir = isdir;
      }

      bool operator < (const Entry& a) const;
    };
    vector<Entry> myArray;
};

#endif
