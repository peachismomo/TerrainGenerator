#include "Terrain.h"
#include <Utils.h>
#include "PoissonDiskSampling.h"
#include "Voronoi.h"
#include "Perlin.h"
#include "triangulation.h"

#include "delaunay.h"
#include "vector2.h"

struct Triangle
{
    PoissonGenerator::Point p1, p2, p3;
    PoissonGenerator::Point circumcenter;
    float circumradius;

    void calculateCircumcircle()
    {
        float ax = p1.x;
        float ay = p1.y;
        float bx = p2.x;
        float by = p2.y;
        float cx = p3.x;
        float cy = p3.y;

        float D = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));

        float ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / D;
        float uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / D;

        circumcenter = { ux, uy };
        circumradius = sqrt((ux - ax) * (ux - ax) + (uy - ay) * (uy - ay));
    }

    bool containsPoint(const PoissonGenerator::Point& p) const
    {
        float dx = circumcenter.x - p.x;
        float dy = circumcenter.y - p.y;
        float dist = sqrt(dx * dx + dy * dy);
        return dist < circumradius;
    }
};

static bool operator==(const Triangle& t1, const Triangle& t2)
{
    return (t1.p1.x == t2.p1.x && t1.p1.y == t2.p1.y &&
            t1.p2.x == t2.p2.x && t1.p2.y == t2.p2.y &&
            t1.p3.x == t2.p3.x && t1.p3.y == t2.p3.y);
}

static bool operator==(const PoissonGenerator::Point& p1, const PoissonGenerator::Point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

static std::vector<Triangle> triangulate(std::vector<PoissonGenerator::Point>& points)
{
    std::vector<Triangle> triangles;

    // Create super-triangle
    float minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const PoissonGenerator::Point& p : points)
    {
        if (p.x < minX) minX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;
    }
    float dx = maxX - minX;
    float dy = maxY - minY;
    float deltaMax = std::max(dx, dy);
    PoissonGenerator::Point p1 = { minX - 10 * deltaMax, minY - 10 * deltaMax };
    PoissonGenerator::Point p2 = { minX - 10 * deltaMax, maxY + 10 * deltaMax };
    PoissonGenerator::Point p3 = { maxX + 10 * deltaMax, minY - 10 * deltaMax };

    Triangle superTriangle = { p1, p2, p3 };
    superTriangle.calculateCircumcircle();
    triangles.push_back(superTriangle);

    // Add each point to the triangulation
    for (const PoissonGenerator::Point& point : points)
    {
        std::vector<Triangle> badTriangles;
        for (const Triangle& triangle : triangles)
        {
            if (triangle.containsPoint(point))
            {
                badTriangles.push_back(triangle);
            }
        }

        std::vector<std::pair<PoissonGenerator::Point, PoissonGenerator::Point>> polygon;
        for (const Triangle& triangle : badTriangles)
        {
            polygon.push_back({ triangle.p1, triangle.p2 });
            polygon.push_back({ triangle.p2, triangle.p3 });
            polygon.push_back({ triangle.p3, triangle.p1 });
        }

        for (const auto& triangle : badTriangles)
        {
            auto it = std::find(triangles.begin(), triangles.end(), triangle);
            if (it != triangles.end())
            {
                triangles.erase(it);
            }
        }

        for (const auto& edge : polygon)
        {
            Triangle newTriangle = { edge.first, edge.second, point };
            newTriangle.calculateCircumcircle();
            triangles.push_back(newTriangle);
        }
    }

    // Remove triangles that share vertices with the super-triangle
    triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](const Triangle& t)
                                   {
                                       return (t.p1 == p1 || t.p1 == p2 || t.p1 == p3 ||
                                               t.p2 == p1 || t.p2 == p2 || t.p2 == p3 ||
                                               t.p3 == p1 || t.p3 == p2 || t.p3 == p3);
                                   }), triangles.end());

    return triangles;
}


void Terrain::GeneratePoints()
{
    // Step 1: Generate Poisson points
    PoissonGenerator::DefaultPRNG prng(1234);
    auto points = PoissonGenerator::generatePoissonPoints(10000, prng, false);

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    for (const auto& p : points)
    {
        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }

    for (auto& p : points)
    {
        float normalized_x = 2.0f * (p.x - min_x) / (max_x - min_x) - 1.0f;
        float normalized_y = 2.0f * (p.y - min_y) / (max_y - min_y) - 1.0f;
        p.x = normalized_x * 10.f;
        p.y = normalized_y * 10.f;
    }

    std::vector<dt::Vector2<double>> coords;
    for (auto& p : points)
    {
        coords.emplace_back(dt::Vector2<double>{p.x, p.y});
    }
    dt::Delaunay<double> triangulation;

    const std::vector<dt::Triangle<double>> triangles = triangulation.triangulate(coords);
    for (auto& tri : triangles)
    {
        m_terrain_vtx.emplace_back(tri.a->x, 0.f, tri.a->y);
        m_terrain_vtx.emplace_back(tri.b->x, 0.f, tri.b->y);
        m_terrain_vtx.emplace_back(tri.c->x, 0.f, tri.c->y);
    }

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{ seed };

    for (auto& p : m_terrain_vtx)
    {
        p.y = (float)perlin.octave2D_11Smooth((double)p.x, (double)p.z, 4, 0.5, 30.0) * 5.f;
    }
}
