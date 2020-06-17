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

struct Vertex
{
	float x, y, z;
	float u, v;
	float r, g, b;
};

class   VertexBufferObject :public OpenGLWindow
{
public:
	GLuint              _texture;
	GLuint              _dynamic;
	PixelBuffer			_pBuffer;
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
		_pBuffer.setup(_hWnd, _hDC, _hRC, 453, 605);
		glEnable(GL_TEXTURE_2D);
		_dynamic = createTexture(_pBuffer._width, _pBuffer._height, 0, GL_RGBA);
	
	}
	virtual void    onShutdownGL()
	{
	}

	void    renderCube(GLuint tex)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, double(_width) / double(_height), 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5);

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
	virtual void render(void)
	{
		_pBuffer.makeCurrent();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube(_texture);
		//使用pbufer数据绘制绑定纹理
		glBindTexture(GL_TEXTURE_2D, _dynamic);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _pBuffer._width, _pBuffer._height);
		makeCurrent();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderCube(_dynamic);

	}
	void    makeCurrent()
	{
		wglMakeCurrent(_hDC, _hRC);
	}
};


int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{

	VertexBufferObject   context;

	context.main(640, 480);


	return  0;
}