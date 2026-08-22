#import "NativeWindow.h"
#import "engine/input/Key.h"
#import "KeyMap.h"
#import "engine/input/InputComponent.h"
#include "platform/IWindow.h"
#include "ObjCUtil.h"

@implementation NativeWindow

-(void)initKeyReceiver
{
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskFlagsChanged|NSEventMaskKeyDown|NSEventMaskKeyUp handler:^NSEvent * (NSEvent * theEvent)
    {


        auto type = [theEvent type];
        bool modifierFlagsChanged = (NSEventMaskFromType(type) & NSEventMaskFlagsChanged) != 0;
        NSEventModifierFlags modifiers = [theEvent modifierFlags];
        bool keyDown = (NSEventMaskFromType(type) & NSEventMaskKeyDown) != 0;

        auto keyCode = [theEvent keyCode];

        se::platform::mac::OnKeyStateChange(keyCode, modifierFlagsChanged, modifiers, keyDown);

        return theEvent;
    }];
}

- (void)keyDown:(NSEvent*) event
{
    // ignore
}

- (void)keyUp:(NSEvent*) event
{
    // ignore
}

- (void)mouseDown:(NSEvent*) event
{
    se::platform::mac::OnLeftMouseDown();
}

- (void)rightMouseDown:(NSEvent*) event
{
    se::platform::mac::OnRightMouseDown();
}

- (void)mouseUp:(NSEvent*) event
{
    se::platform::mac::OnLeftMouseUp();
}

- (void)rightMouseUp:(NSEvent*) event
{
    se::platform::mac::OnRightMouseUp();
}

- (void)scrollWheel:(NSEvent*)event
{
    se::platform::mac::OnMouseScroll([event scrollingDeltaY]);
}

- (void)mouseEntered:(NSEvent *)event
{
    [super mouseEntered:event];
    [[NSCursor pointingHandCursor] set];
}

- (void)mouseExited:(NSEvent *)event
{
    [super mouseExited:event];
    [[NSCursor arrowCursor] set];
}

- (void)mouseMoved:(NSEvent*)event
{
    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    se::platform::mac::OnMouseMove(locationInView.x, ([[self contentView] frame].size.height - locationInView.y));
}

- (void)mouseDragged:(NSEvent*)event
{
    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    se::platform::mac::OnMouseDragged(locationInView.x, ([[self contentView] frame].size.height - locationInView.y));
}

- (void)rightMouseDragged:(NSEvent*)event
{
    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    se::platform::mac::OnRightMouseDragged(locationInView.x, ([[self contentView] frame].size.height - locationInView.y));
}

@end