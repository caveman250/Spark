#include "RunLoop.h"
#import <Cocoa/Cocoa.h>
#import <platform/mac/AppDelegate.h>

namespace se::mac::wrappers
{
    void RunLoop::Run()
    {
        @autoreleasepool {
            NSApplication* app = [NSApplication sharedApplication];
            AppDelegate* appDelegate = [[AppDelegate alloc] init];

            [app setDelegate:appDelegate];

            [app run];
        }
    }

    void RunLoop::Shutdown()
    {
        NSApplication* app = [NSApplication sharedApplication];
        [app stop:0];
    }
}
