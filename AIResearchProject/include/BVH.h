#ifndef BVH_H
#define BVH_H

#include "Primitives.h"
#include "Object.h"

#include "CustomMath.h"

struct BVHNode
{
	BVHNode* left;
	BVHNode* right;
	int height;
	virtual ~BVHNode() {}
};

struct AABBNode : public BVHNode
{
	AABB aabb;
};

struct BSNode : public BVHNode
{
	BoundingSphere bs;
};

enum BVTYPE
{
	T_AABB,
	T_BS
};

namespace BVHHelpers
{
	AABB CombineAABB(AABB a, AABB b);
	BoundingSphere CombineBS(BoundingSphere a, BoundingSphere b);
	float ComputeBoundingVolume(const AABB& aabb);
	float ComputeBoundingVolume(const BoundingSphere& sphere);
	bool CompareAABB(Object* a, Object* b, int axis);
	bool CompareSphere(Object* a, Object* b, int axis);
}

class BVHTopDown
{
public:
	BVHNode* Build(std::vector<Object*>& objects, BVTYPE type, int depth = 0);

	BVHNode*& GetRoot() { return root; }

	void ClearBVH(BVHNode* node);

private:

	int ChooseSplitAxis(std::vector<Object*> objects, BVTYPE type);
	void SplitObjects(std::vector<Object*> objects, int axis, std::vector<Object*>& l_objs, std::vector<Object*>& r_objs, BVTYPE type);
	BVHNode* CreateLeafNode(std::vector<Object*>& objects, BVTYPE type) const;

	float ComputeTotalBoundingVolume(const std::vector<Object*>& objects, int axis, BVTYPE type);

	const int maxHeight = 7;

	BVHNode* root;
};

class BVHBotUp
{
public:
	BVHNode* Build(std::vector<Object*>& objects, BVTYPE type);
	BVHNode*& GetRoot() { return root; }

	void ClearBVH(BVHNode* node);

private:
	void FindNodesToMerge(std::vector<BVHNode*> nodes, BVHNode*& first, BVHNode*& second, BVTYPE type);
	BVHNode* CreateLeafNode(Object* objects, BVTYPE type) const;
	BVHNode* MergeNode(BVHNode* first, BVHNode* second, BVTYPE type);
	float CalculateHeuristic(BVHNode* first, BVHNode* second, BVTYPE type);
	BVHNode* root;
};

#endif // !BVH_H
