#include "BezierPath.h"
//#include "Vector2D.h"


BezierPath::BezierPath() {

}

BezierPath::~BezierPath()
{
}


void BezierPath::AddCurve(BezierCurve curve, int samples) {
	Curves.push_back(curve);
	Samples.push_back(samples);
}

void BezierPath::Sample(std::vector<Vector2D>* samplePath) {

	for (int i = 0; i < Curves.size(); i++) {
		for (float t = 0; t <= 1.0f; t += (1.0f / Samples[i])) {
			samplePath->push_back(Curves[i].CalculateCurvePoint(t));
		}
	}
}