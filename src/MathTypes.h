#pragma once
#include "Types.h"

struct Vector2
{
    FixedUnit x;
    FixedUnit y;

    FixedUnit Cross(Vector2 const& rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }

    Vector2 operator+(Vector2 const& rhs) const
    {
        return Vector2{ x + rhs.x, y + rhs.y };
    }

    Vector2 operator-(Vector2 const& rhs) const
    {
        return Vector2{ x - rhs.x, y - rhs.y };
    }
};

struct Vector3
{
    FixedUnit x;
    FixedUnit y;
    FixedUnit z;

    FixedUnit Length() const
    {
        return fpm::sqrt(fpm::pow(x, 2) + fpm::pow(y, 2) + fpm::pow(z, 2));
    }

    Vector3 Normalize() const
    {
        auto length{ Length() };
        return Vector3{
            .x = x / length,
            .y = y / length,
            .z = z / length,
        };
    }

    Vector3 Cross(Vector3 const& rhs) const
    {
        return Vector3{
            .x = y * rhs.z - z * rhs.y,
            .y = z * rhs.x - x * rhs.z,
            .z = x * rhs.y - y * rhs.x,
        };
    }

    FixedUnit Dot(Vector3 const& rhs) const
    {
        return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
    }

    Vector3 operator+(Vector3 const& rhs) const
    {
        return Vector3{ x + rhs.x, y + rhs.y, z + rhs.z };
    }

