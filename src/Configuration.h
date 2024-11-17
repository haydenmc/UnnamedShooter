#pragma once

struct VideoConfiguration
{
    uint16_t Width;
    uint16_t Height;
    bool IsFullscreen;
};

struct Configuration
{
    Configuration();
    VideoConfiguration GetVideoConfiguration();
private:
    VideoConfiguration m_videoConfiguration;
};