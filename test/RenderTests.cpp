#include <testpch.h>
#include <MathTypes.h>
#include <Renderer/RenderTarget.h>
namespace
{
    constexpr uint32_t c_clearColor{ 0xFF000000 };
    constexpr uint32_t c_primaryTestColor{ 0xFFFF0000 };
}

TEST_CASE("Drawing pixels", "[renderer]")
{
    RenderTarget testTarget{ 8, 4 };
    testTarget.ClearBuffer(c_clearColor);
    testTarget.DrawPixel(1, 1, c_primaryTestColor);
    testTarget.DrawPixel(3, 2, c_primaryTestColor);
    testTarget.DrawPixel(5, 3, c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(5, 3) == c_primaryTestColor);
}

TEST_CASE("Drawing lines", "[renderer]")
{
    RenderTarget testTarget{ 8, 4 };

    // Diagonal line, (1, 1) -> (5, 2)
    testTarget.ClearBuffer(c_clearColor);
    testTarget.DrawLine(Vector2{ FixedUnit{ 1 }, FixedUnit{ 1 } },
        Vector2{ FixedUnit{ 5 }, FixedUnit { 2 } }, c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(2, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(4, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(5, 2) == c_primaryTestColor);

    // Vertical line, (3, 0) -> (3, 3)
    testTarget.ClearBuffer(c_clearColor);
    testTarget.DrawLine(Vector2{ FixedUnit{ 3 }, FixedUnit{ 0 } },
        Vector2{ FixedUnit{ 3 }, FixedUnit { 3 } }, c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 0) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 3) == c_primaryTestColor);

    // Horizontal line, (0, 2) -> (7, 2)
    testTarget.ClearBuffer(0xFF000000);
    testTarget.DrawLine(Vector2{ FixedUnit{ 0 }, FixedUnit{ 2 } },
        Vector2{ FixedUnit{ 7 }, FixedUnit { 2 } }, c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(0, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(2, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(4, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(5, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(6, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(7, 2) == c_primaryTestColor);
}

TEST_CASE("Draw simple triangle", "[renderer]")
{
    RenderTarget testTarget{ 8, 8 };
    testTarget.ClearBuffer(c_clearColor);
    testTarget.DrawTriangle(Vector2{ FixedUnit{ 4 }, FixedUnit { 1 } },
        Vector2{ FixedUnit{ 1 }, FixedUnit { 1 } }, Vector2{ FixedUnit{ 1 }, FixedUnit { 4 } },
        c_primaryTestColor);

    REQUIRE(testTarget.PixelAt(1, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(2, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(4, 1) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(2, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(3, 2) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 3) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(2, 3) == c_primaryTestColor);
    REQUIRE(testTarget.PixelAt(1, 4) == c_primaryTestColor);
}