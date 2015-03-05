/*
 Copyright (c) 2014, OpenEmu Team
 

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
static uint32_t tiaSamplesPerFrame = 0;
Int16 *sampleBuffer[2048];
#include "Stubs.hh"

static Console *console = 0;
static Cartridge *cartridge = 0;
static OSystem osystem;
static StateManager stateManager(&osystem);
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
    int16_t pad[2][OE2600ButtonCount];
    NSString *romName;
    double sampleRate;
}

@end

StellaGameCore *current;
@implementation StellaGameCore

- (id)init
{
    if((self = [super init]))
    {
        if(videoBuffer)
            free(videoBuffer);
        videoBuffer = (uint32_t*)malloc(160 * 256 * 4);
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
    
    console->controller(Controller::Left).update();
    console->controller(Controller::Right).update();
    console->switches().update();
    
    TIA& tia = console->tia();
    tia.update();
    
    // Video
    videoWidth = tia.width();
    videoHeight = tia.height();
    
    for (unsigned int i = 0; i < videoHeight * videoWidth; ++i)
        videoBuffer[i] = Palette[tia.currentFrameBuffer()[i]];
    
    // Audio
    vcsSound->processFragment((Int16*)sampleBuffer, tiaSamplesPerFrame);
    [[current ringBufferAtIndex:0] write:sampleBuffer maxLength:tiaSamplesPerFrame << 2];
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
    
    // Get the game properties
    string cartMD5 = MD5((const uInt8*)data, size);
    Properties props;
    osystem.propSet().getMD5(cartMD5, props);
    
    // Load the cart
    string cartType = props.get(Cartridge_Type);
    string cartId;//, romType("AUTO-DETECT");
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
    
    return YES;
}

- (oneway void)didPush2600Button:(OE2600Button)button forPlayer:(NSUInteger)player;
{
    Event &ev = osystem.eventHandler().event();
    int playerShift = player != 1 ? 7 : 0;
    
    switch (button) {
        case OE2600ButtonUp:
            ev.set(Event::Type(Event::JoystickZeroUp + playerShift), 1);
            break;
        case OE2600ButtonDown:
            ev.set(Event::Type(Event::JoystickZeroDown + playerShift), 1);
            break;
        case OE2600ButtonLeft:
            ev.set(Event::Type(Event::JoystickZeroLeft + playerShift), 1);
            break;
        case OE2600ButtonRight:
            ev.set(Event::Type(Event::JoystickZeroRight + playerShift), 1);
            break;
        case OE2600ButtonFire1:
            ev.set(Event::Type(Event::JoystickZeroFire + playerShift), 1);
            break;
        case OE2600ButtonLeftDiffA:
            ev.set(Event::Type(Event::ConsoleLeftDiffA), 1);
            break;
        case OE2600ButtonLeftDiffB:
            ev.set(Event::Type(Event::ConsoleLeftDiffB), 1);
            break;
        case OE2600ButtonRightDiffA:
            ev.set(Event::Type(Event::ConsoleRightDiffA), 1);
            break;
        case OE2600ButtonRightDiffB:
            ev.set(Event::Type(Event::ConsoleRightDiffB), 1);
            break;
        case OE2600ButtonTVTypeColor:
            ev.set(Event::Type(Event::ConsoleColor), 1);
            break;
        case OE2600ButtonTVTypeBlackAndWhite:
            ev.set(Event::Type(Event::ConsoleBlackWhite), 1);
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
    Event &ev = osystem.eventHandler().event();
    int playerShift = player != 1 ? 7 : 0;
    
    switch (button) {
        case OE2600ButtonUp:
            ev.set(Event::Type(Event::JoystickZeroUp + playerShift), 0);
            break;
        case OE2600ButtonDown:
            ev.set(Event::Type(Event::JoystickZeroDown + playerShift), 0);
            break;
        case OE2600ButtonLeft:
            ev.set(Event::Type(Event::JoystickZeroLeft + playerShift), 0);
            break;
        case OE2600ButtonRight:
            ev.set(Event::Type(Event::JoystickZeroRight + playerShift), 0);
            break;
        case OE2600ButtonFire1:
            ev.set(Event::Type(Event::JoystickZeroFire + playerShift), 0);
            break;
        case OE2600ButtonLeftDiffA:
            ev.set(Event::Type(Event::ConsoleLeftDiffA), 0);
            break;
        case OE2600ButtonLeftDiffB:
            ev.set(Event::Type(Event::ConsoleLeftDiffB), 0);
            break;
        case OE2600ButtonRightDiffA:
            ev.set(Event::Type(Event::ConsoleRightDiffA), 0);
            break;
        case OE2600ButtonRightDiffB:
            ev.set(Event::Type(Event::ConsoleRightDiffB), 0);
            break;
        case OE2600ButtonTVTypeColor:
            ev.set(Event::Type(Event::ConsoleColor), 0);
            break;
        case OE2600ButtonTVTypeBlackAndWhite:
            ev.set(Event::Type(Event::ConsoleBlackWhite), 0);
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
    return OEIntRectMake(0, 0, videoWidth, videoHeight);
    
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

- (NSData *)serializeStateWithError:(NSError **)outError
{
    Serializer serializer;
    if(stateManager.saveState(serializer))
    {
        serializer.myStream->seekg(0, std::ios::end);
        NSUInteger length = serializer.myStream->tellg();
        serializer.myStream->seekg(0, std::ios::beg);
        
        char *bytes = (char *)malloc(length);
        serializer.myStream->read(bytes, length);
        
        return [NSData dataWithBytes:bytes length:length];
    }
    else
    {
        NSError *error = [NSError errorWithDomain:OEGameCoreErrorDomain
                                             code:OEGameCoreCouldNotSaveStateError
                                         userInfo:@{
                                                    NSLocalizedDescriptionKey : @"Could not serialize save state data"
                                                    }];
        
        if(outError)
        {
            *outError = error;
        }
        return nil;
    }
}

- (BOOL)deserializeState:(NSData *)state withError:(NSError **)outError
{
    char const *bytes = (char const *)([state bytes]);
    std::streamsize size = [state length];
    
    Serializer serializer;
    serializer.myStream->write(bytes, size);
    
    if(stateManager.loadState(serializer))
    {
        return YES;
    }
    else
    {
        NSError *error = [NSError errorWithDomain:OEGameCoreErrorDomain
                                             code:OEGameCoreCouldNotLoadStateError
                                         userInfo:@{
                                                    NSLocalizedDescriptionKey : @"The save state data could not be loaded"
                                                    }];
        
        if(outError)
        {
            *outError = error;
        }
        return NO;
    }
}

- (BOOL)saveStateToFileAtPath:(NSString *)fileName
{
    Serializer state([fileName UTF8String], 0);
    if(!stateManager.saveState(state))
    {
        return NO;
    }
    return YES;
}

- (BOOL)loadStateFromFileAtPath:(NSString *)fileName
{
    Serializer state([fileName UTF8String], 1);
    if(!stateManager.loadState(state))
    {
        return NO;
    }
    return YES;
}

- (void)changeDisplayMode
{
    console->toggleFormat();
}

@end
