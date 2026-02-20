
#include "Voronoi.h"
#include <iostream>
#include <algorithm>
#include <set>

using namespace vor;

Voronoi::Voronoi()
{
	edges = 0;
}

Edges* Voronoi::GetEdges(Vertices* v, int w, int h)
{
	places = v;
	width = w;
	height = h;
	root = 0;

	if (!edges) edges = new Edges();
	else
	{
		for (Vertices::iterator i = points.begin(); i != points.end(); ++i) delete (*i);
		for (Edges::iterator i = edges->begin(); i != edges->end(); ++i) delete (*i);
		points.clear();
		edges->clear();
	}

	for (Vertices::iterator i = places->begin(); i != places->end(); ++i)
	{
		queue.push(new VEvent(*i, true));
	}

	VEvent* e;
	while (!queue.empty())
	{
		e = queue.top();
		queue.pop();
		ly = e->point->y;
		if (deleted.find(e) != deleted.end()) { delete(e); deleted.erase(e); continue; }
		if (e->pe) InsertParabola(e->point);
		else RemoveParabola(e);
		delete(e);
	}

	FinishEdge(root);

	for (Edges::iterator i = edges->begin(); i != edges->end(); ++i)
	{
		if ((*i)->neighbour)
		{
			(*i)->start = (*i)->neighbour->end;
			delete (*i)->neighbour;
		}
	}

	return edges;
}

void	Voronoi::InsertParabola(VPoint* p)
{
	if (!root) { root = new VParabola(p); return; }

	if (root->isLeaf && root->site->y - p->y < 1) // degenerovaný pøípad - obì spodní místa ve stejné výšce
	{
		VPoint* fp = root->site;
		root->isLeaf = false;
		root->SetLeft(new VParabola(fp));
		root->SetRight(new VParabola(p));
		VPoint* s = new VPoint((p->x + fp->x) / 2, height); // zaèátek hrany uprostøed míst
		points.push_back(s);
		if (p->x > fp->x) root->edge = new VEdge(s, fp, p); // rozhodnu, který vlevo, který vpravo
		else root->edge = new VEdge(s, p, fp);
		edges->push_back(root->edge);
		return;
	}

	VParabola* par = GetParabolaByX(p->x);

	if (par->cEvent)
	{
		deleted.insert(par->cEvent);
		par->cEvent = 0;
	}

	VPoint* start = new VPoint(p->x, GetY(par->site, p->x));
	points.push_back(start);

	VEdge* el = new VEdge(start, par->site, p);
	VEdge* er = new VEdge(start, p, par->site);

	el->neighbour = er;
	edges->push_back(el);

	// pøestavuju strom .. vkládám novou parabolu
	par->edge = er;
	par->isLeaf = false;

	VParabola* p0 = new VParabola(par->site);
	VParabola* p1 = new VParabola(p);
	VParabola* p2 = new VParabola(par->site);

	par->SetRight(p2);
	par->SetLeft(new VParabola());
	par->Left()->edge = el;

	par->Left()->SetLeft(p0);
	par->Left()->SetRight(p1);

	CheckCircle(p0);
	CheckCircle(p2);
}

void	Voronoi::RemoveParabola(VEvent* e)
{
	VParabola* p1 = e->arch;

	VParabola* xl = VParabola::GetLeftParent(p1);
	VParabola* xr = VParabola::GetRightParent(p1);

	VParabola* p0 = VParabola::GetLeftChild(xl);
	VParabola* p2 = VParabola::GetRightChild(xr);

	if (p0 == p2) std::cout << "chyba - pravá a levá parabola má stejné ohnisko!\n";

	if (p0->cEvent) { deleted.insert(p0->cEvent); p0->cEvent = 0; }
	if (p2->cEvent) { deleted.insert(p2->cEvent); p2->cEvent = 0; }

	VPoint* p = new VPoint(e->point->x, GetY(p1->site, e->point->x));
	points.push_back(p);

	xl->edge->end = p;
	xr->edge->end = p;

	VParabola* higher = nullptr;
	VParabola* par = p1;
	while (par != root)
	{
		par = par->parent;
		if (par == xl) higher = xl;
		if (par == xr) higher = xr;
	}
	if (higher == nullptr)
		return;
	higher->edge = new VEdge(p, p0->site, p2->site);
	edges->push_back(higher->edge);

	VParabola* gparent = p1->parent->parent;
	if (p1->parent->Left() == p1)
	{
		if (gparent->Left() == p1->parent) gparent->SetLeft(p1->parent->Right());
		if (gparent->Right() == p1->parent) gparent->SetRight(p1->parent->Right());
	}
	else
	{
		if (gparent->Left() == p1->parent) gparent->SetLeft(p1->parent->Left());
		if (gparent->Right() == p1->parent) gparent->SetRight(p1->parent->Left());
	}

	delete p1->parent;
	delete p1;

	CheckCircle(p0);
	CheckCircle(p2);
}

