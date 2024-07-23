#include "BVH.h"
#include <algorithm>
#include <numeric>
#include <Engine.h>

BVHNode* BVHTopDown::Build(std::vector<Object*>& objects, BVTYPE type, int depth)
{
	if (objects.empty())
		return nullptr;

	if (depth == maxHeight || objects.size() == 1)
		return CreateLeafNode(objects, type);

	int axis = ChooseSplitAxis(objects, type);

	std::vector<Object*> l_objs, r_objs;
	SplitObjects(objects, axis, l_objs, r_objs, type);

	if (type == T_AABB)
	{
		AABBNode* node = new AABBNode();
		node->left = Build(l_objs, type, depth + 1);
		node->right = Build(r_objs, type, depth + 1);
		if (node->left && node->right)
			node->aabb = BVHHelpers::CombineAABB(reinterpret_cast<AABBNode*>(node->left)->aabb, reinterpret_cast<AABBNode*>(node->right)->aabb);
		node->height = depth;
		return node;
	}
	else
	{
		BSNode* node = new BSNode();
		node->left = Build(l_objs, type, depth + 1);
		node->right = Build(r_objs, type, depth + 1);
		if (node->left && node->right)
			node->bs = BVHHelpers::CombineBS(reinterpret_cast<BSNode*>(node->left)->bs, reinterpret_cast<BSNode*>(node->right)->bs);
		node->height = depth;
		return node;
	}

	return nullptr;
}

void BVHTopDown::ClearBVH(BVHNode* node)
{
	if (node == nullptr)
		return;

	ClearBVH(node->left);
	ClearBVH(node->right);

	delete node;
	node = nullptr;
}

void BVHBotUp::ClearBVH(BVHNode* node)
{
	if (node == nullptr)
		return;

	ClearBVH(node->left);
	ClearBVH(node->right);

	delete node;
	node = nullptr;
}

BVHNode* BVHTopDown::CreateLeafNode(std::vector<Object*>& objects, BVTYPE type) const
{
	if (type == T_AABB)
	{
		AABBNode* leaf = new AABBNode();
		leaf->aabb = objects.front()->GetAABB();
		leaf->left = leaf->right = nullptr;
		leaf->height = maxHeight;
		return leaf;
	}
	else
	{
		BSNode* leaf = new BSNode();
		leaf->bs = objects.front()->GetSphere();
		leaf->left = leaf->right = nullptr;
		leaf->height = maxHeight;
		return leaf;
	}
}

int BVHTopDown::ChooseSplitAxis(std::vector<Object*> objects, BVTYPE type)
{
	float volumeX = ComputeTotalBoundingVolume(objects, 0, type);
	float volumeY = ComputeTotalBoundingVolume(objects, 1, type);
	float volumeZ = ComputeTotalBoundingVolume(objects, 2, type);

	if (volumeX <= volumeY && volumeX <= volumeZ)
		return 0;
	else if (volumeY <= volumeX && volumeY <= volumeZ)
		return 1;
	else
		return 2;
}

void BVHTopDown::SplitObjects(std::vector<Object*> objects, int axis, std::vector<Object*>& l_objs, std::vector<Object*>& r_objs, BVTYPE type)
{
	if (objects.empty())
		return;

	std::vector<size_t> indices(objects.size());
	std::iota(indices.begin(), indices.end(), 0);

	if (type == T_AABB)
	{
		std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b)
				  {
					  return BVHHelpers::CompareAABB(objects[a], objects[b], axis);
				  });
	}
	else
	{
		std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b)
				  {
					  return BVHHelpers::CompareSphere(objects[a], objects[b], axis);
				  });
	}

	if (engine.GetEditor().m_k_even_split && engine.GetEditor().k_split > 1)
	{
		size_t split_sz = indices.size() / engine.GetEditor().k_split;
		for (size_t i = 0; i < indices.size(); ++i)
		{
			if (i < split_sz)
				l_objs.push_back(objects[indices[i]]);
			else
				r_objs.push_back(objects[indices[i]]);
		}
	}
	else
	{
		size_t median_idx = indices.size() / 2;
		for (size_t i = 0; i < median_idx; ++i)
			l_objs.push_back(objects[indices[i]]);
		for (size_t i = median_idx; i < indices.size(); ++i)
			r_objs.push_back(objects[indices[i]]);
	}
}

