#import "NativeWindow.h"
#import "engine/input/Key.h"
#import "KeyMap.h"
#import "engine/input/InputComponent.h"

@implementation NativeWindow

-(void)initKeyReceiver
{
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskFlagsChanged|NSEventMaskKeyDown|NSEventMaskKeyUp handler:^NSEvent * (NSEvent * theEvent)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

        auto type = [theEvent type];
        bool modifierFlagsChanged = (NSEventMaskFromType(type) & NSEventMaskFlagsChanged) != 0;
        bool keyDown = (NSEventMaskFromType(type) & NSEventMaskKeyDown) != 0;

        auto keyCode = [theEvent keyCode];
        se::input::Key key = se::mac::KeyMap::MacKeyToSparkKey(keyCode);
        if (modifierFlagsChanged)
        {
            NSEventModifierFlags modifiers = [theEvent modifierFlags];

            switch (key)
            {
                case se::input::Key::LeftSuper:
                case se::input::Key::RightSuper:
                    keyDown = modifiers & NSEventModifierFlagCommand;
                    break;
                case se::input::Key::LeftShift:
                case se::input::Key::RightShift:
                    keyDown = modifiers & NSEventModifierFlagShift;
                    break;
                case se::input::Key::LeftControl:
                case se::input::Key::RightControl:
                    keyDown = modifiers & NSEventModifierFlagControl;
                    break;
                case se::input::Key::LeftAlt:
                case se::input::Key::RightAlt:
                    keyDown = modifiers & NSEventModifierFlagOption;
                    break;
                default:
                    return theEvent;
            }

            se::input::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.state = keyDown ? se::input::KeyState::Down : se::input::KeyState::Up;

            inputComp->keyEvents.push_back(keyEvent);
            inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
        }
        else
        {
           inputComp->keyStates[static_cast<int>(key)] = keyDown ?
                    se::input::KeyState::Down :
                    se::input::KeyState::Up;

            se::input::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.state = keyDown ? se::input::KeyState::Down : se::input::KeyState::Up;
            inputComp->keyEvents.push_back(keyEvent);
        }

        return theEvent;
    }];
}

- (void)mouseDown:(NSEvent*) event
{
    auto app = se::Application::Get();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] = se::input::KeyState::Down;

    se::input::MouseEvent mouseEvent;
    mouseEvent.button = se::input::MouseButton::Left;
    mouseEvent.state = se::input::KeyState::Down;
    inputComp->mouseEvents.push_back(mouseEvent);
}

- (void)rightMouseDown:(NSEvent*) event
{
    auto app = se::Application::Get();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] = se::input::KeyState::Up;

    se::input::MouseEvent mouseEvent;
    mouseEvent.button = se::input::MouseButton::Left;
    mouseEvent.state = se::input::KeyState::Up;
    inputComp->mouseEvents.push_back(mouseEvent);
}

- (void)mouseUp:(NSEvent*) event
{
    auto app = se::Application::Get();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] = se::input::KeyState::Up;

    se::input::MouseEvent mouseEvent;
    mouseEvent.button = se::input::MouseButton::Left;
    mouseEvent.state = se::input::KeyState::Up;
    inputComp->mouseEvents.push_back(mouseEvent);
}

- (void)rightMouseUp:(NSEvent*) event
{
    auto app = se::Application::Get();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Right)] = se::input::KeyState::Up;

    se::input::MouseEvent mouseEvent;
    mouseEvent.button = se::input::MouseButton::Right;
    mouseEvent.state = se::input::KeyState::Up;
    inputComp->mouseEvents.push_back(mouseEvent);
}

- (void)scrollWheel:(NSEvent*)event
{
    auto app = se::Application::Get();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    inputComp->mouseScrollDelta = [event scrollingDeltaY];

    se::input::MouseEvent mouseEvent;
    mouseEvent.button = se::input::MouseButton::None;
    mouseEvent.scrollDelta = -inputComp->mouseScrollDelta;
    inputComp->mouseEvents.push_back(mouseEvent);
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
    auto app = se::Application::Get();
    auto window = app->GetWindow();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    inputComp->mouseX = locationInView.x * window->GetContentScale();
    inputComp->mouseY = ([[self contentView] frame].size.height - locationInView.y) * window->GetContentScale();
}

- (void)mouseDragged:(NSEvent*)event
{
    auto app = se::Application::Get();
    auto window = app->GetWindow();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

    if (inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] != se::input::KeyState::Down)
    {
        inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] = se::input::KeyState::Down;

        se::input::MouseEvent mouseEvent;
        mouseEvent.button = se::input::MouseButton::Left;
        mouseEvent.state = se::input::KeyState::Down;
        inputComp->mouseEvents.push_back(mouseEvent);
    }

    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    inputComp->mouseX = locationInView.x * window->GetContentScale();
    inputComp->mouseY = ([[self contentView] frame].size.height - locationInView.y) * window->GetContentScale();
}

@end