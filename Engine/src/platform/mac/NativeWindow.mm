#import "NativeWindow.h"
#import "spark.h"
#import "engine/input/Key.h"
#import "KeyMap.h"

static std::map<uint32_t, bool> s_ModifierKeyStates = {}; // TODO

@implementation NativeWindow

-(void)initKeyReceiver
{
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskFlagsChanged|NSEventMaskKeyDown handler:^NSEvent * (NSEvent * theEvent)
    {
        auto keyCode = [theEvent keyCode];
        if (keyCode == 0x38 || keyCode == 0x3C ||
            keyCode == 0x3B || keyCode == 0x3E ||
            keyCode == 0x3A || keyCode == 0x3D)
        {
            s_ModifierKeyStates[keyCode] = !s_ModifierKeyStates[keyCode];
            se::debug::Log::Info("{} {}", se::input::Key::ToString(se::mac::KeyMap::MacKeyToSparkKey(keyCode)), s_ModifierKeyStates[keyCode] ? "Down" : "Up");
        }

        return theEvent;
    }];
}

- (void)keyDown:(NSEvent*) event
{
    se::debug::Log::Info("{} Down", se::input::Key::ToString(se::mac::KeyMap::MacKeyToSparkKey([event keyCode])));
}

- (void)keyUp:(NSEvent*) event
{
    se::debug::Log::Info("{} Up", se::input::Key::ToString(se::mac::KeyMap::MacKeyToSparkKey([event keyCode])));
}

- (void)mouseDown:(NSEvent*) event
{
    se::debug::Log::Info("Left Mouse Down");
}

- (void)rightMouseDown:(NSEvent*) event
{
    se::debug::Log::Info("Right Mouse Down");
}

- (void)mouseUp:(NSEvent*) event
{
    se::debug::Log::Info("Left Mouse Up");
}

- (void)rightMouseUp:(NSEvent*) event
{
   se::debug::Log::Info("Right Mouse Up");
}

- (void)scrollWheel:(NSEvent*)event
{
    se::debug::Log::Info("Mouse Scroll delta X: {}, delta Y: {}", [event scrollingDeltaX], [event scrollingDeltaY]);
}

@end