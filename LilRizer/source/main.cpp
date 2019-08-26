#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

struct Vec2
{
	Vec2() = default;
	Vec2(float _x, float _y) : x(_x), y(_y) {}

	float x = 0.0f, y = 0.0f;
};

struct Line
{
	Line() = default;
	Line(float x0, float y0, float x1, float y1)
	{
		P0.x = x0; P0.y = y0; P1.x = x1; P1.y = y1;
	}

	void Draw(TGAImage &image, TGAColor color)
	{
		float t = 0.0f, y = 0.0f;
		for (float x = P0.x; x < P1.x; ++x)
		{
			t = (x - P0.x) / (P1.x - P0.x);
			//std::cout << t << std::endl;
			y = (1 - t) * P0.y + t * P1.y;
			image.set(x, y, color);
			//std::cout << x << ", " << y << std::endl;
		}
	}

	Vec2 P0, P1;
};

int main(/*int argc, char** argv*/)
{
	 //TGAImage image(400, 400, TGAImage::RGB);
	 //Line line(100.0f, 100.0f, 300.0f, 300.0f);
	 //line.Draw(image, white);
	
	TGAImage image(100, 100, TGAImage::RGB);
	Line line0(13, 20, 80, 40); line0.Draw(image, white); 
	Line line1(20, 13, 40, 80); line1.Draw(image, red); 
	Line line2(80, 40, 13, 20); line0.Draw(image, red);

	//image.set(52, 41, white);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}