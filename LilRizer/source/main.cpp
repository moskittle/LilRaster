#include <iostream>

#include "tgaimage.h"
#include "Timer.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

struct Vec2
{
	Vec2() = default;
	Vec2(int _x, int _y) : x(_x), y(_y) {}

	int x = 0, y = 0;
};

struct Line
{
	Line() = default;
	Line(int x0, int y0, int x1, int y1)
	{
		P0.x = x0; P0.y = y0; P1.x = x1; P1.y = y1;
	}

	bool IsSteep()
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

	void Draw(TGAImage &image, TGAColor color)
	{
		bool bIsSteep = IsSteep();

		int dx = P1.x - P0.x;
		for (float x = P0.x, t = 0.0f, y = 0.0f; x < P1.x; ++x)
		{
			// Draw
			(bIsSteep) ? image.set(x, y, color) : image.set(y, x, color);

			// Calculate y position
			t = (x - P0.x) / dx;
			y = (1 - t) * P0.y + t * P1.y;
		}
	}

	Vec2 P0, P1;
};

int main(/*int argc, char** argv*/)
{
	Timer timer;
	// TGAImage image(400, 400, TGAImage::RGB);
	// Line line(100.0f, 100.0f, 300.0f, 300.0f);
	// line.Draw(image, white);

	TGAImage image(400, 400, TGAImage::RGB);

	timer.Start();
	for (int i = 0; i <= 1000000; ++i)
	{

		Line line0(52, 80, 320, 160); line0.Draw(image, white);
		Line line1(80, 52, 160, 320); line1.Draw(image, red);
		Line line2(320, 160, 52, 80); line0.Draw(image, red);
	}
	timer.Stop();

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}