    Vector3 operator-(Vector3 const& rhs) const
    {
        return Vector3{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
};

struct Vector4
{
    FixedUnit x;
    FixedUnit y;
    FixedUnit z;
    FixedUnit w;

    explicit operator Vector2() const
    {
        return Vector2{
            .x = x,
            .y = y,
        };
    }

    explicit operator Vector3() const
    {
        return Vector3{
            .x = x,
            .y = y,
            .z = z,
        };
    }
};

struct Matrix4x4
{
    Matrix4x4(std::array<std::array<FixedUnit, 4>, 4> const& arg) : Contents{ arg }
    { }

    Matrix4x4() : Matrix4x4{{
            std::array<FixedUnit, 4>{ FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        }}
    { }

    std::array<std::array<FixedUnit, 4>, 4> Contents;

    FixedUnit& At(size_t x, size_t y)
    {
        return Contents.at(y).at(x);
    }

    FixedUnit const& At(size_t x, size_t y) const
    {
        return Contents.at(y).at(x);
    }

    Matrix4x4 operator*(Matrix4x4 const& rhs) const
    {
        return Matrix4x4{{
            // Row 1
            std::array<FixedUnit, 4>{
                At(0, 0) * rhs.At(0, 0) +
                    At(1, 0) * rhs.At(0, 1) +
                    At(2, 0) * rhs.At(0, 2) +
                    At(3, 0) * rhs.At(0, 3),
                At(0, 0) * rhs.At(1, 0) + 
                    At(1, 0) * rhs.At(1, 1) +
                    At(2, 0) * rhs.At(1, 2) +
                    At(3, 0) * rhs.At(1, 3),
                At(0, 0) * rhs.At(2, 0) + 
                    At(1, 0) * rhs.At(2, 1) +
                    At(2, 0) * rhs.At(2, 2) +
                    At(3, 0) * rhs.At(2, 3),
                At(0, 0) * rhs.At(3, 0) + 
                    At(1, 0) * rhs.At(3, 1) +
                    At(2, 0) * rhs.At(3, 2) +
                    At(3, 0) * rhs.At(3, 3)
            },
            // Row 2
            std::array<FixedUnit, 4>{
                At(0, 1) * rhs.At(0, 0) +
                    At(1, 1) * rhs.At(0, 1) +
                    At(2, 1) * rhs.At(0, 2) +
                    At(3, 1) * rhs.At(0, 3),
                At(0, 1) * rhs.At(1, 0) + 
                    At(1, 1) * rhs.At(1, 1) +
                    At(2, 1) * rhs.At(1, 2) +
                    At(3, 1) * rhs.At(1, 3),
                At(0, 1) * rhs.At(2, 0) + 
                    At(1, 1) * rhs.At(2, 1) +
                    At(2, 1) * rhs.At(2, 2) +
                    At(3, 1) * rhs.At(2, 3),
                At(0, 1) * rhs.At(3, 0) + 
                    At(1, 1) * rhs.At(3, 1) +
                    At(2, 1) * rhs.At(3, 2) +
                    At(3, 1) * rhs.At(3, 3)
            },
            // Row 3
            std::array<FixedUnit, 4>{
                At(0, 2) * rhs.At(0, 0) +
                    At(1, 2) * rhs.At(0, 1) +
                    At(2, 2) * rhs.At(0, 2) +
                    At(3, 2) * rhs.At(0, 3),
                At(0, 2) * rhs.At(1, 0) + 
                    At(1, 2) * rhs.At(1, 1) +
                    At(2, 2) * rhs.At(1, 2) +
                    At(3, 2) * rhs.At(1, 3),
                At(0, 2) * rhs.At(2, 0) + 
                    At(1, 2) * rhs.At(2, 1) +
                    At(2, 2) * rhs.At(2, 2) +
                    At(3, 2) * rhs.At(2, 3),
                At(0, 2) * rhs.At(3, 0) + 
                    At(1, 2) * rhs.At(3, 1) +
                    At(2, 2) * rhs.At(3, 2) +
                    At(3, 2) * rhs.At(3, 3)
            },
            // Row 4
            std::array<FixedUnit, 4>{
                At(0, 3) * rhs.At(0, 0) +
                    At(1, 3) * rhs.At(0, 1) +
                    At(2, 3) * rhs.At(0, 2) +
                    At(3, 3) * rhs.At(0, 3),
                At(0, 3) * rhs.At(1, 0) + 
                    At(1, 3) * rhs.At(1, 1) +
                    At(2, 3) * rhs.At(1, 2) +
                    At(3, 3) * rhs.At(1, 3),
                At(0, 3) * rhs.At(2, 0) + 
                    At(1, 3) * rhs.At(2, 1) +
                    At(2, 3) * rhs.At(2, 2) +
                    At(3, 3) * rhs.At(2, 3),
                At(0, 3) * rhs.At(3, 0) + 
                    At(1, 3) * rhs.At(3, 1) +
                    At(2, 3) * rhs.At(3, 2) +
                    At(3, 3) * rhs.At(3, 3)
            },
        }};
    }

    Vector4 operator*(Vector4 const& rhs) const
    {
        return Vector4{
            .x = (At(0, 0) * rhs.x) + (At(1, 0) * rhs.y) + (At(2, 0) * rhs.z) + (At(3, 0) * rhs.w),
            .y = (At(0, 1) * rhs.x) + (At(1, 1) * rhs.y) + (At(2, 1) * rhs.z) + (At(3, 1) * rhs.w),
            .z = (At(0, 2) * rhs.x) + (At(1, 2) * rhs.y) + (At(2, 2) * rhs.z) + (At(3, 2) * rhs.w),
            .w = (At(0, 3) * rhs.x) + (At(1, 3) * rhs.y) + (At(2, 3) * rhs.z) + (At(3, 3) * rhs.w),
        };
    }

    static Matrix4x4 PerspectiveProjection(FixedUnit fieldOfViewRadians, FixedUnit aspectRatio,
        FixedUnit zNear, FixedUnit zFar)
    {
        return Matrix4x4{{
            std::array<FixedUnit, 4>{ (aspectRatio * (1 / fpm::tan(fieldOfViewRadians / 2))),
                FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, (1 / fpm::tan(fieldOfViewRadians / 2)),
                FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, (zFar / (zFar - zNear)),
                ((-zFar * zNear) / (zFar - zNear)) },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 },
                FixedUnit{ 0 } }
        }};
    }

    static Matrix4x4 LookAt(Vector3 eye, Vector3 target, Vector3 up)
    {
        Vector3 z{ (target - eye).Normalize() };
        Vector3 x{ up.Cross(z).Normalize() };
        Vector3 y{ z.Cross(x) };

        return Matrix4x4{{
            std::array<FixedUnit, 4>{ x.x, x.y, x.z, -x.Dot(eye) },
            std::array<FixedUnit, 4>{ y.x, y.y, y.z, -y.Dot(eye) },
            std::array<FixedUnit, 4>{ z.x, z.y, z.z, -z.Dot(eye) },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 },
                FixedUnit{ 1 } },
        }};
    }

    static Matrix4x4 Translation(Vector3 translate)
    {
        return Matrix4x4{{
            std::array<FixedUnit, 4>{ FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 }, translate.x },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 }, translate.y },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 }, translate.z },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        }};
    }

    static Matrix4x4 EulerRotationX(FixedUnit angle)
    {
        return Matrix4x4{{
            std::array<FixedUnit, 4>{ FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, fpm::cos(angle), fpm::sin(angle), FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, -fpm::sin(angle), fpm::cos(angle), FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        }};
    }

    static Matrix4x4 EulerRotationY(FixedUnit angle)
    {
        return Matrix4x4{{
            std::array<FixedUnit, 4>{ fpm::cos(angle), FixedUnit{ 0 }, -fpm::sin(angle), FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ fpm::sin(angle), FixedUnit{ 0 }, fpm::cos(angle), FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        }};
    }

    static Matrix4x4 EulerRotationZ(FixedUnit angle)
    {
        return Matrix4x4{{
            std::array<FixedUnit, 4>{ fpm::cos(angle), fpm::sin(angle), FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ -fpm::sin(angle), fpm::cos(angle), FixedUnit{ 0 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 } },
            std::array<FixedUnit, 4>{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        }};
    }

    static Matrix4x4 EulerRotation(Vector3 angles)
    {
        return EulerRotationX(angles.x) * EulerRotationY(angles.y) * EulerRotationZ(angles.z);
    }
};

struct MyPolygon
{
    std::vector<Vector3> Vertices;
    std::vector<Vector2> TextureCoordinates;
};

struct Plane
{
    Vector3 Point;
    Vector3 Normal;
};

struct Triangle
{
    std::array<Vector3, 3> Vertices;
    std::array<Vector2, 3> TextureCoordinates;
};

constexpr FixedUnit Lerp(const FixedUnit& a, const FixedUnit& b, const FixedUnit& t)
{
    return a + t * (b - a);
}