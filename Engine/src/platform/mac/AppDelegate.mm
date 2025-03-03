#import "AppDelegate.h"
#include "platform/PlatformRunLoop.h"
#include "Window.h"
#include "engine/Application.h"

@implementation ViewDelegate

- (void) drawInMTKView:(MTKView *)view
{
    se::PlatformRunLoop::Get()->Update();
}

- (void) mtkView:(MTKView*)view drawableSizeWillChange:(CGSize) size
{

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