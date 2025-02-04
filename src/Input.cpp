#include <pch.h>
#include "Input.h"

namespace
{
    const uint8_t* GetSDLKeyboardStatePointer()
    {
        SDL_InitSubSystem(SDL_INIT_EVENTS);
        SPDLOG_INFO("Getting global SDL Keyboard State pointer");
        return SDL_GetKeyboardState(nullptr);
    }
}

Input::Input() : m_sdlKeyboardState{ GetSDLKeyboardStatePointer() }
{
    CheckSdlReturn(SDL_SetRelativeMouseMode(SDL_TRUE));
}

Input::~Input()
{
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

InputState const& Input::GetInputState()
{
    SDL_PumpEvents();
    // TODO: Map bindings dynamically based on configuration.
    m_inputState.Escape = m_sdlKeyboardState[SDL_SCANCODE_ESCAPE];
    m_inputState.MoveForward = m_sdlKeyboardState[SDL_SCANCODE_W];
    m_inputState.MoveBackward = m_sdlKeyboardState[SDL_SCANCODE_S];
    m_inputState.MoveLeft = m_sdlKeyboardState[SDL_SCANCODE_A];
    m_inputState.MoveRight = m_sdlKeyboardState[SDL_SCANCODE_D];
    SDL_GetRelativeMouseState(&m_inputState.RelativeLookX, &m_inputState.RelativeLookY);
    return m_inputState;
}
