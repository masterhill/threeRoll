﻿
#include <windows.h>
#include <tchar.h>
#include <math.h>

#include "FreeImage.h"


#include "CELLMath.hpp"

#include "OpenGLWindow.h"
#include <gl/GLU.h>
#include <vector>
#include "FFMPEGVideoReader.h"
#include <map>
#include "PixelBuffer.h"
#include "FrameBufferObject.h"
struct Vertex
{
	float x, y, z;
	float u, v;
	float r, g, b;
};

class   FBOWindow :public OpenGLWindow
{
public:
	GLuint              _texture;
	GLuint              _pbo[2];
	int _DMA;
	int _READ;
	FBOWindow()
	{
		_DMA = 0;
		_READ = 1;
	}
public:
	unsigned    createTexture(int w, int h, const void* data, GLenum type)
	{
		unsigned    texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);


		return  texId;
	}
	/**
	*   使用FreeImage加载图片
	*/
	unsigned    createTextureFromImage(const char* fileName)
	{
		//1 获取图片格式
		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
		if (fifmt == FIF_UNKNOWN)
		{
			return  0;
		}
		//2 加载图片
		FIBITMAP    *dib = FreeImage_Load(fifmt, fileName, 0);

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);

		//! 获取数据指针
		FIBITMAP*   temp = dib;
		dib = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(temp);

		BYTE*   pixels = (BYTE*)FreeImage_GetBits(dib);
		int     width = FreeImage_GetWidth(dib);
		int     height = FreeImage_GetHeight(dib);

		for (int i = 0; i < width * height * 4; i += 4)
		{
			BYTE temp = pixels[i];
			pixels[i] = pixels[i + 2];
			pixels[i + 2] = temp;
		}

		unsigned    res = createTexture(width, height, pixels, GL_RGBA);
		FreeImage_Unload(dib);
		return      res;
	}
public:
	virtual void    onInitGL()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, double(_width) / double(_height), 0.1f, 100.0f);
		_texture = createTextureFromImage("floor.bmp");

		//双缓存
		glGenBuffers(2, _pbo);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER, _width * _height * 4, 0, GL_STATIC_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, _width * _height * 4, 0, GL_STATIC_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	}
	virtual void    onShutdownGL()
	{
	}

	void    renderCube(GLuint tex, bool rot)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, double(_width) / double(_height), 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5);

		static  float   angle = 0;
		if (rot)
		{
			glRotated(angle, 1, 1, 1);
			angle += 1;
		}

		glBindTexture(GL_TEXTURE_2D, tex);

		Vertex g_cubeVertices[] =
		{
			{ -1.0f,-1.0f, 1.0f,0.0f, 0.0f,1.0f, 0.0f, 0.0f },
			{ 1.0f,-1.0f, 1.0f,1.0f, 0.0f,1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f,1.0f, 1.0f,1.0f, 0.0f, 0.0f },
			{ -1.0f, 1.0f, 1.0f,0.0f, 1.0f,1.0f, 0.0f, 0.0f },

			{ -1.0f,-1.0f,-1.0f,1.0f, 0.0f,0.0f, 1.0f, 0.0f },
			{ -1.0f, 1.0f,-1.0f,1.0f, 1.0f,0.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f,-1.0f,0.0f, 1.0f,0.0f, 1.0f, 0.0f },
			{ 1.0f,-1.0f,-1.0f,0.0f, 0.0f,0.0f, 1.0f, 0.0f },

			{ -1.0f, 1.0f,-1.0f,0.0f, 1.0f,0.0f, 0.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f,0.0f, 0.0f,0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f,1.0f, 0.0f,0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f,-1.0f,1.0f, 1.0f,0.0f, 0.0f, 1.0f },

			{ -1.0f,-1.0f,-1.0f,1.0f, 1.0f,1.0f, 1.0f, 0.0f },
			{ 1.0f,-1.0f,-1.0f,0.0f, 1.0f,1.0f, 1.0f, 0.0f },
			{ 1.0f,-1.0f, 1.0f,0.0f, 0.0f,1.0f, 1.0f, 0.0f },
			{ -1.0f,-1.0f, 1.0f,1.0f, 0.0f,1.0f, 1.0f, 0.0f },

			{ 1.0f,-1.0f,-1.0f,1.0f, 0.0f,1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f,-1.0f,1.0f, 1.0f,1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f,0.0f, 1.0f,1.0f, 0.0f, 1.0f },
			{ 1.0f,-1.0f, 1.0f,0.0f, 0.0f,1.0f, 0.0f, 1.0f },

			{ -1.0f,-1.0f,-1.0f,0.0f, 0.0f,0.0f, 1.0f, 1.0f },
			{ -1.0f,-1.0f, 1.0f,1.0f, 0.0f,0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f,1.0f, 1.0f,0.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f,-1.0f,0.0f, 1.0f,0.0f, 1.0f, 1.0f }
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);


		/**
		* 这里大家可以慢慢体会
		*/
		float* addrVertex = (float*)g_cubeVertices;
		float* uvAddress = (float*)&g_cubeVertices[0].u;

		float* colorAddress = (float*)&g_cubeVertices[0].r;

		//--------------元素个数---元素类型---元素之间的内存偏移---数据地址
		//OpenGL根据元素之间的内存偏移来计算下一个元素的位置。
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), addrVertex);
		glColorPointer(3, GL_FLOAT, sizeof(Vertex), colorAddress);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), uvAddress);

		glDrawArrays(GL_QUADS, 0, 24);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	bool    save(int w, int h, char* data, size_t length)
	{

		FIBITMAP*   bitmap = FreeImage_Allocate(w, h, 32, 0, 0, 0);

		BYTE*       pixels = (BYTE*)FreeImage_GetBits(bitmap);

		memcpy(pixels, data, w * h * 4);
		bool    bSuccess = FreeImage_Save(FIF_PNG, bitmap, "e:\\xx.png", PNG_DEFAULT);
		FreeImage_Unload(bitmap);
		return  bSuccess;
	}

	virtual void render(void)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		renderCube(_texture, true);

		//! DMA
		glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo[_DMA]);
		glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo[_READ]);
		void*   data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (data)
		{
			save(_width, _height, (char*)data, _width * _height * 4);
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		std::swap(_DMA, _READ);
	}
};


int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{

	FBOWindow   context;

	context.main(640, 480);


	return  0;
}