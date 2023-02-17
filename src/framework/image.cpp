#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixel( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size)
		return false;

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
		return false;

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixel(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		perror("Failed to open file: ");
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	return true;
}

void Image::DrawRect(int x, int y, int w, int h, const Color& c)
{
	for (int i = 0; i < w; ++i) {
		SetPixel(x + i, y, c);
		SetPixel(x + i, y + h, c);
	}

	for (int j = 0; j < h; ++j) {
		SetPixel(x, y + j, c);
		SetPixel(x + w, y + j, c);
	}
}

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//A PARTIR D'AQUI COMENÇA EL CODI ESCRIT PER L'ALUMNE

//Dibuixa una línia utilitzant l'algoritme DDA
void Image::DrawLineDDA(int x0, int y0, int x, int y, const Color& c)

{
	int dx = x - x0;
	int dy = y - y0;
	int d = std::max(abs(dx), abs(dy));

	Vector2* v = new Vector2(float(dx) / float(d), float(dy) / float(d));
	Vector2* pos = new Vector2(float(x0), float(y0));

	for (int i = 0; i <= d; i++)
	{
		SetPixelSafe(floor(pos->x), floor(pos->y), c);
		pos->operator+=(*v);
	}
}

//Dibuixa una línia utilitzant l'algoritme Bresenham
void Image::DrawLineBresenham(int x0, int y0, int x1, int y1, const Color& c)

{
	int dx, dy, inc_E, inc_NE, d, x, y;
	if (abs(x1 - x0) >= abs(y1 - y0))
	{								//Aquí els octans 1, 4, 5 i 8
		if (x0 > x1)
		{					//Per als octans 4 i 5
			int aux_x = x0;
			x0 = x1;
			x1 = aux_x;
			int aux_y = y0;
			y0 = y1;
			y1 = aux_y;
		}

		dx = x1 - x0;
		dy = y1 - y0;
		inc_E = 2 * dy;
		if (dy < 0)
		{							//En cas dels octans 8 i 4
			d = 2 * dy + dx;
			inc_NE = 2 * (dy + dx);
		}
		else
		{							//En cas dels octans 1 i 5
			d = 2 * dy - dx;
			inc_NE = 2 * (dy - dx);
		}
		x = x0;
		y = y0;
		SetPixelSafe(x, y, c);
		while (x < x1)
		{
			if (d <= 0)
			{					//Si el punt està per sota la meitat
				x = x + 1;
				if (dy < 0)
				{
					y = y - 1;		//En cas dels octans 8 i 5
					d = d + inc_NE;
				}
				else
				{
					d = d + inc_E;	//En cas dels octans 1 i 4
				}
			}
			else
			{					//Si el punt està per sobre la meitat
				x = x + 1;
				if (dy < 0)
				{					//En cas dels octans 8 i 5
					d = d + inc_E;
				}
				else
				{					//En cas dels octans 1 i 4
					y = y + 1;
					d = d + inc_NE;
				}

			}
			SetPixelSafe(x, y, c);
		}
	}
	else
	{					//Aquí els octans 2, 3, 6 i 7
		if (y0 > y1)
		{						//Per els octans 6 i 7
			int aux_y = y0;
			y0 = y1;
			y1 = aux_y;
			int aux_x = x0;
			x0 = x1;
			x1 = aux_x;
		}

		dx = x1 - x0;
		dy = y1 - y0;
		inc_E = 2 * (-dx);
		if (dx < 0)
		{					//Pels octans 3 i 6									
			d = -2 * dx - dy;
			inc_NE = 2 * (-dx - dy);
		}
		else
		{				//Pels octans 2 i 7
			d = dy - 2 * dx;
			inc_NE = 2 * (dy - dx);
		}
		x = x0;
		y = y0;
		SetPixelSafe(x, y, c);
		while (y < y1)
		{
			if (d <= 0)
			{					//Si el punt està per sota la meitat
				y = y + 1;
				if (dx < 0)
				{					//En cas dels octans 3 i 6
					d = d + inc_E;
				}
				else
				{					//En cas dels octans 2 i 7
					x = x + 1;
					d = d + inc_NE;
				}
			}
			else
			{					//Si el punt està per sobre la meitat
				y = y + 1;
				if (dx < 0)
				{					//En cas dels octans 3 i 6
					x = x - 1;
					d = d + inc_NE;
				}
				else
				{					//En cas dels octans 2 i 7
					d = d + inc_E;
				}
			}
			SetPixelSafe(x, y, c);

		}
	}
}