bool BVHHelpers::CompareAABB(Object* a, Object* b, int axis)
{
	if (engine.GetEditor().m_use_extents)
		return a->GetAABB().half_extent.p[axis] < b->GetAABB().half_extent.p[axis];
	else if (engine.GetEditor().m_use_centers)
		return a->GetAABB().center.p[axis] < b->GetAABB().center.p[axis];
	else
		return false;
}

bool BVHHelpers::CompareSphere(Object* a, Object* b, int axis)
{
	if (engine.GetEditor().m_use_extents)
		return a->GetSphere().radius < b->GetSphere().radius;
	else if (engine.GetEditor().m_use_centers)
		return a->GetSphere().position.p[axis] < b->GetSphere().position.p[axis];
	else
		return false;
}

AABB BVHHelpers::CombineAABB(AABB a, AABB b)
{
	glm::vec3 minA = a.center.p - a.half_extent.p;
	glm::vec3 maxA = a.center.p + a.half_extent.p;
	glm::vec3 minB = b.center.p - b.half_extent.p;
	glm::vec3 maxB = b.center.p + b.half_extent.p;

	glm::vec3 min_combined = glm::vec3(CMIN(minA.x, minB.x), CMIN(minA.y, minB.y), CMIN(minA.z, minB.z));
	glm::vec3 max_combined = glm::vec3(CMAX(maxA.x, maxB.x), CMAX(maxA.y, maxB.y), CMAX(maxA.z, maxB.z));

	glm::vec3 center = (min_combined + max_combined) / 2.f;
	glm::vec3 halfExtents = (max_combined - min_combined) / 2.f;

	return AABB{ center, halfExtents };
}

BoundingSphere BVHHelpers::CombineBS(BoundingSphere a, BoundingSphere b)
{
	float distance = CMATH::Distance(a.position, b.position);

	if (a.radius >= distance + b.radius)
		return a;
	if (b.radius >= distance + a.radius)
		return b;

	float radius = (distance + a.radius + b.radius) / 2.f;

	glm::vec3 direction = glm::normalize(b.position.p - a.position.p);
	glm::vec3 center = a.position.p + direction * (radius - a.radius);

	return BoundingSphere{ center, radius };
}

float BVHHelpers::ComputeBoundingVolume(const AABB& aabb)
{
	glm::vec3 extents = aabb.half_extent.p * 2.f;
	return extents.x * extents.y * extents.z;
}

float BVHHelpers::ComputeBoundingVolume(const BoundingSphere& sphere)
{
	return (4.f / 3.f) * M_PI * std::powf(sphere.radius, 3.f);
}

float BVHTopDown::ComputeTotalBoundingVolume(const std::vector<Object*>& objects, int axis, BVTYPE type)
{
	if (type == T_AABB)
	{
		glm::vec3 minPoint(FLT_MAX);
		glm::vec3 maxPoint(-FLT_MAX);

		for (const auto& obj : objects)
		{
			glm::vec3 minObj = obj->GetAABB().center.p - obj->GetAABB().half_extent.p;
			glm::vec3 maxObj = obj->GetAABB().center.p + obj->GetAABB().half_extent.p;

			minPoint = glm::vec3(CMIN(minPoint.x, minObj.x), CMIN(minPoint.y, minObj.y), CMIN(minPoint.z, minObj.z));
			maxPoint = glm::vec3(CMAX(maxPoint.x, maxObj.x), CMAX(maxPoint.y, maxObj.y), CMAX(maxPoint.z, maxObj.z));
		}

		glm::vec3 halfExtents = (maxPoint - minPoint) / 2.f;
		AABB combinedAABB{ (minPoint + maxPoint) / 2.f, halfExtents };

		return BVHHelpers::ComputeBoundingVolume(combinedAABB);
	}
	else
	{
		glm::vec3 centroid(0.f);
		for (const auto& sphere : objects)
			centroid += sphere->GetSphere().position.p;
		centroid /= static_cast<float>(objects.size());

		float max_radius = 0.f;
		for (const auto& sphere : objects)
		{
			float distance = CMATH::Distance(centroid, sphere->GetSphere().position.p) + sphere->GetSphere().radius;
			if (distance > max_radius)
				max_radius = distance;
		}
		return BVHHelpers::ComputeBoundingVolume(BoundingSphere{ centroid, max_radius });
	}
}

