#define OLC_PGE_APPLICATION

#include <iostream>
#include "olcPixelGameEngine.h"
#include <vector>
#include <stack>
#include <algorithm>

struct Vertex {
	olc::Vector2 position;
	float polarAngle;

	Vertex(){}

	Vertex(olc::Vector2 _position, float _polarAngle) {
		position = _position;
		polarAngle = _polarAngle;
	}
};


static bool compareByPosition(const Vertex& a, const Vertex& b) {
	return (a.position.y < b.position.y);
}

static bool compareByPolarAngle(const Vertex& a, const Vertex& b) {
	return (a.polarAngle < b.polarAngle);
}

struct Triangle {
	std::vector<olc::Vector2> vertices;

	Triangle()
	{
		vertices.reserve(3);
	}

};

class Application : public olc::PixelGameEngine {

	std::vector<Vertex> vertices;

public:
	Application() 
	{
		sAppName = "Triangulation";
	}

	bool OnUserCreate() override 
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetMouse(0).bPressed) {
			vertices.emplace_back(Vertex(olc::Vector2(GetMouseX(), GetMouseY()), 0));
			FillCircle(GetMouseX(), GetMouseY(), 5.0f, olc::YELLOW);
		}
		else if (GetMouse(1).bPressed) {
			std::sort(vertices.begin(), vertices.end(), compareByPosition);
			vertices[0].polarAngle = -99;
			CalculatePolarAngles(vertices[0]);
			std::sort(vertices.begin(), vertices.end(), compareByPolarAngle);
			for (int i = 0; i < vertices.size(); ++i) {
				DrawString(vertices[i].position.x - 20, vertices[i].position.y - 5, std::to_string(i), olc::RED);
			}
			GrahamScan();
			//for (int i = 0; i < vertices.size() - 1; ++i) {
			//	DrawLine(vertices[i].position.x, vertices[i].position.y, vertices[i + 1].position.x, vertices[i + 1].position.y, olc::WHITE);
			//	DrawString(vertices[i].position.x + 5, vertices[i].position.y + 5, std::to_string(vertices[i].polarAngle));
			//}
		}

		return true;
	}

private:
	void GrahamScan() {

		if (vertices.size() < 3) {
			std::cout << "Too few vertices." << std::endl;
		}
		std::stack<Vertex> currentPath;
		Vertex parentBegin, parentEnd;

		currentPath.push(vertices[0]);
		currentPath.push(vertices[1]);	
		currentPath.push(vertices[2]);

		parentBegin = vertices[0];
		parentEnd = vertices[2];
		
		int verticesIndex = 3;
		float dir;

		while (verticesIndex < vertices.size()) {
			dir = CalculateRelativeAngle(parentBegin, parentEnd, vertices[verticesIndex]);

			if (dir > 0) {
				parentBegin = currentPath.top();
				currentPath.push(vertices[verticesIndex]);
				parentEnd = currentPath.top();
			}
			else if (dir < 0) {
				currentPath.pop();
				do {
					parentEnd = currentPath.top();
					//std::cout<< verticesIndex <<std::endl;
					currentPath.pop();
					parentBegin = currentPath.top();
				} while (CalculateRelativeAngle(parentBegin, parentEnd, vertices[verticesIndex]) < 0);
				currentPath.push(parentBegin);
				currentPath.push(parentEnd);
				parentBegin = currentPath.top();
				currentPath.push(vertices[verticesIndex]);
				parentEnd = currentPath.top();
			}
			verticesIndex++;
		}

		int i = 0;
		Vertex start = currentPath.top();
		Vertex endV = start;
		currentPath.pop();
		while (currentPath.size() > 0) {
			Vertex v = currentPath.top();
			DrawLine(endV.position.x, endV.position.y, v.position.x, v.position.y, olc::YELLOW);
			endV = v;
			currentPath.pop();
			DrawString(v.position.x + 10, v.position.y + 5, std::to_string(i));
			i++;
		}
		DrawLine(endV.position.x, endV.position.y, start.position.x, start.position.y, olc::YELLOW);
	}

	float CalculateRelativeAngle(const Vertex& parentBegin, const Vertex& parentEnd, const Vertex& checkEnd) {
		return((checkEnd.position.x - parentBegin.position.x) * (parentEnd.position.y - parentBegin.position.y)
			- (checkEnd.position.y - parentBegin.position.y) * (parentEnd.position.x - parentBegin.position.x));
	}
	
		void CalculatePolarAngles(const Vertex &p0) {
		if (vertices.size() == 0) return;
		float dx, dy;
		float angle;
		for (auto &v : vertices) {
			if (&v != &p0) {
				dx = v.position.x - p0.position.x;
				dy = v.position.y - p0.position.y;

				angle = atan2(dx, dy);
				v.polarAngle = angle;
			}
		}
	}

	
};

int main(int argc, char* argv[]) {

	Application myApplication;

	if (myApplication.Construct(800, 800, 1, 1)) {
		myApplication.Start();
	}

	return 0;
}