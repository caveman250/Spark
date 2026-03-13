#include "platform/MouseCursorUtil.h"
#include "engine/Application.h"
#include "Window.h"

namespace se
{
    static SDL_Cursor* ArrowCursor = nullptr;
    static SDL_Cursor* IBeamCursor = nullptr;
    static SDL_Cursor* ResizeLRCursor = nullptr;
    static SDL_Cursor* ResizeUDCursor = nullptr;
    static bool HasInitializedCursors = false;

    void MouseCursorUtil::InitCursors()
    {
        ArrowCursor = SDL_GetDefaultCursor();
        IBeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        ResizeLRCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        ResizeUDCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        HasInitializedCursors = true;
    }

    void MouseCursorUtil::FreeCursors()
    {
        SDL_FreeCursor(IBeamCursor);
        SDL_FreeCursor(ResizeLRCursor);
        SDL_FreeCursor(ResizeUDCursor);
        HasInitializedCursors = false;
    }

    void MouseCursorUtil::SetMouseCursor(MouseCursor cursor)
    {
        if (!HasInitializedCursors)
        {
            InitCursors();
        }

        SDL_Cursor* currentCursor = SDL_GetCursor();
        SDL_Cursor* desiredCursor = ArrowCursor;
        switch(cursor)
        {
            case MouseCursor::Arrow:
                desiredCursor = ArrowCursor;
                break;
            case MouseCursor::IBeam:
                desiredCursor = IBeamCursor;
                break;
            case MouseCursor::ResizeLeftRight:
                desiredCursor = ResizeLRCursor;
                break;
            case MouseCursor::ResizeUpDown:
                desiredCursor = ResizeUDCursor;
                break;
        }

        if (!currentCursor || desiredCursor != currentCursor)
        {
            SDL_SetCursor(desiredCursor);
        }
    }
}