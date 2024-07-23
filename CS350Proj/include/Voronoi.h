#ifndef Voronoi_h
#define Voronoi_h

#include <list>
#include <queue>
#include <set>

#include "VPoint.h"
#include "VEdge.h"
#include "VParabola.h"
#include "VEvent.h"
#include <functional>

namespace vor
{
	/*
		Useful data containers for Vertices (places) and Edges of Voronoi diagram
	*/

	typedef std::list<VPoint*>		Vertices;
	typedef std::list<VEdge*>		Edges;

	/*
		Class for generating the Voronoi diagram
	*/

	class Voronoi
	{
	public:

		/*
			Constructor - without any parameters
		*/

		Voronoi();

		/*
			The only public function for generating a diagram

			input:
				v		: Vertices - places for drawing a diagram
				w		: width  of the result (top left corner is (0, 0))
				h		: height of the result
			output:
				pointer to list of edges

			All the data structures are managed by this class
		*/

		Edges* GetEdges(Vertices* v, int w, int h);

	private:

		/*
						places		: container of places with which we work
						edges		: container of edges which will be teh result
						width		: width of the diagram
						height		: height of the diagram
						root		: the root of the tree, that represents a beachline sequence
						ly			: current "y" position of the line (see Fortune's algorithm)
		*/

		Vertices* places;
		Edges* edges;
		double			width, height;
		VParabola* root;
		double			ly;

		/*
						deleted		: set  of deleted (false) Events (since we can not delete from PriorityQueue
						points		: list of all new points that were created during the algorithm
						queue		: priority queue with events to process
		*/

		std::set<VEvent*>	deleted;
		std::list<VPoint*> points;
		std::priority_queue<VEvent*, std::vector<VEvent*>, VEvent::CompareEvent> queue;

		/*
						InsertParabola		: processing the place event
						RemoveParabola		: processing the circle event
						FinishEdge			: recursively finishes all infinite edges in the tree
						GetXOfEdge			: returns the current x position of an intersection point of left and right parabolas
						GetParabolaByX		: returns the Parabola that is under this "x" position in the current beachline
						CheckCircle			: checks the circle event (disappearing) of this parabola
						GetEdgeInterse
		*/

		void		InsertParabola(VPoint* p);
		void		RemoveParabola(VEvent* e);
		void		FinishEdge(VParabola* n);
		double		GetXOfEdge(VParabola* par, double y);
		VParabola* GetParabolaByX(double xx);
		double		GetY(VPoint* p, double x);
		void		CheckCircle(VParabola* b);
		VPoint* GetEdgeIntersection(VEdge* a, VEdge* b);
	};

	struct Point
	{
		float x, y;
		Point(float a, float b) : x(a), y(b) {}
		Point() = default;

		bool operator==(Point a)
		{
			return (a.x == x && a.y == y);
		}

		bool operator<(const Point& other) const
		{
			return (x < other.x) || (x == other.x && y < other.y);
		}
	};

	// Define a simple Triangle structure
	struct Triangle
	{
		Point p1, p2, p3;
		Point circumcenter;
		float circumradius;

		Triangle(Point a, Point b, Point c) : p1(a), p2(b), p3(c)
		{
			calculateCircumcircle();
		}

		void calculateCircumcircle()
		{
			float ax = p1.x, ay = p1.y;
			float bx = p2.x, by = p2.y;
			float cx = p3.x, cy = p3.y;

			float D = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
			float Ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / D;
			float Uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / D;

			circumcenter = { Ux, Uy };
			circumradius = sqrt((Ux - ax) * (Ux - ax) + (Uy - ay) * (Uy - ay));
		}

		bool containsPoint(Point p)
		{
			float dist = sqrt((p.x - circumcenter.x) * (p.x - circumcenter.x) + (p.y - circumcenter.y) * (p.y - circumcenter.y));
			return dist < circumradius;
		}
	};

	// Define a simple edge structure
	struct vEdge
	{
		Point p1, p2;

		bool operator==(const vEdge& e) const
		{
			return (p1.x == e.p1.x && p1.y == e.p1.y && p2.x == e.p2.x && p2.y == e.p2.y) ||
				(p1.x == e.p2.x && p1.y == e.p2.y && p2.x == e.p1.x && p2.y == e.p1.y);
		}
	};

	// Function to check if two edges are equal
	bool equalEdges(vEdge e1, vEdge e2);

	// Function to remove duplicate edges
	std::vector<vEdge> uniqueEdges(const std::vector<vEdge>& edges);

	// Main function to perform Delaunay triangulation using the Bowyer-Watson algorithm
	std::vector<Triangle> delaunayTriangulation(const std::vector<Point>& points);

	std::vector<Triangle> computeDelaunayFromVoronoi(Vertices* v, Edges* edges);
}

#endif