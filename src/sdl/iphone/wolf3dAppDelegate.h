/*
 
 Copyright (C) 2009 Id Software, Inc.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 
 */

#import <UIKit/UIKit.h>
#import <UIKit/UIAccelerometer.h>

#ifndef IPHONE_APPSTORE
#import "AltAds.h"

@interface SplashView : UIImageView
{
}
@end
#endif

#ifdef _cplusplus
extern "C" {
#endif
void vibrateDevice();
#ifdef _cplusplus
}
#endif

@class EAGLView;

@interface wolf3dAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate> {
    UIWindow *window;
    EAGLView *glView;
	int		lastAccelUpdateMsec;
#ifndef IPHONE_APPSTORE
  SplashView* splashView;
  AltAds* altAds;
#endif
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

- (void)restartAccelerometerIfNeeded;
#ifndef IPHONE_APPSTORE
- (void)startUp;
#endif

@end

