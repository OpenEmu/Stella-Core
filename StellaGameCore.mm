/*
 Copyright (c) 2013, OpenEmu Team
 

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the OpenEmu Team nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY OpenEmu Team ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL OpenEmu Team BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "StellaGameCore.h"
#import <OpenEmuBase/OERingBuffer.h>
#import "OE2600SystemResponderClient.h"
#import <OpenGL/gl.h>

#include "Console.hxx"
#include "Cart.hxx"
#include "Props.hxx"
#include "MD5.hxx"
#include "Sound.hxx"
#include "SerialPort.hxx"
#include "TIA.hxx"
#include "Switches.hxx"
#include "StateManager.hxx"
#include "PropsSet.hxx"
#include "Paddles.hxx"
#include "SoundSDL.hxx"

static SoundSDL *vcsSound = 0;
static uint32_t tiaSoundRate = 0, tiaSamplesPerFrame = 0;
Int16 *sampleBuffer[2048];
#include "Stubs.hh"

static Console *console = 0;
static Cartridge *cartridge = 0;
static OSystem osystem;
static StateManager stateManager(&osystem);
//static Settings *settings = new Settings(&osystem);
static bool p1DiffB = 1, p2DiffB = 1, vcsColor = 1;
const uint32_t* Palette;

// Set the palette for the current stella instance
void stellaOESetPalette (const uInt32* palette)
{
    Palette = palette;
}

@interface StellaGameCore () <OE2600SystemResponderClient>
{
    uint32_t *videoBuffer;
    int videoWidth, videoHeight;
    int16_t pad[2][12];
    NSString *romName;
    double sampleRate;
    BOOL isPAL;
}

@end

//NSUInteger A2600EmulatorValues[] = { RETRO_DEVICE_ID_JOYPAD_UP, RETRO_DEVICE_ID_JOYPAD_DOWN, RETRO_DEVICE_ID_JOYPAD_LEFT, RETRO_DEVICE_ID_JOYPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_B, RETRO_DEVICE_ID_JOYPAD_L, RETRO_DEVICE_ID_JOYPAD_L2, RETRO_DEVICE_ID_JOYPAD_R, RETRO_DEVICE_ID_JOYPAD_R2, RETRO_DEVICE_ID_JOYPAD_START, RETRO_DEVICE_ID_JOYPAD_SELECT };

StellaGameCore *current;
@implementation StellaGameCore

- (id)init
{
    if((self = [super init]))
    {
        if(videoBuffer)
            free(videoBuffer);
        videoBuffer = (uint32_t*)malloc(160 * 256 * 4); // 320x210 ?
    }
    
	current = self;
    
	return self;
}

#pragma mark Exectuion

- (void)executeFrame
{
    [self executeFrameSkippingFrame:NO];
}

- (void)executeFrameSkippingFrame: (BOOL) skip
{
    tiaSamplesPerFrame = 31400.0f/console->getFramerate();
    //Event &ev = osystem.eventHandler().event();
    //console->event().set(Event::ConsoleReset, input_state_cb(Controller::Left, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START));
    
    //ev.set(Event::Type(Event::JoystickZeroFire + playerShift), state == INPUT_PUSHED);
    //ev.set(Event::Type(Event::ConsoleReset), 1);
    
    console->controller(Controller::Left).update();
    console->controller(Controller::Right).update();
    console->switches().update();
    
    TIA& tia = console->tia();
    tia.update();
    
    // Video
    videoWidth = tia.width();
    videoHeight = tia.height();
    
    uint8* currentFrame = tia.currentFrameBuffer() /*+ (tia.ystart() * 160)*/;
    for ( unsigned int i = 0; i < videoHeight * videoWidth; ++i )
    //for ( unsigned int i = 0; i < frameHeight * 160; ++i )
        //videoBuffer[i] = Palette[tia.currentFrameBuffer()[i]];
        videoBuffer[i] = Palette[currentFrame[i]];
    
    // Audio
    vcsSound->processFragment((Int16*)sampleBuffer, tiaSamplesPerFrame);
    [[current ringBufferAtIndex:0] write:sampleBuffer maxLength:tiaSamplesPerFrame << 2];
    
    isPAL = tia.isPAL();
    //NSLog(@"Value: %d", isPAL);//console myDisplayFormat
    //NSLog(@"Framerate: %f", console->getFramerate());
}

- (BOOL)loadFileAtPath:(NSString *)path error:(NSError **)error
{
	memset(pad, 0, sizeof(int16_t) * 10);
    
    const void *data;
    size_t size;
    romName = [path copy];
    
    // load cart, read bytes, get length
    NSData* dataObj = [NSData dataWithContentsOfFile:[romName stringByStandardizingPath]];
    if(dataObj == nil) return false;
    size = [dataObj length];
    data = (uint8_t*)[dataObj bytes];
    
//    //Input - Set paddles for games that require them (Range: 1-10)
//    Paddles::setDigitalSensitivity(5);
//    Paddles::setMouseSensitivity(5);
    
    // Get the game properties
    string cartMD5 = MD5((const uInt8*)data, size);
    Properties props;
    osystem.propSet().getMD5(cartMD5, props);
    //PropertiesSet propslist(0);
    //propslist.getMD5(cartMD5, props);
    
    // Load the cart
    string cartType = props.get(Cartridge_Type);
    string cartId;//, romType("AUTO-DETECT");
    //Settings settings = osystem.settings();
    Settings *settings = new Settings(&osystem);
    settings->setValue("romloadcount", 0);
    cartridge = Cartridge::create((const uInt8*)data, (uInt32)size, cartMD5, cartType, cartId, osystem, *settings);
    if(cartridge == 0)
    {
        return NO;
    }
    console = new Console(&osystem, cartridge, props);
    osystem.myConsole = console;
    
    //tia.enableAutoFrame(false);
    console->initializeVideo();
    console->initializeAudio();
    
    // Get the ROM's width and height
    TIA& tia = console->tia();
    videoWidth = tia.width();
    videoHeight = tia.height();
    
    // Config audio
    tiaSoundRate = 31400;
    //tiaSamplesPerFrame = tiaSoundRate/60.;
    //tiaSamplesPerFrame = 31400.0f/59.92;
    //vcsSound->tiaSound().outputFrequency(tiaSoundRate);
    return YES;
}

