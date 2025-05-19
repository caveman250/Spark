#import "NativeWindow.h"
#import "engine/input/Key.h"
#import "KeyMap.h"
#import "engine/input/InputComponent.h"

static std::map<uint32_t, bool> s_ModifierKeyStates = {}; // TODO

@implementation NativeWindow

-(void)initKeyReceiver
{
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskFlagsChanged|NSEventMaskKeyDown handler:^NSEvent * (NSEvent * theEvent)
    {
        auto app = se::Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

        auto keyCode = [theEvent keyCode];
        if (keyCode == 0x38 || keyCode == 0x3C ||
            keyCode == 0x3B || keyCode == 0x3E ||
            keyCode == 0x3A || keyCode == 0x3D)
        {
            s_ModifierKeyStates[keyCode] = !s_ModifierKeyStates[keyCode];

            se::input::Key key = se::mac::KeyMap::MacKeyToSparkKey(keyCode);

            se::input::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.state = s_ModifierKeyStates[keyCode] ? se::input::KeyState::Down : se::input::KeyState::Up;

            inputComp->keyEvents.push_back(keyEvent);
            inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
        }

        return theEvent;
    }];
}

- (void)keyDown:(NSEvent*) event
{
    uint32_t scanCode = [event keyCode];
    se::input::Key key = se::mac::KeyMap::MacKeyToSparkKey(scanCode);
    auto inputComp = se::Application::Get()->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

    se::input::KeyEvent keyEvent;
    keyEvent.key = key;
    keyEvent.state = se::input::KeyState::Down;

    inputComp->keyEvents.push_back(keyEvent);
    inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
}

- (void)keyUp:(NSEvent*) event
{
    uint32_t scanCode = [event keyCode];
    se::input::Key key = se::mac::KeyMap::MacKeyToSparkKey(scanCode);
    auto inputComp = se::Application::Get()->GetWorld()->GetSingletonComponent<se::input::InputComponent>();

    se::input::KeyEvent keyEvent;
    keyEvent.key = key;
    keyEvent.state = se::input::KeyState::Up;

    inputComp->keyEvents.push_back(keyEvent);
    inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
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

- (void)mouseMoved:(NSEvent*)event
{
    auto app = se::Application::Get();
    auto window = app->GetPrimaryWindow();
    auto inputComp = app->GetWorld()->GetSingletonComponent<se::input::InputComponent>();
    NSPoint locationInView = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];
    inputComp->mouseX = locationInView.x * window->GetContentScale();
    inputComp->mouseY = ([[self contentView] frame].size.height - locationInView.y) * window->GetContentScale();
}

- (void)mouseDragged:(NSEvent*)event
{
    auto app = se::Application::Get();
    auto window = app->GetPrimaryWindow();
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