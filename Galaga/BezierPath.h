#pragma once

#include <vector> 
#include "Vector2D.h" 


struct BezierCurve {
	Vector2D p0;
	Vector2D p1;
	Vector2D p2;
	Vector2D p3;

	Vector2D CalculateCurvePoint(float t) {
		float tSquared = t * t;
		float tCubed = tSquared * t;
		float u = 1.0f - t;
		float uSquared = u * u;
		float uCubed = uSquared * u;
		Vector2D point = (p0 * uCubed) + (p1 * 3 * uSquared * t) + (p2 * 3 * u * tSquared) + (p3 * tCubed);
		point.x = round(point.x);
		point.y = round(point.y);
		return point;
	}
};

class BezierPath {

private:
	std::vector<BezierCurve> Curves;
	std::vector<int> Samples;

public:
	BezierPath();
	~BezierPath();

	void AddCurve(BezierCurve curve, int samples);
	void Sample(std::vector<Vector2D>* samplePath);

};