- (oneway void)didPush2600Button:(OE2600Button)button forPlayer:(NSUInteger)player;
{
    //pad[player-1][A2600EmulatorValues[button]] = 1;
    Event &ev = osystem.eventHandler().event();
    //console->event().set(Event::ConsoleReset, input_state_cb(Controller::Left, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START));
    
    //ev.set(Event::Type(Event::JoystickZeroFire + playerShift), state == INPUT_PUSHED);
    //ev.set(Event::Type(Event::ConsoleReset), 1);
    
    switch (button) {
        case OE2600ButtonUp:
            ev.set(Event::Type(Event::JoystickZeroUp), 1);
            break;
        case OE2600ButtonDown:
            ev.set(Event::Type(Event::JoystickZeroDown), 1);
            break;
        case OE2600ButtonLeft:
            ev.set(Event::Type(Event::JoystickZeroLeft), 1);
            break;
        case OE2600ButtonRight:
            ev.set(Event::Type(Event::JoystickZeroRight), 1);
            break;
        case OE2600ButtonFire1:
            ev.set(Event::Type(Event::JoystickZeroFire), 1);
            break;
        case OE2600ButtonReset:
            ev.set(Event::Type(Event::ConsoleReset), 1);
            break;
        case OE2600ButtonSelect:
            ev.set(Event::Type(Event::ConsoleSelect), 1);
            break;
            
        default:
            break;
    }
}

- (oneway void)didRelease2600Button:(OE2600Button)button forPlayer:(NSUInteger)player;
{
    //pad[player-1][A2600EmulatorValues[button]] = 0;
    Event &ev = osystem.eventHandler().event();
    //console->event().set(Event::ConsoleReset, input_state_cb(Controller::Left, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START));
    
    //ev.set(Event::Type(Event::JoystickZeroFire + playerShift), state == INPUT_PUSHED);
    //ev.set(Event::Type(Event::ConsoleReset), 0);
    
    switch (button) {
        case OE2600ButtonUp:
            ev.set(Event::Type(Event::JoystickZeroUp), 0);
            break;
        case OE2600ButtonDown:
            ev.set(Event::Type(Event::JoystickZeroDown), 0);
            break;
        case OE2600ButtonLeft:
            ev.set(Event::Type(Event::JoystickZeroLeft), 0);
            break;
        case OE2600ButtonRight:
            ev.set(Event::Type(Event::JoystickZeroRight), 0);
            break;
        case OE2600ButtonFire1:
            ev.set(Event::Type(Event::JoystickZeroFire), 0);
            break;
        case OE2600ButtonReset:
            ev.set(Event::Type(Event::ConsoleReset), 0);
            break;
        case OE2600ButtonSelect:
            ev.set(Event::Type(Event::ConsoleSelect), 0);
            break;
            
        default:
            break;
    }
    
}

#pragma mark Video
- (const void *)videoBuffer
{
    return videoBuffer;
}

- (OEIntRect)screenRect
{
    return OEIntRectMake(0, 0, videoWidth, videoHeight); //160w x 210h    160x250
    
}

- (OEIntSize)bufferSize
{
    return OEIntSizeMake(160, 256);
}

- (OEIntSize)aspectSize
{
    return OEIntSizeMake(160 * 2, videoHeight);
}

- (void)setupEmulation
{
}

- (void)resetEmulation
{
    console->system().reset();
}

- (void)stopEmulation
{
    [super stopEmulation];
}

- (void)dealloc
{
    free(videoBuffer);
    free(sampleBuffer);
    [super dealloc];
}

- (GLenum)pixelFormat
{
    return GL_BGRA;
}

- (GLenum)pixelType
{
    return GL_UNSIGNED_INT_8_8_8_8_REV;
}

- (GLenum)internalPixelFormat
{
    return GL_RGB8;
}

- (double)audioSampleRate
{
    return 31400;
}

- (NSTimeInterval)frameInterval
{
    return console->getFramerate();
}

- (NSUInteger)channelCount
{
    return 2;
}

- (BOOL)saveStateToFileAtPath:(NSString *)fileName
{
    Serializer state([fileName UTF8String], 0);
    if(!stateManager.saveState(state))
    {
        return NO;
    }
    return YES;
    //return NO;
}

- (BOOL)loadStateFromFileAtPath:(NSString *)fileName
{
    Serializer state([fileName UTF8String], 1);
    if(!stateManager.loadState(state))
    {
        return NO;
    }
    return YES;
    //return NO;
}

- (void)changeDisplayMode
{
    console->toggleFormat();
}

@end
