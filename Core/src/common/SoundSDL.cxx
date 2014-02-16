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
// $Id: SoundSDL.cxx 2838 2014-01-17 23:34:03Z stephena $
//============================================================================

#ifdef SOUND_SUPPORT

#include <sstream>
#include <cassert>
#include <cmath>
#include <SDL.h>

#include "TIASnd.hxx"
#include "FrameBuffer.hxx"
#include "Settings.hxx"
#include "System.hxx"
#include "OSystem.hxx"
#include "Console.hxx"
#include "SoundSDL.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::SoundSDL(OSystem* osystem)
  : Sound(osystem),
    myIsEnabled(false),
    myIsInitializedFlag(false),
    myLastRegisterSetCycle(0),
    myNumChannels(0),
    myFragmentSizeLogBase2(0),
    myIsMuted(true),
    myVolume(100)
{
  myOSystem->logMessage("SoundSDL::SoundSDL started ...", 2);

  // The sound system is opened only once per program run, to eliminate
  // issues with opening and closing it multiple times
  // This fixes a bug most prevalent with ATI video cards in Windows,
  // whereby sound stopped working after the first video change
  SDL_AudioSpec desired;
  desired.freq   = myOSystem->settings().getInt("freq");
  desired.format = AUDIO_S16SYS;
  desired.channels = 2;
  desired.samples  = myOSystem->settings().getInt("fragsize");
  desired.callback = callback;
  desired.userdata = (void*)this;

  ostringstream buf;
  if(SDL_OpenAudio(&desired, &myHardwareSpec) < 0)
  {
    buf << "WARNING: Couldn't open SDL audio system! " << endl
        << "         " << SDL_GetError() << endl;
    myOSystem->logMessage(buf.str(), 0);
    return;
  }

  // Make sure the sample buffer isn't to big (if it is the sound code
  // will not work so we'll need to disable the audio support)
  if(((float)myHardwareSpec.samples / (float)myHardwareSpec.freq) >= 0.25)
  {
    buf << "WARNING: Sound device doesn't support realtime audio! Make "
        << "sure a sound" << endl
        << "         server isn't running.  Audio is disabled." << endl;
    myOSystem->logMessage(buf.str(), 0);

    SDL_CloseAudio();
    return;
  }

  // Pre-compute fragment-related variables as much as possible
  myFragmentSizeLogBase2 = log((double)myHardwareSpec.samples) / log(2.0);
  myFragmentSizeLogDiv1 = myFragmentSizeLogBase2 / 60.0;
  myFragmentSizeLogDiv2 = (myFragmentSizeLogBase2 - 1) / 60.0;

  myIsInitializedFlag = true;
  SDL_PauseAudio(1);

  myOSystem->logMessage("SoundSDL::SoundSDL initialized", 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::~SoundSDL()
{
  // Close the SDL audio system if it's initialized
  if(myIsInitializedFlag)
  {
    SDL_CloseAudio();
    myIsEnabled = myIsInitializedFlag = false;
  }

  myOSystem->logMessage("SoundSDL destroyed", 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::setEnabled(bool state)
{
  myOSystem->settings().setValue("sound", state);

  myOSystem->logMessage(state ? "SoundSDL::setEnabled(true)" : 
                                "SoundSDL::setEnabled(false)", 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::open()
{
  myOSystem->logMessage("SoundSDL::open started ...", 2);
  myIsEnabled = false;
  mute(true);
  if(!myIsInitializedFlag || !myOSystem->settings().getBool("sound"))
  {
    myOSystem->logMessage("Sound disabled\n", 1);
    return;
  }

  // Now initialize the TIASound object which will actually generate sound
  myTIASound.outputFrequency(myHardwareSpec.freq);
  const string& chanResult =
      myTIASound.channels(myHardwareSpec.channels, myNumChannels == 2);

  // Adjust volume to that defined in settings
  myVolume = myOSystem->settings().getInt("volume");
  setVolume(myVolume);

  // Show some info
  ostringstream buf;
  buf << "Sound enabled:"  << endl
      << "  Volume:      " << (int)myVolume << endl
      << "  Frag size:   " << (int)myHardwareSpec.samples << endl
      << "  Frequency:   " << (int)myHardwareSpec.freq << endl
      << "  Channels:    " << (int)myHardwareSpec.channels
                           << " (" << chanResult << ")" << endl
      << endl;
  myOSystem->logMessage(buf.str(), 1);

  // And start the SDL sound subsystem ...
  myIsEnabled = true;
  mute(false);

  myOSystem->logMessage("SoundSDL::open finished", 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::close()
{
  if(myIsInitializedFlag)
  {
    myIsEnabled = false;
    SDL_PauseAudio(1);
    myLastRegisterSetCycle = 0;
    myTIASound.reset();
    myRegWriteQueue.clear();
    myOSystem->logMessage("SoundSDL::close", 2);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::mute(bool state)
{
  if(myIsInitializedFlag)
  {
    myIsMuted = state;
    SDL_PauseAudio(myIsMuted ? 1 : 0);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::reset()
{
  if(myIsInitializedFlag)
  {
    SDL_PauseAudio(1);
    myLastRegisterSetCycle = 0;
    myTIASound.reset();
    myRegWriteQueue.clear();
    mute(myIsMuted);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::setVolume(Int32 percent)
{
  if(myIsInitializedFlag && (percent >= 0) && (percent <= 100))
  {
    myOSystem->settings().setValue("volume", percent);
    SDL_LockAudio();
    myVolume = percent;
    myTIASound.volume(percent);
    SDL_UnlockAudio();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::adjustVolume(Int8 direction)
{
  ostringstream strval;
  string message;

  Int32 percent = myVolume;

  if(direction == -1)
    percent -= 2;
  else if(direction == 1)
    percent += 2;

  if((percent < 0) || (percent > 100))
    return;

  setVolume(percent);

  // Now show an onscreen message
  strval << percent;
  message = "Volume set to ";
  message += strval.str();

  myOSystem->frameBuffer().showMessage(message);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::adjustCycleCounter(Int32 amount)
{
  myLastRegisterSetCycle += amount;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::setChannels(uInt32 channels)
{
  if(channels == 1 || channels == 2)
    myNumChannels = channels;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::setFrameRate(float framerate)
{
  // Recalculate since frame rate has changed
  // FIXME - should we clear out the queue or adjust the values in it?
  myFragmentSizeLogDiv1 = myFragmentSizeLogBase2 / framerate;
  myFragmentSizeLogDiv2 = (myFragmentSizeLogBase2 - 1) / framerate;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::set(uInt16 addr, uInt8 value, Int32 cycle)
{
  SDL_LockAudio();

  // First, calculate how many seconds would have past since the last
  // register write on a real 2600
  double delta = (((double)(cycle - myLastRegisterSetCycle)) / 
      (1193191.66666667));

  // Now, adjust the time based on the frame rate the user has selected. For
  // the sound to "scale" correctly, we have to know the games real frame 
  // rate (e.g., 50 or 60) and the currently emulated frame rate. We use these
  // values to "scale" the time before the register change occurs.
  RegWrite info;
  info.addr = addr;
  info.value = value;
  info.delta = delta;
  myRegWriteQueue.enqueue(info);

  // Update last cycle counter to the current cycle
  myLastRegisterSetCycle = cycle;

  SDL_UnlockAudio();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::processFragment(Int16* stream, uInt32 length)
{
  uInt32 channels = myHardwareSpec.channels;
  length = length / channels;

  // If there are excessive items on the queue then we'll remove some
  if(myRegWriteQueue.duration() > myFragmentSizeLogDiv1)
  {
    double removed = 0.0;
    while(removed < myFragmentSizeLogDiv2)
    {
      RegWrite& info = myRegWriteQueue.front();
      removed += info.delta;
      myTIASound.set(info.addr, info.value);
      myRegWriteQueue.dequeue();
    }
  }

  double position = 0.0;
  double remaining = length;

  while(remaining > 0.0)
  {
    if(myRegWriteQueue.size() == 0)
    {
      // There are no more pending TIA sound register updates so we'll
      // use the current settings to finish filling the sound fragment
      myTIASound.process(stream + ((uInt32)position * channels),
          length - (uInt32)position);

      // Since we had to fill the fragment we'll reset the cycle counter
      // to zero.  NOTE: This isn't 100% correct, however, it'll do for
      // now.  We should really remember the overrun and remove it from
      // the delta of the next write.
      myLastRegisterSetCycle = 0;
      break;
    }
    else
    {
      // There are pending TIA sound register updates so we need to
      // update the sound buffer to the point of the next register update
      RegWrite& info = myRegWriteQueue.front();

      // How long will the remaining samples in the fragment take to play
      double duration = remaining / (double)myHardwareSpec.freq;

      // Does the register update occur before the end of the fragment?
      if(info.delta <= duration)
      {
        // If the register update time hasn't already passed then
        // process samples upto the point where it should occur
        if(info.delta > 0.0)
        {
          // Process the fragment upto the next TIA register write.  We
          // round the count passed to process up if needed.
          double samples = (myHardwareSpec.freq * info.delta);
          myTIASound.process(stream + ((uInt32)position * channels),
              (uInt32)samples + (uInt32)(position + samples) - 
              ((uInt32)position + (uInt32)samples));

          position += samples;
          remaining -= samples;
        }
        myTIASound.set(info.addr, info.value);
        myRegWriteQueue.dequeue();
      }
      else
      {
        // The next register update occurs in the next fragment so finish
        // this fragment with the current TIA settings and reduce the register
        // update delay by the corresponding amount of time
        myTIASound.process(stream + ((uInt32)position * channels),
            length - (uInt32)position);
        info.delta -= duration;
        break;
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::callback(void* udata, uInt8* stream, int len)
{
  SoundSDL* sound = (SoundSDL*)udata;
  if(sound->myIsEnabled)
  {
    // The callback is requesting 8-bit (unsigned) data, but the TIA sound
    // emulator deals in 16-bit (signed) data
    // So, we need to convert the pointer and half the length
    sound->processFragment((Int16*)stream, (uInt32)len >> 1);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundSDL::save(Serializer& out) const
{
  try
  {
    out.putString(name());

    uInt8 reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg5 = 0, reg6 = 0;

    // Only get the TIA sound registers if sound is enabled
    if(myIsInitializedFlag)
    {
      reg1 = myTIASound.get(0x15);
      reg2 = myTIASound.get(0x16);
      reg3 = myTIASound.get(0x17);
      reg4 = myTIASound.get(0x18);
      reg5 = myTIASound.get(0x19);
      reg6 = myTIASound.get(0x1a);
    }

    out.putByte(reg1);
    out.putByte(reg2);
    out.putByte(reg3);
    out.putByte(reg4);
    out.putByte(reg5);
    out.putByte(reg6);

    out.putInt(myLastRegisterSetCycle);
  }
  catch(...)
  {
    myOSystem->logMessage("ERROR: SoundSDL::save", 0);
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundSDL::load(Serializer& in)
{
  try
  {
    if(in.getString() != name())
      return false;

    uInt8 reg1 = in.getByte(),
          reg2 = in.getByte(),
          reg3 = in.getByte(),
          reg4 = in.getByte(),
          reg5 = in.getByte(),
          reg6 = in.getByte();

    myLastRegisterSetCycle = (Int32) in.getInt();

    // Only update the TIA sound registers if sound is enabled
    // Make sure to empty the queue of previous sound fragments
    if(myIsInitializedFlag)
    {
      SDL_PauseAudio(1);
      myRegWriteQueue.clear();
      myTIASound.set(0x15, reg1);
      myTIASound.set(0x16, reg2);
      myTIASound.set(0x17, reg3);
      myTIASound.set(0x18, reg4);
      myTIASound.set(0x19, reg5);
      myTIASound.set(0x1a, reg6);
      if(!myIsMuted) SDL_PauseAudio(0);
    }
  }
  catch(...)
  {
    myOSystem->logMessage("ERROR: SoundSDL::load", 0);
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::RegWriteQueue::RegWriteQueue(uInt32 capacity)
  : myCapacity(capacity),
    myBuffer(0),
    mySize(0),
    myHead(0),
    myTail(0)
{
  myBuffer = new RegWrite[myCapacity];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::RegWriteQueue::~RegWriteQueue()
{
  delete[] myBuffer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::RegWriteQueue::clear()
{
  myHead = myTail = mySize = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::RegWriteQueue::dequeue()
{
  if(mySize > 0)
  {
    myHead = (myHead + 1) % myCapacity;
    --mySize;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double SoundSDL::RegWriteQueue::duration()
{
  double duration = 0.0;
  for(uInt32 i = 0; i < mySize; ++i)
  {
    duration += myBuffer[(myHead + i) % myCapacity].delta;
  }
  return duration;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::RegWriteQueue::enqueue(const RegWrite& info)
{
  // If an attempt is made to enqueue more than the queue can hold then
  // we'll enlarge the queue's capacity.
  if(mySize == myCapacity)
    grow();

  myBuffer[myTail] = info;
  myTail = (myTail + 1) % myCapacity;
  ++mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::RegWrite& SoundSDL::RegWriteQueue::front()
{
  assert(mySize != 0);
  return myBuffer[myHead];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundSDL::RegWriteQueue::size() const
{
  return mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::RegWriteQueue::grow()
{
  RegWrite* buffer = new RegWrite[myCapacity * 2];
  for(uInt32 i = 0; i < mySize; ++i)
  {
    buffer[i] = myBuffer[(myHead + i) % myCapacity];
  }
  myHead = 0;
  myTail = mySize;
  myCapacity = myCapacity * 2;
  delete[] myBuffer;
  myBuffer = buffer;
}

#endif  // SOUND_SUPPORT
