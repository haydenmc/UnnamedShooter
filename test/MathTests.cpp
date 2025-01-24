#include <testpch.h>
#include <MathTypes.h>

MyPolygon ClipPolygonAgainstPlane(const MyPolygon& polygon, const Plane& plane)
{
    const Vector3& planePoint{ plane.Point };
    const Vector3& planeNormal{ plane.Normal };

    printf("Plane point: %f, %f, %f\n", static_cast<float>(planePoint.x), static_cast<float>(planePoint.y), static_cast<float>(planePoint.z));
    printf("Plane normal: %f, %f, %f\n", static_cast<float>(planeNormal.x), static_cast<float>(planeNormal.y), static_cast<float>(planeNormal.z));

    MyPolygon result;
    size_t currentVertexIndex = 0;
    size_t currentTextureCoordIndex = 0;
    size_t previousVertexIndex = polygon.Vertices.size() - 1;
    size_t previousTextureCoordIndex = polygon.TextureCoordinates.size() - 1;

    auto prevVertexToPlane{ (polygon.Vertices.at(previousVertexIndex) - planePoint) };
    FixedUnit previousDot{ prevVertexToPlane.Dot(planeNormal) };
    printf("prevVertexToPlane: %f, %f, %f\n", static_cast<float>(prevVertexToPlane.x), static_cast<float>(prevVertexToPlane.y), static_cast<float>(prevVertexToPlane.z));
    printf("previousDot: %f\n", static_cast<float>(previousDot));

    while (currentVertexIndex != polygon.Vertices.size())
    {
        const auto& currentVertex{ polygon.Vertices.at(currentVertexIndex) };
        const auto& currentTextureCoord{
            polygon.TextureCoordinates.at(currentTextureCoordIndex) };
        const auto& previousVertex{ polygon.Vertices.at(previousVertexIndex) };
        const auto& previousTextureCoord{
            polygon.TextureCoordinates.at(previousTextureCoordIndex) };

        printf("currentVertex: %f, %f, %f\n", static_cast<float>(currentVertex.x), static_cast<float>(currentVertex.y), static_cast<float>(currentVertex.z));
        FixedUnit currentDot{ (currentVertex - planePoint).Dot(planeNormal) };

        printf("currentDot: %f\n", static_cast<float>(currentDot));

        // Signs have changed between last dot and current dot, indicating
        // the line between the previous and current vertices has crossed
        // the plane boundary
        if (currentDot * previousDot < FixedUnit{ 0 })
        {
            // Split the polygon at the intersection point of the line and
            // the plane
            FixedUnit t{ previousDot / (previousDot - currentDot) };
            Vector3 intersectionPoint{
                .x = Lerp(previousVertex.x, currentVertex.x, t),
                .y = Lerp(previousVertex.y, currentVertex.y, t),
                .z = Lerp(previousVertex.z, currentVertex.z, t),
            };
            Vector2 interpolatedTextureCoord{
                .x = Lerp(previousTextureCoord.x, currentTextureCoord.x, t),
                .y = Lerp(previousTextureCoord.y, currentTextureCoord.y, t),
            };
            result.Vertices.push_back(intersectionPoint);
            result.TextureCoordinates.push_back(interpolatedTextureCoord);
        }

        if (currentDot > FixedUnit{ 0 })
        {
            // Current vertex is inside the plane
            result.Vertices.push_back(currentVertex);
            result.TextureCoordinates.push_back(currentTextureCoord);
        }

        previousDot = currentDot;
        previousVertexIndex = currentVertexIndex;
        ++currentVertexIndex;
        previousTextureCoordIndex = currentTextureCoordIndex;
        ++currentTextureCoordIndex;
    }

    return result;
}

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

TEST_CASE("Polygon clipping", "[math]")
{
    MyPolygon inputPolygon{
        .Vertices = {
            Vector3{ FixedUnit{  2 }, FixedUnit{ 0 }, FixedUnit{ 1 } }, // A
            Vector3{ FixedUnit{ -2 }, FixedUnit{ 0 }, FixedUnit{ 1 } }, // B
            Vector3{ FixedUnit{ -2 }, FixedUnit{ 2 }, FixedUnit{ 1 } }, // C
            Vector3{ FixedUnit{  2 }, FixedUnit{ 2 }, FixedUnit{ 1 } }, // D
        },
        .TextureCoordinates = {
            Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // A
            Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // B
            Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // C
            Vector2{ FixedUnit{ 0 }, FixedUnit{ 0 } }, // D
        }
    };

    Plane clippingPlane{
        .Point = Vector3{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
        .Normal = Vector3{ FixedUnit{ 1 }, FixedUnit{ 0 }, FixedUnit{ 0 } },
    };

    MyPolygon clippedPolygon{ ClipPolygonAgainstPlane(inputPolygon, clippingPlane) };
    for (const auto& vertex : clippedPolygon.Vertices)
    {
        printf("Clipped vertex: %f, %f, %f\n", static_cast<float>(vertex.x),static_cast<float>(vertex.y),static_cast<float>(vertex.z));
    }
    REQUIRE(clippedPolygon.Vertices.at(0).x == FixedUnit{ 2 });
}