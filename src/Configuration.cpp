#include <pch.h>
#include "Configuration.h"

Configuration::Configuration() : m_videoConfiguration{ 640, 360, true }
{ }

VideoConfiguration Configuration::GetVideoConfiguration()
{
    return m_videoConfiguration;
}