void	Voronoi::FinishEdge(VParabola* n)
{
	if (n->isLeaf) { delete n; return; }
	double mx;
	if (n->edge->direction->x > 0.0)	mx = std::max(width, n->edge->start->x + 10);
	else							mx = std::min(0.0, n->edge->start->x - 10);

	VPoint* end = new VPoint(mx, mx * n->edge->f + n->edge->g);
	n->edge->end = end;
	points.push_back(end);

	FinishEdge(n->Left());
	FinishEdge(n->Right());
	delete n;
}

double	Voronoi::GetXOfEdge(VParabola* par, double y)
{
	VParabola* left = VParabola::GetLeftChild(par);
	VParabola* right = VParabola::GetRightChild(par);

	VPoint* p = left->site;
	VPoint* r = right->site;

	double dp = 2.0 * (p->y - y);
	double a1 = 1.0 / dp;
	double b1 = -2.0 * p->x / dp;
	double c1 = y + dp / 4 + p->x * p->x / dp;

	dp = 2.0 * (r->y - y);
	double a2 = 1.0 / dp;
	double b2 = -2.0 * r->x / dp;
	double c2 = ly + dp / 4 + r->x * r->x / dp;

	double a = a1 - a2;
	double b = b1 - b2;
	double c = c1 - c2;

	double disc = b * b - 4 * a * c;
	double x1 = (-b + std::sqrt(disc)) / (2 * a);
	double x2 = (-b - std::sqrt(disc)) / (2 * a);

	double ry;
	if (p->y < r->y) ry = std::max(x1, x2);
	else ry = std::min(x1, x2);

	return ry;
}

VParabola* Voronoi::GetParabolaByX(double xx)
{
	VParabola* par = root;
	double x = 0.0;

	while (!par->isLeaf) // projdu stromem dokud nenarazím na vhodný list
	{
		x = GetXOfEdge(par, ly);
		if (x > xx) par = par->Left();
		else par = par->Right();
	}
	return par;
}

double	Voronoi::GetY(VPoint* p, double x) // ohnisko, x-souøadnice
{
	double dp = 2 * (p->y - ly);
	double a1 = 1 / dp;
	double b1 = -2 * p->x / dp;
	double c1 = ly + dp / 4 + p->x * p->x / dp;

	return(a1 * x * x + b1 * x + c1);
}

void	Voronoi::CheckCircle(VParabola* b)
{
	VParabola* lp = VParabola::GetLeftParent(b);
	VParabola* rp = VParabola::GetRightParent(b);

	VParabola* a = VParabola::GetLeftChild(lp);
	VParabola* c = VParabola::GetRightChild(rp);

	if (!a || !c || a->site == c->site) return;

	VPoint* s = 0;
	s = GetEdgeIntersection(lp->edge, rp->edge);
	if (s == 0) return;

	double dx = a->site->x - s->x;
	double dy = a->site->y - s->y;

	double d = std::sqrt((dx * dx) + (dy * dy));

	if (s->y - d >= ly) { return; }

	VEvent* e = new VEvent(new VPoint(s->x, s->y - d), false);
	points.push_back(e->point);
	b->cEvent = e;
	e->arch = b;
	queue.push(e);
}

VPoint* Voronoi::GetEdgeIntersection(VEdge* a, VEdge* b)
{
	double x = (b->g - a->g) / (a->f - b->f);
	double y = a->f * x + a->g;

	if ((x - a->start->x) / a->direction->x < 0) return 0;
	if ((y - a->start->y) / a->direction->y < 0) return 0;

	if ((x - b->start->x) / b->direction->x < 0) return 0;
	if ((y - b->start->y) / b->direction->y < 0) return 0;

	VPoint* p = new VPoint(x, y);
	points.push_back(p);
	return p;
}

