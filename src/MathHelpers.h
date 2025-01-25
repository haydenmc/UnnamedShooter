#pragma once

namespace game
{
struct Polygon
{
    std::vector<Eigen::Vector3f> Vertices;
    std::vector<Eigen::Vector2f> TextureCoordinates;
};

struct Plane
{
    Eigen::Vector3f Point;
    Eigen::Vector3f Normal;
};

struct Triangle
{
    std::array<Eigen::Vector3f, 3> Vertices;
    std::array<Eigen::Vector2f, 3> TextureCoordinates;
};
    
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

    return Eigen::Matrix4f{
        { x.x(), x.y(), x.z(), -x.dot(eye) },
        { y.x(), y.y(), y.z(), -y.dot(eye) },
        { z.x(), z.y(), z.z(), -z.dot(eye) },
        {  0.0f,  0.0f,  0.0f,        0.0f },
    };
}
};