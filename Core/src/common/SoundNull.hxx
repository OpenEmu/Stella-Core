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
// $Id: SoundNull.hxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifndef SOUND_NULL_HXX
#define SOUND_NULL_HXX

class OSystem;

#include "bspf.hxx"
#include "Sound.hxx"
#include "OSystem.hxx"

/**
  This class implements a Null sound object, where-by sound generation
  is completely disabled.

  @author Stephen Anthony
  @version $Id: SoundNull.hxx 2838 2014-01-17 23:34:03Z stephena $
*/
class SoundNull : public Sound
{
  public:
    /**
      Create a new sound object.  The init method must be invoked before
      using the object.
    */
    SoundNull(OSystem* osystem) : Sound(osystem)
    {
      myOSystem->logMessage("Sound disabled.\n", 1);
    }
 
    /**
      Destructor
    */
    virtual ~SoundNull();

  public: 
    /**
      Enables/disables the sound subsystem.

      @param enable  Either true or false, to enable or disable the sound system
      @return        Whether the sound system was enabled or disabled
    */
    void setEnabled(bool enable) { }

    /**
      The system cycle counter is being adjusting by the specified amount.  Any
      members using the system cycle counter should be adjusted as needed.

      @param amount The amount the cycle counter is being adjusted by
    */
    void adjustCycleCounter(Int32 amount) { }

    /**
      Sets the number of channels (mono or stereo sound).

      @param channels The number of channels
    */
    void setChannels(uInt32 channels) { }

    /**
      Sets the display framerate.  Sound generation for NTSC and PAL games
      depends on the framerate, so we need to set it here.

      @param framerate The base framerate depending on NTSC or PAL ROM
    */
    void setFrameRate(float framerate) { }

    /**
      Initializes the sound device.  This must be called before any
      calls are made to derived methods.
    */
    void open() { }

    /**
      Should be called to close the sound device.  Once called the sound
      device can be started again using the initialize method.
    */
    void close() { }

    /**
      Set the mute state of the sound object.  While muted no sound is played.

      @param state Mutes sound if true, unmute if false
    */
    void mute(bool state) { }

    /**
      Reset the sound device.
    */
    void reset() { }

    /**
      Sets the sound register to a given value.

      @param addr  The register address
      @param value The value to save into the register
      @param cycle The system cycle at which the register is being updated
    */
    void set(uInt16 addr, uInt8 value, Int32 cycle) { }

    /**
      Sets the volume of the sound device to the specified level.  The
      volume is given as a percentage from 0 to 100.  Values outside
      this range indicate that the volume shouldn't be changed at all.

      @param percent The new volume percentage level for the sound device
    */
    void setVolume(Int32 percent) { }

    /**
      Adjusts the volume of the sound device based on the given direction.

      @param direction  Increase or decrease the current volume by a predefined
                        amount based on the direction (1 = increase, -1 =decrease)
    */
    void adjustVolume(Int8 direction) { }

  public:
    /**
      Saves the current state of this device to the given Serializer.

      @param out The serializer device to save to.
      @return The result of the save.  True on success, false on failure.
    */
    bool save(Serializer& out) const
    {
      out.putString("TIASound");

      for(int = 0; i < 6; ++i)
        out.putByte(0);

      // myLastRegisterSetCycle
      out.putInt(0);

      return true;
    }

    /**
      Loads the current state of this device from the given Serializer.

      @param in The Serializer device to load from.
      @return The result of the load.  True on success, false on failure.
    */
    bool load(Serializer& in)
    {
      if(in.getString() != "TIASound")
        return false;

      // Read sound registers and discard
      for(int = 0; i < 6; ++i)
        in.getByte();

      // myLastRegisterSetCycle
      in.getInt();

      return true;
    }

    /**
      Get a descriptor for this console class (used in error checking).

      @return The name of the object
    */
    string name() const { return "TIASound"; }
};

#endif