BVHNode* BVHBotUp::Build(std::vector<Object*>& objects, BVTYPE type)
{
	std::vector<BVHNode*> nodes{};

	for (auto& obj : objects)
		nodes.emplace_back(CreateLeafNode(obj, type));

	while (nodes.size() > 1)
	{
		BVHNode* first{ nullptr };
		BVHNode* second{ nullptr };

		FindNodesToMerge(nodes, first, second, type);

		BVHNode* parent = MergeNode(first, second, type);

		nodes.erase(std::remove(nodes.begin(), nodes.end(), first), nodes.end());
		nodes.erase(std::remove(nodes.begin(), nodes.end(), second), nodes.end());

		nodes.emplace_back(parent);
	}

	return nodes.empty() ? nullptr : nodes[0];
}

void BVHBotUp::FindNodesToMerge(std::vector<BVHNode*> nodes, BVHNode*& first, BVHNode*& second, BVTYPE type)
{
	float best_heuristic = FLT_MAX;

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		for (size_t j = i + 1; j < nodes.size(); ++j)
		{
			float heuristic = CalculateHeuristic(nodes[i], nodes[j], type);

			if (heuristic < best_heuristic)
			{
				best_heuristic = heuristic;
				first = nodes[i];
				second = nodes[j];
			}
		}
	}
}

BVHNode* BVHBotUp::CreateLeafNode(Object* object, BVTYPE type) const
{
	if (type == T_AABB)
	{
		AABBNode* node = new AABBNode();
		node->aabb = object->GetAABB();
		return node;
	}
	else
	{
		BSNode* node = new BSNode();
		node->bs = object->GetSphere();
		return node;
	}
}

BVHNode* BVHBotUp::MergeNode(BVHNode* first, BVHNode* second, BVTYPE type)
{
	if (type == T_AABB)
	{
		AABBNode* parent = new AABBNode();
		parent->aabb = BVHHelpers::CombineAABB(reinterpret_cast<AABBNode*>(first)->aabb, reinterpret_cast<AABBNode*>(second)->aabb);
		parent->left = first;
		parent->right = second;
		return parent;
	}
	else
	{
		BSNode* parent = new BSNode();
		parent->bs = BVHHelpers::CombineBS(reinterpret_cast<BSNode*>(first)->bs, reinterpret_cast<BSNode*>(second)->bs);
		parent->left = first;
		parent->right = second;
		return parent;
	}
}

float BVHBotUp::CalculateHeuristic(BVHNode* first, BVHNode* second, BVTYPE type)
{
	if (!first || !second)
		return FLT_MAX;

	if (type == T_AABB)
	{
		AABB aabb_first		= reinterpret_cast<AABBNode*>(first)->aabb;
		AABB aabb_second	= reinterpret_cast<AABBNode*>(second)->aabb;
		AABB comb_aabb		= BVHHelpers::CombineAABB(aabb_first, aabb_second);

		float distance			= engine.GetEditor().m_nearest_neighbor ? CMATH::Distance(aabb_first.center, aabb_second.center) : 0.f;
		float comb_vol			= engine.GetEditor().m_min_comb_vol ? BVHHelpers::ComputeBoundingVolume(comb_aabb) : 0.f;
		float relative_increase	= engine.GetEditor().m_relative_increase ? (comb_vol - (BVHHelpers::ComputeBoundingVolume(aabb_first) + BVHHelpers::ComputeBoundingVolume(aabb_second))) /
			(BVHHelpers::ComputeBoundingVolume(aabb_first) + BVHHelpers::ComputeBoundingVolume(aabb_second)) : 0.f;

		return distance + comb_vol + relative_increase;
	}
	else
	{
		BoundingSphere bs_first			= reinterpret_cast<BSNode*>(first)->bs;
		BoundingSphere bs_second			= reinterpret_cast<BSNode*>(second)->bs;
		BoundingSphere combined_sphere	= BVHHelpers::CombineBS(bs_first, bs_second);

		float distance			= engine.GetEditor().m_nearest_neighbor ? CMATH::Distance(bs_first.position, bs_second.position) : 0.f;
		float comb_vol			= engine.GetEditor().m_min_comb_vol ? BVHHelpers::ComputeBoundingVolume(combined_sphere) : 0.f;
		float relative_increase	= engine.GetEditor().m_relative_increase ? (comb_vol - (BVHHelpers::ComputeBoundingVolume(bs_first) + BVHHelpers::ComputeBoundingVolume(bs_second))) /
			(BVHHelpers::ComputeBoundingVolume(bs_first) + BVHHelpers::ComputeBoundingVolume(bs_second)) : 0.f;

		return distance + comb_vol + relative_increase;
	}
}