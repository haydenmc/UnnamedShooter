#pragma once

struct InputState
{
    bool Escape;
    bool MoveForward;
    bool MoveBackward;
    bool MoveLeft;
    bool MoveRight;
};

struct Input
{
    Input();
    ~Input();
    InputState const& GetInputState();
private:
    const uint8_t* m_sdlKeyboardState;
    InputState m_inputState;
};