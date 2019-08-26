#include <iostream>

#include "tgaimage.h"
#include "Timer.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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
		float dt = std::abs(dy / static_cast<float>(dx));
		float t = 0.0f;
		for (int x = P0.x, y = P0.y; x < P1.x; ++x)
		{
			// Draw pixel
			bIsSteep ? image.set(y, x, color) : image.set(x, y, color);

			// Calculate y position
			t += dt;
			if (t > 0.5f)
			{
				y += (P1.y > P0.y ? 1 : -1);
				--t;
			}
		}
	}

	Point P0, P1;
};

int main(/*int argc, char** argv*/)
{
	Timer timer;
	// TGAImage image(400, 400, TGAImage::RGB);
	// Line line(100.0f, 100.0f, 300.0f, 300.0f);
	// line.Draw(image, white);

	TGAImage image(400, 400, TGAImage::RGB);

	timer.Start();
	for (int i = 0; i <= 100000; ++i)
	{

		Line line0(52, 80, 320, 160); line0.Draw(image, white);
		Line line1(80, 52, 160, 320); line1.Draw(image, red);
		Line line2(320, 160, 52, 80); line2.Draw(image, red);
	}
	timer.Stop();

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}