//Dibuixar un cercle amb l'algoritme Bresenham
void Image::DrawCircle(int x0, int y0, int r, const Color& c, bool fill)
{
	int x, x2, x3, y, y2, y3; int v;
	x = x0;
	x2 = x0 + r;
	x3 = x0 - r;	//Simetrica a x2 centrada en la recta vertical que passa per r
	y = y0 + r;
	y2 = y0;
	y3 = y0 - r;	//Simetrica a y centrada en la recta y = r
	v = 1 - r;

	int n = 0;
	SetPixelSafe(x2, y2, c);				//1r octau
	SetPixelSafe(x, y, c);					//2n octau
	SetPixelSafe(x - (2 * n), y, c);		//3r octau --> 2*n pq hem d'arribar a 0 i després arribar a la posició simètrica en negatiu
	SetPixelSafe(x3, y2, c);				//4t octau
	SetPixelSafe(x3, y2 - (2 * n), c);		//5è octau
	SetPixelSafe(x, y3, c);					//6è octau
	SetPixelSafe(x - (2 * n), y3, c);			//7è octau
	SetPixelSafe(x2, y2 - (2 * n), c);		//8è octau
	if (fill == true)
	{									//Emplenar el cercle
		int aux_x = x - (2 * n);
		int aux_x3 = x3;
		while (aux_x < x)
		{								//Emplenar els octaus 2, 3, 6 i 7
			SetPixelSafe(aux_x, y, c);
			SetPixelSafe(aux_x, y3, c);
			aux_x++;
		}
		while (aux_x3 < x2)
		{								//Emplenar els octaus 1, 4, 5 i 8
			SetPixelSafe(aux_x3, y2, c);
			SetPixelSafe(aux_x3, y2 - (2 * n), c);
			aux_x3++;
		}
	}

	while ((y - y0) > (x - x0))
	{
		if (v < 0)
		{					//Si el píxel ha de ser per sobre
			v = v + 2 * (x - x0) + 3;
			x++;
			y2++;
		}
		else
		{					//Si el píxel ha de ser per sota
			v = v + 2 * ((x - x0) - (y - y0)) + 5;
			x++;
			x2--;
			x3++;
			y--;
			y2++;
			y3++;
		}
		n++;
		SetPixelSafe(x2, y2, c);				//1r octau
		SetPixelSafe(x, y, c);					//2n octau
		SetPixelSafe(x - (2 * n), y, c);		//3r octau
		SetPixelSafe(x3, y2, c);				//4t octau
		SetPixelSafe(x3, y2 - (2 * n), c);		//5è octau
		SetPixelSafe(x, y3, c);					//6è octau
		SetPixelSafe(x - (2 * n), y3, c);		//7è octau
		SetPixelSafe(x2, y2 - (2 * n), c);		//8è octau

		if (fill == true)
		{									//Emplenar el cercle
			int aux_x = x - (2 * n);
			int aux_x3 = x3;
			while (aux_x < x)
			{								//Emplenar els octaus 2, 3, 6 i 7
				SetPixelSafe(aux_x, y, c);
				SetPixelSafe(aux_x, y3, c);
				aux_x++;
			}
			while (aux_x3 < x2)
			{								//Emplenar els octaus 1, 4, 5 i 8
				SetPixelSafe(aux_x3, y2, c);
				SetPixelSafe(aux_x3, y2 - (2 * n), c);
				aux_x3++;
			}
		}

	}
}


//Dibuixar una Image pixel per pixel
void Image::DrawImagePixels(const Image& image, int x, int y, bool top)
{
	if (top == false)
	{
		for (int h = y; h < image.height + y; h++)
		{
			for (int w = x; w < image.width + x; w++)
			{
				SetPixelSafe(w, h, image.GetPixel(w - x, h - y));
			}
		}
	}
	else
	{
		for (int h = this->height - y; h > this->height - image.height - y; h--)
		{
			for (int w = x; w < image.width; w++)
			{
				SetPixelSafe(w, h, image.GetPixel(w - x, (h - this->height + y) * -1));		//No podem utilitzar abs perque this->height és unsigned int
			}
		}
	}
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& color)
{
	std::vector<Cell> AET;
	AET.resize(this->height);

	for (int i = 0; i < AET.size(); i++)
	{
		AET[i].max_X = -1;
		AET[i].min_X = this->width + 20;
	}
	ScanLineBresenham(p0.x, p0.y, p1.x, p1.y, AET);
	ScanLineBresenham(p0.x, p0.y, p2.x, p2.y, AET);
	ScanLineBresenham(p1.x, p1.y, p2.x, p2.y, AET);
	for (int j = 0; j < AET.size(); j++)
	{
		if (AET[j].max_X > AET[j].min_X)
		{
			this->DrawHorizontal(AET[j].min_X, AET[j].max_X, j, color);
		}
	}
}

void Image::DrawHorizontal(int x0, int x1, int y, Color c)
{
	for (int i = x0; i <= x1; i++)
	{
		SetPixelSafe(i, y, c);
	}
}