bool vor::equalEdges(vEdge e1, vEdge e2)
{
	return (e1.p1.x == e2.p1.x && e1.p1.y == e2.p1.y && e1.p2.x == e2.p2.x && e1.p2.y == e2.p2.y) ||
		(e1.p1.x == e2.p2.x && e1.p1.y == e2.p2.y && e1.p2.x == e2.p1.x && e1.p2.y == e2.p1.y);
}

std::vector<vEdge> vor::uniqueEdges(const std::vector<vEdge>& edges)
{
	std::vector<vEdge> unique;
	for (const auto& e : edges)
	{
		bool found = false;
		for (const auto& ue : unique)
		{
			if (equalEdges(e, ue))
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			unique.push_back(e);
		}
	}
	return unique;
}

std::vector<Triangle> vor::delaunayTriangulation(const std::vector<Point>& points)
{
	// Create a super triangle
	float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest(), maxY = std::numeric_limits<float>::lowest();
	for (const auto& p : points)
	{
		if (p.x < minX) minX = p.x;
		if (p.y < minY) minY = p.y;
		if (p.x > maxX) maxX = p.x;
		if (p.y > maxY) maxY = p.y;
	}
	float dx = maxX - minX;
	float dy = maxY - minY;
	float deltaMax = std::max(dx, dy);
	float midx = (minX + maxX) / 2;
	float midy = (minY + maxY) / 2;
	Point p1 = { midx - 2 * deltaMax, midy - deltaMax };
	Point p2 = { midx, midy + 2 * deltaMax };
	Point p3 = { midx + 2 * deltaMax, midy - deltaMax };
	Triangle superTriangle(p1, p2, p3);

	// Initialize the triangulation with the super triangle
	std::vector<Triangle> triangulation;
	triangulation.push_back(superTriangle);

	// Add each point one at a time to the triangulation
	for (const auto& p : points)
	{
		std::vector<vEdge> polygon;
		std::vector<Triangle> newTriangles;

		// Find all triangles that are no longer valid due to the insertion
		for (auto it = triangulation.begin(); it != triangulation.end();)
		{
			if (it->containsPoint(p))
			{
				polygon.push_back({ it->p1, it->p2 });
				polygon.push_back({ it->p2, it->p3 });
				polygon.push_back({ it->p3, it->p1 });
				it = triangulation.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Remove duplicate edges from the polygon
		polygon = uniqueEdges(polygon);

		// Create new triangles from the point and the polygon edges
		for (const auto& e : polygon)
		{
			newTriangles.emplace_back(e.p1, e.p2, p);
		}

		// Add the new triangles to the triangulation
		triangulation.insert(triangulation.end(), newTriangles.begin(), newTriangles.end());
	}

	// Remove triangles that share a vertex with the super triangle
	for (auto it = triangulation.begin(); it != triangulation.end();)
	{
		if (it->p1 == p1 || it->p1 == p2 || it->p1 == p3 ||
			it->p2 == p1 || it->p2 == p2 || it->p2 == p3 ||
			it->p3 == p1 || it->p3 == p2 || it->p3 == p3)
		{
			it = triangulation.erase(it);
		}
		else
		{
			++it;
		}
	}

	return triangulation;
}

std::vector<Triangle> vor::computeDelaunayFromVoronoi(Vertices* v, Edges* edges)
{
	std::vector<Triangle> delaunayTriangles;

	// Collect all unique points from the Voronoi diagram
	std::set<Point> uniquePoints;
	for (auto& edge : *edges)
	{
		uniquePoints.insert({ static_cast<float>(edge->start->x), static_cast<float>(edge->start->y) });
		uniquePoints.insert({ static_cast<float>(edge->end->x), static_cast<float>(edge->end->y) });
	}

	// Convert set to vector
	std::vector<Point> points(uniquePoints.begin(), uniquePoints.end());

	// Create triangles
	for (auto& edge : *edges)
	{
		Point p1 = { static_cast<float>(edge->start->x), static_cast<float>(edge->start->y) };
		Point p2 = { static_cast<float>(edge->end->x), static_cast<float>(edge->end->y) };

		for (auto& p : points)
		{
			if (p.x == p1.x && p.y == p1.y) continue;
			if (p.x == p2.x && p.y == p2.y) continue;
			delaunayTriangles.emplace_back(p1, p2, p);
		}
	}

	return delaunayTriangles;
}
