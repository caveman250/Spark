#import "AppDelegate.h"
#include "platform/PlatformRunLoop.h"
#include "Window.h"
#include "ObjCUtil.h"

@implementation SparkView

- (void)createTrackingArea
{
    NSTrackingAreaOptions options =  NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingCursorUpdate;
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:self.bounds options:options owner:self userInfo:nil];
    [self addTrackingArea:area];
}

- (void)cursorUpdate:(NSEvent *)event
{
    [_currentCursor set];
}

@end

@implementation ViewDelegate

- (void) drawInMTKView:(MTKView *)view
{
    se::PlatformRunLoop::Get()->Update();
}

- (void) mtkView:(MTKView*)view drawableSizeWillChange:(CGSize) size
{
    se::platform::mac::OnWindowResize(size.width, size.height);
}

@end

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    NSApplication* app = reinterpret_cast<NSApplication*>([aNotification object]);
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    se::PlatformRunLoop::Get()->Init();

    _viewDelegate = [ViewDelegate new];
    _window = se::platform::mac::GetNativeWindow();
    auto view= [_window contentView];
    [view setDelegate:_viewDelegate];

    NSApplication* app = reinterpret_cast<NSApplication*>([aNotification object]);
    [app activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

- (void) applicationWillTerminate:(NSNotification *)aNotification
{
  se::PlatformRunLoop::Get()->Shutdown();
}

@end