#include <testpch.h>
#include <MathHelpers.h>
#include <Mesh/Mesh.h>

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

TEST_CASE("Rotation transformation", "[math][matrix][transformation]")
{
    Eigen::Vector4f testVertex{ 1.0f, 1.0f, 1.0f, 1.0f };
    float angleRads{ static_cast<float>(M_PI_2) }; // 90 degrees

    // Rotate 90deg around X axis
    auto rotationMatrix{ game::Rotation(Eigen::Vector3f{ angleRads, 0.0f, 0.0f }) };
    Eigen::Vector4f transformedVertex{ rotationMatrix * testVertex };
    REQUIRE(transformedVertex.x() ==  1.0f);
    REQUIRE(game::AreEqual(transformedVertex.y(), -1.0f));
    REQUIRE(transformedVertex.z() ==  1.0f);

    // Rotate 90deg around Y axis
    rotationMatrix = game::Rotation(Eigen::Vector3f{ 0.0f, angleRads, 0.0f });
    transformedVertex = (rotationMatrix * testVertex);
    REQUIRE(transformedVertex.x() ==  1.0f);
    REQUIRE(transformedVertex.y() ==  1.0f);
    REQUIRE(game::AreEqual(transformedVertex.z(), -1.0f));

    // Rotate 90deg around Z axis
    rotationMatrix = game::Rotation(Eigen::Vector3f{ 0.0f, 0.0f, angleRads });
    transformedVertex = (rotationMatrix * testVertex);
    REQUIRE(game::AreEqual(transformedVertex.x(), -1.0f));
    REQUIRE(transformedVertex.y() ==  1.0f);
    REQUIRE(transformedVertex.z() ==  1.0f);
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