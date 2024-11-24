#include <pch.h>
#include "Entity.h"
#include "CameraEntity.h"

CameraEntity::CameraEntity()
{
    m_position = { FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ -10 } };
}