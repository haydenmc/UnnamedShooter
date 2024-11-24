#include <testpch.h>
#include <MathTypes.h>

TEST_CASE("Vector addition and subtraction", "[math][vector]")
{
    Vector3 first{ FixedUnit{ 1 }, FixedUnit{ 2 }, FixedUnit{ 3 } };
    Vector3 second{ FixedUnit{ 3 }, FixedUnit{ 2 }, FixedUnit{ 1 } };
    Vector3 add{ first + second };
    REQUIRE( add.x == FixedUnit{ 4 } );
    REQUIRE( add.y == FixedUnit{ 4 } );
    REQUIRE( add.z == FixedUnit{ 4 } );

    Vector3 sub{ first - second };
    REQUIRE( sub.x == FixedUnit{ -2 } );
    REQUIRE( sub.y == FixedUnit{ 0 } );
    REQUIRE( sub.z == FixedUnit{ 2 } );
}

TEST_CASE("Translation transformation", "[math][matrix][transformation]")
{
    Vector4 currentPosition{ FixedUnit{ 1 }, FixedUnit { 1 }, FixedUnit { 1 }, FixedUnit { 1 } };
    Vector3 translateBy{ FixedUnit{ 1 }, FixedUnit { 2 }, FixedUnit{ 3 } };
    auto translationMatrix{ Matrix4x4::Translation(translateBy) };
    auto result{ translationMatrix * currentPosition };
    REQUIRE(result.x == FixedUnit{ 2 });
    REQUIRE(result.y == FixedUnit{ 3 });
    REQUIRE(result.z == FixedUnit{ 4 });
}