void Image::ScanLineBresenham(int x0, int y0, int x1, int y1, std::vector<Cell>& AET)
{
	int dx, dy, inc_E, inc_NE, d, x, y;
	if (abs(x1 - x0) >= abs(y1 - y0))
	{								//Aquí els octans 1, 4, 5 i 8
		if (x0 > x1)
		{					//Per als octans 4 i 5
			int aux_x = x0;
			x0 = x1;
			x1 = aux_x;
			int aux_y = y0;
			y0 = y1;
			y1 = aux_y;
		}

		dx = x1 - x0;
		dy = y1 - y0;
		inc_E = 2 * dy;
		if (dy < 0)
		{							//En cas dels octans 8 i 4
			d = 2 * dy + dx;
			inc_NE = 2 * (dy + dx);
		}
		else
		{							//En cas dels octans 1 i 5
			d = 2 * dy - dx;
			inc_NE = 2 * (dy - dx);
		}
		x = x0;
		y = y0;
		if (y < this->height)
		{
			AET[y].max_X = std::max(AET[y].max_X, x);
			AET[y].min_X = std::min(AET[y].min_X, x);

		}
		while (x < x1)
		{
			if (d <= 0)
			{					//Si el punt està per sota la meitat
				x = x + 1;
				if (dy < 0)
				{
					y = y - 1;		//En cas dels octans 8 i 5
					d = d + inc_NE;
				}
				else
				{
					d = d + inc_E;	//En cas dels octans 1 i 4
				}
			}
			else
			{					//Si el punt està per sobre la meitat
				x = x + 1;
				if (dy < 0)
				{					//En cas dels octans 8 i 5
					d = d + inc_E;
				}
				else
				{					//En cas dels octans 1 i 4
					y = y + 1;
					d = d + inc_NE;
				}

			}
			if (y < this->height)
			{
				AET[y].max_X = std::max(AET[y].max_X, x);
				AET[y].min_X = std::min(AET[y].min_X, x);

			}
		}
	}
	else
	{					//Aquí els octans 2, 3, 6 i 7
		if (y0 > y1)
		{						//Per els octans 6 i 7
			int aux_y = y0;
			y0 = y1;
			y1 = aux_y;
			int aux_x = x0;
			x0 = x1;
			x1 = aux_x;
		}

		dx = x1 - x0;
		dy = y1 - y0;
		inc_E = 2 * (-dx);
		if (dx < 0)
		{					//Pels octans 3 i 6									
			d = -2 * dx - dy;
			inc_NE = 2 * (-dx - dy);
		}
		else
		{				//Pels octans 2 i 7
			d = dy - 2 * dx;
			inc_NE = 2 * (dy - dx);
		}
		x = x0;
		y = y0;
		if (y < this->height)
		{
			AET[y].max_X = std::max(AET[y].max_X, x);
			AET[y].min_X = std::min(AET[y].min_X, x);

		}
		while (y < y1)
		{
			if (d <= 0)
			{					//Si el punt està per sota la meitat
				y = y + 1;
				if (dx < 0)
				{					//En cas dels octans 3 i 6
					d = d + inc_E;
				}
				else
				{					//En cas dels octans 2 i 7
					x = x + 1;
					d = d + inc_NE;
				}
			}
			else
			{					//Si el punt està per sobre la meitat
				y = y + 1;
				if (dx < 0)
				{					//En cas dels octans 3 i 6
					x = x - 1;
					d = d + inc_NE;
				}
				else
				{					//En cas dels octans 2 i 7
					d = d + inc_E;
				}
			}
			if (y < this->height)
			{
				AET[y].max_X = std::max(AET[y].max_X, x);
				AET[y].min_X = std::min(AET[y].min_X, x);

			}

		}
	}
}

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2)
{
	std::vector<Cell> AET;
	AET.resize(this->height);

	for (int i = 0; i < AET.size(); i++)
	{
		AET[i].max_X = -1;
		AET[i].min_X = this->width + 20;
	}
	ScanLineBresenham(p0.x, p0.y, p1.x, p1.y, AET);
	ScanLineBresenham(p0.x, p0.y, p2.x, p2.y, AET);
	ScanLineBresenham(p1.x, p1.y, p2.x, p2.y, AET);

	Vector2 v0 = Vector2(p1.x - p0.x, p1.y - p0.y);
	Vector2 v1 = Vector2(p2.x - p0.x, p2.y - p0.y);
	for (int j = 0; j < AET.size(); j++)
	{
		if (AET[j].max_X > AET[j].min_X)
		{
			this->DrawHorizontalInterpolated(AET[j].min_X, AET[j].max_X, j, p0, v0, v1, c0, c1, c2);
		}
	}
}

void Image::DrawHorizontalInterpolated(int x0, int x1, int y, Vector3 p0, Vector2 v0, Vector2 v1, const Color& c0, const Color& c1, const Color& c2)
{
	for (int i = x0; i <= x1; i++)
	{
		Vector2 v2 = Vector2(i - p0.x, y - p0.y);

		float d00 = v0.Dot(v0);
		float d01 = v0.Dot(v1);
		float d11 = v1.Dot(v1);
		float d20 = v2.Dot(v0);
		float d21 = v2.Dot(v1);
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0 - v - w;
		Vector3 weights = Vector3(u, v, w);
		float sum = v + u + w;
		weights.Clamp(0,1);
		u = weights.x / sum;
		v = weights.y / sum;
		w = weights.z / sum;
		Color c = c0 * u + c1 * v + c2 * w;

		SetPixelSafe(i, y, c);
	}
}