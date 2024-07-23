#include "Utils.h"
#include <CustomMath.h>

glm::vec2 Perlin::RandomGradient(int ix, int iy) const
{
	const unsigned int w = 8 * sizeof(unsigned);
	const unsigned int s = w / 2;

	unsigned int a = ix, b = iy;

    a *= prime1;

    b ^= (a << s) | (a >> (w - s));
    b *= prime2;

    a ^= (b << s) | (b >> (w - s));
    a *= prime3;

    float random = a * (3.14159265f / ~(~0u >> 1));

    return glm::vec2(sinf(random), cosf(random));
}

float Perlin::DotGridGradient(int ix, int iy, float x, float y)
{
    glm::vec2 gradient = RandomGradient(ix, iy);

    float dx = x - (float)ix;
    float dy = y - (float)iy;

    return (dx * gradient.x + dy * gradient.y);
}

float Perlin::Interpolate(float p1, float p2, float w)
{
    return (p2 - p1) * (3.f - w * 2.f) * w * w + p1;
}

float Perlin::GenPerlin(float x, float y)
{
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    float n0 = DotGridGradient(x0, y0, x, y);
    float n1 = DotGridGradient(x1, y0, x, y);
    float ix0 = Interpolate(n0, n1, sx);

    n0 = DotGridGradient(x0, y1, x, y);
    n1 = DotGridGradient(x1, y1, x, y);
    float ix1 = Interpolate(n0, n1, sx);
    float value = Interpolate(ix0, ix1, sy);

    return value;
}

PoissonDiskSampling::PoissonDiskSampling(int p_width, int p_height, float p_min_dist, int p_point_count)
{
    m_width = p_width;
    m_height = p_height;
    m_min_dist = p_min_dist;
    m_point_count = p_point_count;
    m_cell_size = m_min_dist / 1.414214f;
    m_grid_width = static_cast<int>(ceil(m_width / m_cell_size));
    m_grid_height = static_cast<int>(ceil(m_height / m_cell_size));
    m_grid = std::vector<std::vector<glm::vec3*>>(m_grid_width, std::vector<glm::vec3*>(m_grid_height, NULL));
}

std::vector<std::pair<float, float>> PoissonDiskSampling::Generate()
{
    glm::vec3 first_point(rand() % m_width, 0.f, rand() % m_height);

    m_process.push_back(first_point);
    m_sample.push_back(std::make_pair(first_point.x, first_point.z));
    int first_point_x = static_cast<int>(first_point.x / m_cell_size);
    int first_point_y = static_cast<int>(first_point.z / m_cell_size);
    m_grid[first_point_x][first_point_y] = new glm::vec3(first_point);

    while (!m_process.empty())
    {
        int new_point_index = rand() % m_process.size();
        glm::vec3 new_point = m_process[new_point_index];
        m_process.erase(m_process.begin() + new_point_index);

        for (int i = 0; i < m_point_count; i++)
        {
            glm::vec3 new_point_around = GeneratePointAround(new_point);

            if (InRectangle(new_point_around) && !InNeighbourhood(new_point_around))
            {
                m_process.push_back(new_point_around);
                m_sample.push_back(std::make_pair(new_point_around.x, new_point_around.z));
                int new_point_x = static_cast<int>(new_point_around.x / m_cell_size);
                int new_point_y = static_cast<int>(new_point_around.z / m_cell_size);
                m_grid[new_point_x][new_point_y] = new glm::vec3(new_point_around);
            }
        }
    }

    return m_sample;
}

glm::vec3 PoissonDiskSampling::GeneratePointAround(glm::vec3 p_point) const
{
    float r1 = (float)rand() / RAND_MAX;
    float r2 = (float)rand() / RAND_MAX;

    float radius = m_min_dist * (r1 + 1.f);

    float angle = 2.f * 3.14159265f * r2;

    float new_x = p_point.x + radius * cosf(angle);
    float new_y = p_point.y + radius * sinf(angle);

    return glm::vec3(new_x, 0.f, new_y);
}

bool PoissonDiskSampling::InRectangle(glm::vec3 p_point) const
{
    return (p_point.x >= 0 && p_point.z >= 0 && p_point.x < m_width && p_point.z < m_height);
}

bool PoissonDiskSampling::InNeighbourhood(glm::vec3 p_point)
{
    std::vector<glm::vec3*> cells = GetCellsAround(p_point);
    int size = static_cast<int>(cells.size());
    for (int i = 0; i < size; i++)
    {
        if (glm::length(p_point) < m_min_dist)
        {
            return true;
        }
    }
    return false;
}

std::vector<glm::vec3*> PoissonDiskSampling::GetCellsAround(glm::vec3 p_point)
{
    std::vector<glm::vec3*> cells;
    int x_index = static_cast<int>(p_point.x / m_cell_size);
    int y_index = static_cast<int>(p_point.y / m_cell_size);

    int min_x = CMIN(0, x_index - 1);
    int max_x = CMAX(m_grid_width - 1, x_index + 1);

    int min_y = CMAX(0, y_index - 1);
    int max_y = CMIN(m_grid_height - 1, y_index + 1);

    if (min_x < 0 || min_y < 0)
        return cells;

    for (int i = min_x; i < max_x; i++)
    {
        for (int j = min_y; j < max_y; j++)
        {
            if (m_grid[i][j] != NULL)
            {
                cells.push_back(m_grid[i][j]);
            }
        }
    }
    return cells;
}
