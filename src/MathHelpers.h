#pragma once

namespace Eigen
{
template<> struct NumTraits<fpm::fixed_16_16> : NumTraits<double>
{
    typedef fpm::fixed_16_16 Real;
    typedef fpm::fixed_16_16 NonInteger;
    typedef fpm::fixed_16_16 Literal;
    typedef fpm::fixed_16_16 Nested;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};
}

// Eigen formatting for fmt
template <typename T>
struct fmt::formatter<
  T,
  std::enable_if_t<
    std::is_base_of_v<Eigen::DenseBase<T>, T>,
    char>> : ostream_formatter {};

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
    float const& aspectRatio, float const& zNear, float const& zFar);

Eigen::Matrix4f LookAt(Eigen::Vector3f const& eye, Eigen::Vector3f const& target,
    Eigen::Vector3f const& up);

Eigen::Matrix4f Translation(Eigen::Vector3f position);

Eigen::Matrix4f Rotation(Eigen::Vector3f rotation);

float CrossProduct2D(Eigen::Vector2f const& a, Eigen::Vector2f const& b);

constexpr float Lerp(float const& a, float const& b, float const& t)
{
    return a + t * (b - a);
}
}