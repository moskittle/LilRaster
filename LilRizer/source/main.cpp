#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


#include "tgaimage.h"
#include "model.h"

#include "Timer.h"

#define DRAW_IMAGE
//#define DRAW_SCENE

//#define DRAW_POLYGON_MODEL
//#define DRAW_TRIANGLE
//#define DRAW_CLOWN_FACE
#define DRAW_BACKFACE_CULLING


#define BARYCENTRIC_METHOD
//#define TRADITIONAL_METHOD


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 800, height = 800;
Timer timer;

// cross product
Vec3f cross(Vec3f v1, Vec3f v2)
{
	return Vec3f(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v2.z * v1.x, v1.x * v2.y - v2.x * v1.y);
}

struct Point
{
	Point() = default;
	Point(int _x, int _y) : x(_x), y(_y), z() {}
	Point(int _x, int _y, float _z) : x(_x), y(_y), z(_z) {}

	inline Point operator+(const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
	inline Point operator-(const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
	inline Point operator*(float t) { return Point(x * t, y * t); }

	int x = 0, y = 0;
	float z = 0.0f;
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

	Vec3f GetBaricentricCoord(Point P)
	{
		Vec3f u = cross(Vec3f(P2.x - P0.x, P1.x - P0.x, P0.x - P.x), Vec3f(P2.y - P0.y, P1.y - P0.y, P0.y - P.y));
		
		// abs(u.z) < 1 means this is a degenerate triangle
		if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
		return Vec3f(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}


#ifdef BARYCENTRIC_METHOD
	// Barycentric and bounding box method
	void Draw(float* zBuffer, TGAImage& image, TGAColor color)
	{
		Point bboxMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point bboxMax(0, 0);
		Point clamp(image.get_width() - 1, image.get_height() - 1);

		// Get bbox boundaries
		bboxMax.x = std::min(clamp.x, std::max(std::max(P0.x, P1.x), P2.x));
		bboxMax.y = std::min(clamp.y, std::max(std::max(P0.y, P1.y), P2.y));
		bboxMin.x = std::max(0, std::min(std::min(P0.x, P1.x), P2.x));
		bboxMin.y = std::max(0, std::min(std::min(P0.y, P1.y), P2.y));

		Point P;
		int index;
		for (P.x = bboxMin.x; P.x <= bboxMax.x; ++P.x)
		{
			for (P.y = bboxMin.y; P.y <= bboxMax.y; ++P.y)
			{
				Vec3f bc_coord = GetBaricentricCoord(P);
				if (bc_coord.x < 0 || bc_coord.y < 0 || bc_coord.z < 0) { continue; }
				
				// zBuffer
				P.z = 0;
				P.z = P0.z * bc_coord.x + P1.z * bc_coord.y + P2.z * bc_coord.z;
				index = static_cast<int>(P.x + P.y * width);
				if (P.z > zBuffer[index])
				{
					zBuffer[index] = P.z;
					image.set(P.x, P.y, color);
				}

			}
		}
	}
#endif //BARYCENTRIC_METHOD

#ifdef TRADITIONAL_METHOD
	// Traditional method
	void Draw(TGAImage& image, TGAColor color)
	{
		// Degenerated triangles
		if (P0.y == P1.y && P0.y == P2.y) return;

		// Sort vertices
		if (P0.y > P1.y) { std::swap(P0, P1); }
		if (P0.y > P2.y) { std::swap(P0, P2); }
		if (P1.y > P2.y) { std::swap(P1, P2); }

		int totalHeight = P2.y - P0.y;
		for (int y = P0.y; y < P2.y; ++y)
		{
			bool bIsFirstHalf = (y < P1.y) || (P1.y == P0.y);
			int segementHeight = bIsFirstHalf ? (P1.y - P0.y + 1) : (P2.y - P1.y + 1);

			float alpha = static_cast<float>(y - P0.y) / totalHeight;
			float beta = bIsFirstHalf ? (static_cast<float>(y - P0.y) / segementHeight) : (static_cast<float>(y - P1.y) / segementHeight);
			Point A = P0 + (P2 - P0) * alpha;
			Point B = bIsFirstHalf ? (P0 + (P1 - P0) * beta) : (P1 + (P2 - P1) * beta);

			if (A.x > B.x) { std::swap(A, B); }
			for (int x = A.x; x < B.x; ++x) 
			{ 
				image.set(x, y, color);
				std::cout <<x << ", " << y << ": " << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a << std::endl;
			}
			
		}
	}
#endif TRADITIONAL_METHOD

	Point P0, P1, P2;
};

std::ostream& operator<<(std::ostream& s, TGAColor& color) {
	s << "(" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ", " << (int)color.a << ")\n";
	return s;
}

void rasterize_1D(Point P0, Point P1, TGAImage& image, TGAColor color, int yBuffer[])
{
	if (P0.x > P1.x) { std::swap(P0, P1); }

	for (int x = P0.x, y = 0; x <= P1.x; ++x)
	{
		float t = (x - P0.x) / static_cast<float>(P1.x - P0.x);
		y = (1.0 - t) * P0.y + t * P1.y ;

		// Check if the value is closed to camera
		if (yBuffer[x] < y)
		{
			yBuffer[x] = y;
			image.set(x, 0, color);
		}
	}
}

int main(/*int argc, char** argv*/)
{
	Timer timer;

#ifdef DRAW_IMAGE

	TGAImage image(width, height, TGAImage::RGB);


	std::string filePath = "E:/dev/LilRizer/LilRizer/obj/";
	Model* model = new Model(filePath.append("african_head.obj").c_str());

#ifdef DRAW_TRIANGLE
	Point p0[3] = { Point(10, 70),   Point(50, 160),  Point(70, 80) };
	Point p1[3] = { Point(180, 50),  Point(150, 1),   Point(70, 180) };
	Point p2[3] = { Point(180, 150), Point(120, 160), Point(130, 180) };
	Triangle t0(p0[0], p0[1], p0[2]); t0.Draw(image, red);
	Triangle t1(p1[0], p1[1], p1[2]); t1.Draw(image, white);
	Triangle t2(p2[0], p2[1], p2[2]); t2.Draw(image, green);
#endif // DRAW_TRIANGLE

#ifdef DRAW_CLOWN_FACE
	for (int i = 0; i < model->nfaces(); ++i)
	{
		std::vector<int> face = model->face(i);
		Point screen_coords[3];
		for (int j = 0; j < 3; ++j)
		{
			Vec3f world_coords = model->vert(face[j]);
			screen_coords[j] = Point((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
		}
		Triangle T(screen_coords[0], screen_coords[1], screen_coords[2]); T.Draw(image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}
#endif // DRAW_CLOWN_FACE

#ifdef DRAW_BACKFACE_CULLING
	float* zBuffer = new float[width * height];
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			zBuffer[i * width + j] = std::numeric_limits<int>::min();

	Vec3f light_dir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Point screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++) 
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Point((v.x + 1.) * width / 2., (v.y + 1.) * height / 2., v.z);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0) {
			Triangle T(screen_coords[0], screen_coords[1], screen_coords[2]); T.Draw(zBuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}
#endif // DRAW_BACKFACE_CULLING

#ifdef DRAW_POLYGON_MODEL

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

#endif // DRAW_POLYGON_MODEL

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	
#endif // DRAW_IMAGE

#ifdef DRAW_SCENE
	
	//TGAImage scene(width, height, TGAImage::RGB);

	//// scene "2d mesh"
	//Line L0(Point(20, 34), Point(744, 400)); L0.Draw(scene, red);
	//Line L1(Point(120, 434), Point(444, 400)); L1.Draw(scene, green);
	//Line L2(Point(330, 463), Point(594, 200)); L2.Draw(scene, blue);

	//scene.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//scene.write_tga_file("scene.tga");

	//// screen line
	//Line screen_line(Point(10, 10), Point(790, 10)); screen_line.Draw(scene, white);

	TGAImage render(width, 1, TGAImage::RGB);
	int yBuffer[width];
	for (int i = 0; i < width; ++i) { yBuffer[i] = std::numeric_limits<int>::min(); }
	
	rasterize_1D(Point(20, 34), Point(744, 400), render, red, yBuffer);
	rasterize_1D(Point(120, 434), Point(444, 400), render, green, yBuffer);
	rasterize_1D(Point(330, 463), Point(594, 200), render, blue, yBuffer);

	render.flip_vertically();
	render.write_tga_file("render.tga");


#endif // DRAW_SCENE

	
	return 0;
}