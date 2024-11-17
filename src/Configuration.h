#pragma once

struct VideoResolution
{
    uint16_t Width;
    uint16_t Height;
};

struct Configuration
{
    Configuration();
    VideoResolution GetVideoResolution();
private:
    VideoResolution m_videoResolution;
};