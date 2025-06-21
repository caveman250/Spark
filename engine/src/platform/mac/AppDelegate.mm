#import "AppDelegate.h"
#include "platform/PlatformRunLoop.h"
#include "Window.h"
#include "engine/Application.h"

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
    PROFILE_BEGIN_FRAME()
    PROFILE_BEGIN_THREAD()
    se::PlatformRunLoop::Get()->Update();
}

- (void) mtkView:(MTKView*)view drawableSizeWillChange:(CGSize) size
{
    if(auto window = se::Application::Get()->GetPrimaryWindow())
    {
        window->OnResize(size.width, size.height);
    }
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
    auto window = static_cast<se::mac::Window*>(se::Application::Get()->GetPrimaryWindow());
    _window = window->GetNativeWindow();
    auto view= [_window contentView];
    [view setDelegate:_viewDelegate];

    NSApplication* app = reinterpret_cast<NSApplication*>([aNotification object]);
    [app activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

@end