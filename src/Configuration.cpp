#include <pch.h>
#include "Configuration.h"

Configuration::Configuration() : m_videoResolution{ 640, 360 }
{ }

VideoResolution Configuration::GetVideoResolution()
{
    return m_videoResolution;
}