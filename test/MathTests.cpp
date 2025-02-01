#include <testpch.h>
#include <MathHelpers.h>

TEST_CASE("Translation transformation", "[math][matrix][transformation]")
{
    Eigen::Vector4f currentPosition{ 1, 1, 1, 1 };
    Eigen::Vector3f translateBy{ 1, 2 ,3 };
    auto translationMatrix{ game::Translation(translateBy) };
    auto result{ translationMatrix * currentPosition };
    REQUIRE(result.x() == 2.0f);
    REQUIRE(result.y() == 3.0f);
    REQUIRE(result.z() == 4.0f);
}

// TEST_CASE("Polygon clipping", "[math]")
// {
//     MyPolygon inputPolygon{
//         .Vertices = {
//             Vector3{ FixedUnit{  2 }, FixedUnit{ 0 }, FixedUnit{ 1 } }, // A
//             Vector3{ FixedUnit{ -2 }, FixedUnit{ 0 }, FixedUnit{ 1 } }, // B
//             Vector3{ FixedUnit{ -2 }, FixedUnit{ 2 }, FixedUnit{ 1 } }, // C
//             Vector3{ FixedUnit{  2 }, FixedUnit{ 2 }, FixedUnit{ 1 } }, // D
//         },
//         .TextureCoordinates = {
//             Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // A
//             Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // B
//             Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // C
//             Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // D
//         }
//     };

//     Plane clippingPlane{
//         .Point = Vector3{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
//         .Normal = Vector3{ FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
//     };

//     MyPolygon clippedPolygon{ ClipPolygonAgainstPlane(inputPolygon, clippingPlane) };
//     for (const auto& vertex : clippedPolygon.Vertices)
//     {
//         printf("Clipped vertex: %f, %f, %f\n", static_cast<float>(vertex.x),static_cast<float>(vertex.y),static_cast<float>(vertex.z));
//     }
//     REQUIRE(clippedPolygon.Vertices.at(0).x == FixedUnit{ 2 });
// }