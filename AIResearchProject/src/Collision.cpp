#include "Collision.h"
#include "Engine.h"
#include "includes.h"

void Collision::Init()
{
	m_objects = &engine.GetRenderer().GetObjects();
}

void Collision::Update()
{
	for (auto& obj1 : *m_objects)
	{
		for (auto& obj2 : *m_objects)
		{
			if (obj1 == obj2)
				continue;

			unsigned int obj1_attribs = obj1->GetAttribs();
			unsigned int obj2_attribs = obj2->GetAttribs();

			unsigned int active = obj1_attribs | obj2_attribs;

			if (active == 0)
				continue;

			int bits_active = 0;
			while (active) 
			{
				bits_active += active & 1;
				active >>= 1;
			}

			if (bits_active < 2 && obj1_attribs != obj2_attribs)
			{
				if (bits_active == 1)
				{
					if (obj2->IsAttribActive(Object::ATTRIB_AABB))
						obj1->Intersection() = obj2->Intersection() = TESTS::PointAABB(obj1->GetTranslate(), obj2->GetAABB());
					else if (obj2->IsAttribActive(Object::ATTRIB_SPHERE))
						obj1->Intersection() = obj2->Intersection() = TESTS::PointSphere(obj1->GetTranslate(), obj2->GetSphere());
					else if (obj2->IsAttribActive(Object::ATTRIB_PLANE))
						obj1->Intersection() = obj2->Intersection() = TESTS::PointPlane(obj1->GetTranslate(), obj2->GetPlane());
					else if (obj2->IsAttribActive(Object::ATTRIB_TRIANGLE))
						obj1->Intersection() = obj2->Intersection() = TESTS::PointTriangle(obj1->GetTranslate(), obj2->GetTriangle());
				}
				else
				{
					obj1->Intersection() = obj2->Intersection() = false;
					continue;
				}
			}
			else
			{
				if (obj1->IsAttribActive(Object::ATTRIB_PLANE))
				{
					if (obj2->IsAttribActive(Object::ATTRIB_AABB))
						obj1->Intersection() = obj2->Intersection() = TESTS::PlaneAABB(obj1->GetPlane(), obj2->GetAABB());
					else if (obj2->IsAttribActive(Object::ATTRIB_SPHERE))
						obj1->Intersection() = obj2->Intersection() = TESTS::PlaneSphere(obj1->GetPlane(), obj2->GetSphere());
				}

				else if (obj1->IsAttribActive(Object::ATTRIB_AABB))
				{
					if (obj2->IsAttribActive(Object::ATTRIB_AABB))
						obj1->Intersection() = obj2->Intersection() = TESTS::AABBAABB(obj1->GetAABB(), obj2->GetAABB());
					else if (obj2->IsAttribActive(Object::ATTRIB_SPHERE))
						obj1->Intersection() = obj2->Intersection() = TESTS::AABBSphere(obj1->GetAABB(), obj2->GetSphere());
				}

				else if (obj1->IsAttribActive(Object::ATTRIB_SPHERE))
				{
					if (obj2->IsAttribActive(Object::ATTRIB_AABB))
						obj1->Intersection() = obj2->Intersection() = TESTS::SphereAABB(obj1->GetSphere(), obj2->GetAABB());
					else if (obj2->IsAttribActive(Object::ATTRIB_SPHERE))
						obj1->Intersection() = obj2->Intersection() = TESTS::SphereSphere(obj1->GetSphere(), obj2->GetSphere());
				}

				else if (obj1->IsAttribActive(Object::ATTRIB_RAY))
				{
					if (obj2->IsAttribActive(Object::ATTRIB_AABB))
						obj1->Intersection() = obj2->Intersection() = TESTS::RayAABB(obj1->GetRay(), obj2->GetAABB());
					else if (obj2->IsAttribActive(Object::ATTRIB_SPHERE))
						obj1->Intersection() = obj2->Intersection() = TESTS::RaySphere(obj1->GetRay(), obj2->GetSphere());
					else if (obj2->IsAttribActive(Object::ATTRIB_PLANE))
						obj1->Intersection() = obj2->Intersection() = TESTS::RayPlane(obj1->GetRay(), obj2->GetPlane());
					else if (obj2->IsAttribActive(Object::ATTRIB_TRIANGLE))
						obj1->Intersection() = obj2->Intersection() = TESTS::RayTriangle(obj1->GetRay(), obj2->GetTriangle());
				}
			}			
		}
	}
}

void Collision::End()
{
}
