#include <iostream>
#include <string>

#include "tgaimage.h"
#include "model.h"

#include "Timer.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
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

int main(/*int argc, char** argv*/)
{
	Timer timer;
	TGAImage image(width, height, TGAImage::RGB);

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

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}