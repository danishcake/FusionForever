#pragma once

#include "vmath.h"
#include <cmath>

/**
* Class for 2D collisions.
* @note If you use a Vector3 derivative then the z component is ignored
*/
template <class T>
class Collisions2
{
public:
	/**
	* Finds the square of the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T DistanceSqr(Vector2<T> a, Vector2<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return x * x + y * y;
	}


	/**
	* Finds the square of the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T DistanceSqr(Vector3<T> a, Vector3<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return x * x + y * y;
	}

	/**
	* Finds the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T Distance(Vector2<T> a, Vector2<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return (T)sqrt((double)(x * x + y * y));
	}

	/**
	* Finds the distance between two points
	* @param a Position of first point
	* @param b Position of second point
	*/
	static T Distance(Vector3<T> a, Vector3<T> b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;

		return (T)sqrt((double)(x * x + y * y));
	}

	/**
	* Determines if a point is within a circle
	* @param a Point being tested
	* @param b Centre of circle
	* @param radius Radius of circle
	*/
	static bool PointInCircle(Vector2<T> a, Vector2<T> b, T radius)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		return x * x + y * y < radius *  radius;
	}

	/**
	* Determines if a point is within a circle
	* @param a Point being tested
	* @param b Centre of circle
	* @param radius Radius of circle
	*/
	static bool PointInCircle(Vector3<T> a, Vector3<T> b, T radius)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		return x * x + y * y < radius *  radius;
	}

	/**
	* Determines if a point is within a circle
	* @param a Centre of circle A
	* @param radius_a Radius of circle A
	* @param b Centre of circle B
	* @param radius_b Radius of circle B
	*/
	static bool CirclesIntersect(Vector2<T> a, T radius_a, Vector2<T> b, T radius_b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		T r = (radius_a + radius_b);

		return x * x + y * y < r * r;
	}

	/**
	* Determines if a point is within a circle
	* @param a Centre of circle A
	* @param radius_a Radius of circle A
	* @param b Centre of circle B
	* @param radius_b Radius of circle B
	*/
	static bool CirclesIntersect(Vector3<T> a, T radius_a, Vector3<T> b, T radius_b)
	{
		T x = a.x - b.x;
		T y = a.y - b.y;
		T r = (radius_a + radius_b);

		return x * x + y * y < r * r;
	}

	/**
	* Determines if line segment intersects with a triangle.
	* @param a_1 Point 1 in triangle
	* @param a_2 Point 2 in triangle
	* @param a_3 Point 3 in triangle
	* @param b_1 Start of line B
	* @param b_2 End of line B
	*/
	static bool LineTriangleIntersect(Vector2<T> a_1, Vector2<T> a_2, Vector2<T> a_3, Vector2<T> b_1, Vector2<T> b_2)
	{
		Vector2<T> out; //Ignored
		if(Collisions2<T>::LineSegmentsIntersect(a_1, a_2, b_1, b_2, &out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_1, a_3, b_1, b_2, &out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_2, a_3, b_1, b_2, &out))
		   return true;
		return false;

	}
	
	/**
	* Determines if line segment intersects with a triangle.
	* @param a_1 Point 1 in triangle
	* @param a_2 Point 2 in triangle
	* @param a_3 Point 3 in triangle
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	
	static bool LineTriangleIntersect(Vector3<T> a_1, Vector3<T> a_2, Vector3<T> a_3, Vector3<T> b_1, Vector3<T> b_2)
	{
		Vector3<T> out; //Ignored
		if(Collisions2<T>::LineSegmentsIntersect(a_1, a_2, b_1, b_2, out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_1, a_3, b_1, b_2, out) ||
		   Collisions2<T>::LineSegmentsIntersect(a_2, a_3, b_1, b_2, out))
		   return true;
		return false;
	}


	/**
	* Determines if two line segments intersect. If true then out is set to intersection point
	* @param a_1 Start of line A
	* @param a_2 End of line A
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	static bool LineSegmentsIntersect(Vector2<T> a_1, Vector2<T> a_2, Vector2<T> b_1, Vector2<T> b_2, Vector2<T>& out)
	{
		Vector2<T> da = a_2 - a_1;
		Vector2<T> db = b_2 - b_1;
		T d = db.y * da.x - db.x * da.y;

		if(d != 0)
		{
			T u_a = ((db.x * (a_1.y - b_1.y)) - (db.y * (a_1.x - b_1.x))) / d;
			T u_b = ((da.x * (a_1.y - b_1.y)) - (da.y * (a_1.x - b_1.x))) / d;

			if(u_a < 1 && u_a > 0 && u_b > 0 && u_b < 1)
			{
				out.x = a_1.x + u_a * da.x;
				out.y = a_1.y + u_a * da.y;
				return true;
			}
			else
			{
				return false;
			}

		}
		else //Parallel lines
		{
			return false;
		}
	}
	/**
	* Determines if two line segments intersect. If true then out is set to intersection point
	* @param a_1 Start of line A
	* @param a_2 End of line A
	* @param b_1 Start of line B
	* @param b_2 End of line B
	* @param out Intersection point.
	*/
	static bool LineSegmentsIntersect(Vector3<T> a_1, Vector3<T> a_2, Vector3<T> b_1, Vector3<T> b_2, Vector3<T>& out)
	{
		Vector3<T> da = a_2 - a_1;
		Vector3<T> db = b_2 - b_1;
		T d = db.y * da.x - db.x * da.y;

		if(d != 0)
		{
			T u_a = ((db.x * (a_1.y - b_1.y)) - (db.y * (a_1.x - b_1.x))) / d;
			T u_b = ((da.x * (a_1.y - b_1.y)) - (da.y * (a_1.x - b_1.x))) / d;

			if(u_a < 1 && u_a > 0 && u_b > 0 && u_b < 1)
			{
				out.x = a_1.x + u_a * da.x;
				out.y = a_1.y + u_a * da.y;
				return true;
			}
			else
			{
				return false;
			}

		}
		else //Parallel lines
		{
			return false;
		}
	}

	/**
	* Determines if a point is with a triangle.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @param p The point to test
	* @note Using algorithm described at http://www.mathforum.org/library/drmath/view/54386.html
	*/
	static bool PointInTriangle(Vector2<T> a, Vector2<T> b, Vector2<T> c, Vector2<T> p)
	{
		T abp = TriangleAreaDouble(a,b,p);
		T bcp = TriangleAreaDouble(b,c,p);
		T cap = TriangleAreaDouble(c,a,p);
		return ((abp > 0) && (bcp > 0) && (cap >0)) || ((abp < 0) && (bcp < 0) && (cap < 0));
	}
	/**
	* Determines if a point is with a triangle.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @param p The point to test
	* @note Using algorithm described at http://www.mathforum.org/library/drmath/view/54386.html
	*/
	static bool PointInTriangle(Vector3<T> a, Vector3<T> b, Vector3<T> c, Vector3<T> p)
	{
		T abp = TriangleAreaDouble(a,b,p);
		T bcp = TriangleAreaDouble(b,c,p);
		T cap = TriangleAreaDouble(c,a,p);
		return ((abp > 0) && (bcp > 0) && (cap >0)) || ((abp < 0) && (bcp < 0) && (cap < 0));
	}
	/**
	* Determines if a point is with a triangle.
	* @param a Pointer to array of Vector2 describing a convex hull
	* @param num_points The number of Vector2 in a
	* @param p The point to be tested
	*/
	static bool PointInConvex(Vector2<T>* a, int num_points, Vector2<T> p)
	{
		bool allPositive = true;
		bool allNegative = true;
		for(int i = 0; i < num_points-1; i++)
		{
			T aread = TriangleAreaDouble(a[i], a[i+1], p);
			if(aread > 0)
				allNegative = false;
			if(aread < 0)
				allNegative = false;
		}
		T aread2 = TriangleAreaDouble(a[num_points-1], a[0], p);
		if(aread2 > 0)
			allNegative = false;
		if(aread2 < 0)
			allNegative = false;
		return allNegative || allPositive;
	}
   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(Vector2<T> P1, Vector2<T> P2, Vector2<T> c, T radius)
   {
      Vector2<T> L = P2 - P1;
	  Vector2<T> out;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + beta * L;
      if((beta >=0.0f) && (beta <=1.0f) &&
         (DistanceSqr(out, c) < radius * radius))
         return true;
      else
         return false;
   }

   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(Vector3<T> P1, Vector3<T> P2, Vector3<T> c, T radius)
   {
      Vector3<T> L = P2 - P1;
	  Vector3<T> out;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + L * beta;
      if((beta >=0.0f) && (beta <=1.0f) &&
         (DistanceSqr(out, c) < radius * radius))
         return true;
      else
         return false;
   }


   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(Vector2<T> P1, Vector2<T> P2, Vector2<T> c, T radius, Vector2<T>& out)
   {
      Vector2<T> L = P2 - P1;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + beta * L;
      if((beta >=0.0f) && (beta <=1.0f) &&
         (DistanceSqr(out, c) < radius * radius))
         return true;
      else
         return false;
   }

   /**
   * Determines if a line segment enters a circle
   * @param P1 The first point in the line segment
   * @param P2 The second point in the line segment
   * @param c The centre of the circle
   * @param radius The radius of the circle
   * @param out The point of closest approach
   */
   static bool LineInCircle(Vector3<T> P1, Vector3<T> P2, Vector3<T> c, T radius, Vector3<T>& out)
   {
      Vector3<T> L = P2 - P1;
      T beta = ((c.x - P1.x) * L.x - (P1.y - c.y) * L.y) / L.lengthSq();
      if(beta < 0)
         out = P1;
      else if (beta >1)
         out = P2;
      else
         out = P1 + beta * L;
      if((beta >=0.0f) && (beta <=1.0f) &&
         (DistanceSqr(out, c) < radius * radius))
         return true;
      else
         return false;
   }

