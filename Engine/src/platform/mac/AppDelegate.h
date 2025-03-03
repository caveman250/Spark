#pragma once

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

@interface ViewDelegate : NSObject <MTKViewDelegate>

@end

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (retain) ViewDelegate* viewDelegate;
@property (assign) IBOutlet NSWindow *window;

@end
