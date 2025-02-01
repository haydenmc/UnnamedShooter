#include "pch.h"
#include "MathHelpers.h"

namespace game
{
Eigen::Matrix4f PerspectiveProjectionTransformMatrix(float const& fieldOfViewRadians,
    float const& aspectRatio, float const& zNear, float const& zFar)
{
    Eigen::Matrix4f m;
    m(0, 0) = (aspectRatio * (1.0f / tanf(fieldOfViewRadians / 2.0f)));
    m(1, 1) = (1.0f / tanf(fieldOfViewRadians / 2.0f));
    m(2, 2) = (zFar / (zFar - zNear));
    m(2, 3) = ((-zFar * zNear) / (zFar - zNear));
    m(3, 2) = 1.0f;
    return m;
}
Eigen::Matrix4f LookAt(Eigen::Vector3f const& eye, Eigen::Vector3f const& target,
    Eigen::Vector3f const& up)
{
    Eigen::Vector3f z{ target - eye };
    z.normalize();
    Eigen::Vector3f x{ up.cross(z) };
    x.normalize();
    Eigen::Vector3f y{ z.cross(x) };

    Eigen::Matrix4f m;
    m << 
        x.x(), x.y(), x.z(), -x.dot(eye),
        y.x(), y.y(), y.z(), -y.dot(eye),
        z.x(), z.y(), z.z(), -z.dot(eye),
        0.0f,  0.0f,  0.0f,         1.0f;
    return m;
}

Eigen::Matrix4f Translation(Eigen::Vector3f position)
{
    Eigen::Matrix4f m{ Eigen::Matrix4f::Identity() };
    m(0, 3) = position.x();
    m(1, 3) = position.y();
    m(2, 3) = position.z();
    return m;
}

Eigen::Matrix4f Rotation(Eigen::Vector3f rotation)
{
    Eigen::Matrix3f m{ Eigen::AngleAxisf(rotation.x(), Eigen::Vector3f::UnitZ())
        * Eigen::AngleAxisf(rotation.y(), Eigen::Vector3f::UnitY())
        * Eigen::AngleAxisf(rotation.z(), Eigen::Vector3f::UnitZ()) };
    Eigen::Matrix4f m4{ Eigen::Matrix4f::Identity() };
    m4.topLeftCorner(3, 3) = m.topLeftCorner(3, 3);
    return m4;
}

float CrossProduct2D(Eigen::Vector2f const& a, Eigen::Vector2f const& b)
{
    return a.x() * b.y() - a.y() * b.x();
}
}