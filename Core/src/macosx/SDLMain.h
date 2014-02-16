/*   SDLMain.h - main entry point for our Cocoa-ized SDL app
       Initial Version: Darrell Walisser <dwaliss1@purdue.edu>
       Non-NIB-Code & other changes: Max Horn <max@quendi.de>

    Feel free to customize this file to suit your needs
*/
/* $Id: SDLMain.h 2007 2010-04-13 12:32:08Z stephena $ */

#import <Cocoa/Cocoa.h>

@interface SDLMain : NSObject
{
}
+ (SDLMain *)sharedInstance;

@end
