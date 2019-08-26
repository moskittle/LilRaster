#include <iostream>
#include <string>

#include "tgaimage.h"
#include "model.h"

#include "Timer.h"

//#define DRAW_MODEL
#define DRAW_TRIANGLE

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 800, height = 800;
Timer timer;

struct Point
{
	Point() = default;
	Point(int _x, int _y) : x(_x), y(_y) {}

	int x = 0, y = 0;
};

struct Line
{
	Line() = default;
	Line(int x0, int y0, int x1, int y1)
		: P0(x0, y0), P1(x1, y1) 
	{
	}
	Line(Point p0, Point p1)
		: P0(p0), P1(p1)
	{
	}

	// Swap X and Y if the line is Steep to avoid holes
	bool SwapXYifSteep()
	{
		int dx = abs(P1.x - P0.x), dy = abs(P1.y - P0.y);
		bool bIsSteep = false;
		if (dx < dy)
		{
			std::swap(P0.x, P0.y);
			std::swap(P1.x, P1.y);
			bIsSteep = true;
		}
		return bIsSteep;
	}

	// Swap points to make sure P0 is on the left
	void SwapP0toLeft()
	{
		if (P0.x > P1.x)
		{
			std::swap(P0.x, P1.x);
			std::swap(P0.y, P1.y);
		}
	}

	// Use Bresenham's line algorithm
	void Draw(TGAImage &image, TGAColor color)
	{
		bool bIsSteep = SwapXYifSteep();

		SwapP0toLeft();

		int dx = P1.x - P0.x;
		int dy = P1.y - P0.y;
		
		//Replaced dt and t with dt*dx*2 and t*dx*2 to avoid floats
		int dt_dx_2 = std::abs(dy) * 2;
		int t_dx_2 = 0;
		for (int x = P0.x, y = P0.y; x < P1.x; ++x)
		{
			// Draw pixel
			bIsSteep ? image.set(y, x, color) : image.set(x, y, color);

			// Calculate y position
			t_dx_2 += dt_dx_2;
			if (t_dx_2 > dx)
			{
				y += (P1.y > P0.y ? 1 : -1);
				t_dx_2 -= dx*2;
			}
		}
	}

	Point P0, P1;
};

struct Triangle
{
	Triangle() = default;
	Triangle(Point p0, Point p1, Point p2)
		: P0(p0), P1(p1), P2(p2)
	{
	}

	void DrawOutline(TGAImage& image, TGAColor color)
	{
		Line L0(P0, P1); L0.Draw(image, color); 
		Line L1(P1, P2); L1.Draw(image, color); 
		Line L2(P2, P0); L2.Draw(image, color);
	}

	void Draw(TGAImage& image, TGAColor color)
	{
		// sort vertices
		if (P0.y > P1.y)
		{
			std::swap(P0.x, P1.x);
			std::swap(P0.y, P1.y);
		}
		if (P0.y > P2.y)
		{
			std::swap(P0.x, P2.x);
			std::swap(P0.y, P2.y);
		}
		if (P1.y > P2.y)
		{
			std::swap(P1.x, P2.x);
			std::swap(P1.y, P2.y);
		}

		float leftBound = 0.0f, rightBound = 0.0f;
		
		// two functions
		float k02 = (P2.y - P0.y) / (float)(P2.x - P0.x);
		float b02 = P0.y - k02 * P0.x;

		float k12 = (P2.y - P1.y) / (float)(P2.x - P1.x);
		float b12 = P1.y - k12 * P1.x;

		float k01 = (P1.y - P0.y) / (float)(P1.x - P0.x);
		float b01 = P1.y - k01 * P1.x;

		for (int y = P0.y; y < P2.y; ++y)
		{
			if (P0.x < P1.x)
			{
				// between P0.y - P1.y
				if (y < P1.y)
				{
					for (int x = (y - b02) / k02; x < (y - b01) / k01; ++x)
					{
						image.set(x, y, color);
					}
				}
				else
				{
					for (int x = (y - b02) / k02; x < (y - b12) / k12; ++x)
					{
						image.set(x, y, color);
					}
				}
			}
			else
			{
				// between P0.y - P1.y
				if (y < P1.y)
				{
					for (int x = (y - b01) / k01; x < (y - b02) / k02; ++x)
					{
						image.set(x, y, color);
					}
				}
				else
				{
					for (int x = (y - b12) / k12; x < (y - b02) / k02; ++x)
					{
						image.set(x, y, color);
					}
				}
			}
		}
	}

	Point P0, P1, P2;
};

int main(/*int argc, char** argv*/)
{
	Timer timer;
	TGAImage image(width, height, TGAImage::RGB);

#ifdef DRAW_TRIANGLE
	Point p0[3] = { Point(10, 70),   Point(50, 160),  Point(70, 80) };
	Point p1[3] = { Point(180, 50),  Point(150, 1),   Point(70, 180) };
	Point p2[3] = { Point(180, 150), Point(120, 160), Point(130, 180) };
	Triangle t0(p0[0], p0[1], p0[2]); t0.Draw(image, red);
	Triangle t1(p1[0], p1[1], p1[2]); t1.Draw(image, white);
	Triangle t2(p2[0], p2[1], p2[2]); t2.Draw(image, green);


#endif // DRAW_TRIANGLE

#ifdef DRAW_MODEL

	// TODO: Unsolved relative path issue
	//char filePath[80] = "E:/dev/LilRizer/LilRizer/obj/";	
	//Model* model = new Model(strcat(filePath, "african_head.obj"));
	//std::string filePath = "../../../LilRizer/obj/";

	std::string filePath = "E:/dev/LilRizer/LilRizer/obj/";
	Model* model = new Model(filePath.append("african_head.obj").c_str());

	timer.Start();
	for (int i = 0; i < model->nfaces(); ++i)
	{
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; ++j)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			Line line2(x0, y0, x1, y1); line2.Draw(image, white);
		}
	}
	timer.Stop();

#endif // DRAW_MODEL

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}