private:
	/**
	* Gets twice the area of a triangle using the determinant method.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @note If described CW then the result is positive, if CCW then negative
	*/
	static T TriangleAreaDouble(Vector2<T> a, Vector2<T> b, Vector2<T> c)
	{
		return b.x * c.y - b.y * c.x -
			   a.x * c.y + a.y * c.x +
			   a.x * b.y - a.y * b.x;
	}

	/**
	* Gets twice the area of a triangle using the determinant method.
	* @param a Triangle point a
	* @param b Triangle point b
	* @param c Triangle point c
	* @note If described CW then the result is positive, if CCW then negative
	*/
	static T TriangleAreaDouble(Vector3<T> a, Vector3<T> b, Vector3<T> c)
	{
		return b.x * c.y - b.y * c.x -
			   a.x * c.y + a.y * c.x +
			   a.x * b.y - a.y * b.x;
	}
};

typedef Collisions2<float> Collisions2f;
typedef Collisions2<double> Collisions2d;














/*template <class T>
class Collisions3
{

};

typedef Collisions3<Vector3f> Collisions3f;
typedef Collisions3<Vector3d> Collisions3d;

template <class T>
class Collisions4
{

};

typedef Collisions4<Vector4f> Collisions4f;
typedef Collisions4<Vector4d> Collisions4d;
*/
