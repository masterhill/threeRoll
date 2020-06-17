
#include <windows.h>
#include <tchar.h>
#include <math.h>

#include "FreeImage.h"


#include "CELLMath.hpp"

#include "OpenGLWindow.h"
#include <vector>
using namespace CELL;

struct Vertex
{
	float x, y, z;
	float u, v;
};
class   SamplerTexture :public OpenGLWindow
{
	GLuint  _texture;
	GLuint  _ground;
	float3 _eye;
	float3 _center;

public:
	SamplerTexture()
	{
		_eye = float3{ 0,0,10 };
		_center = float3{ 0,0,0 };
	}
	/**
	*   Ê¹ÓÃFreeImage¼ÓÔØÍ¼Æ¬
	*/
	unsigned    createTextureFromImage(const char* fileName)
	{
		//1 »ñÈ¡Í¼Æ¬¸ñÊ½
		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
		if (fifmt == FIF_UNKNOWN)
		{
			return  0;
		}
		//2 ¼ÓÔØÍ¼Æ¬
		FIBITMAP    *dib = FreeImage_Load(fifmt, fileName, 0);

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);

		//! »ñÈ¡Êý¾ÝÖ¸Õë
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

		unsigned    res = createTexture(width, height, pixels);
		FreeImage_Unload(dib);
		return      res;
	}

	unsigned    createTexture(int w, int h, const void* data)
	{
		unsigned    texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return  texId;
	}

	virtual void    onInitGL()
	{
		_texture = createTextureFromImage("1.jpg");
		_ground = createTextureFromImage("3.jpg");

	}
	virtual void    render()
	{
#define M_PI (3.14159265358979323846)
		//! Ö¸¶¨ÒÔÏÂµÄ²Ù×÷Õë¶ÔÍ¶Ó°¾ØÕó
		glMatrixMode(GL_PROJECTION);
		//! ½«Í¶Ó°¾ÙÖ¤Çå¿Õ³Éµ¥Î»¾ØÕó
		glLoadIdentity();

		gluPerspective(60, double(_width) / double(_height), 0.1, 1000);

		int size = 100;
		Vertex cubeVertices[] =
		{
			{ -1.0f,-1.0f, 1.0f    ,0.3f,  0.3f },
			{ 1.0f,-1.0f, 1.0f     ,0.5f,  0.3f },
			{ 1.0f, 1.0f, 1.0f     ,0.5f,  0.8f },
			{ -1.0f, 1.0f, 1.0f     ,0.3f,  0.8f },

			{ -1.0f,-1.0f,-1.0f    ,0,  0 },
			{ -1.0f, 1.0f,-1.0f    ,1,  0 },
			{ 1.0f, 1.0f,-1.0f     ,1,  1 },
			{ 1.0f,-1.0f,-1.0f     ,0,  1 },

			{ -1.0f, 1.0f,-1.0f    ,0,  0 },
			{ -1.0f, 1.0f, 1.0f    ,1,  0 },
			{ 1.0f, 1.0f, 1.0f     ,1,  1 },
			{ 1.0f, 1.0f,-1.0f     ,0,  1 },

			{ -1.0f,-1.0f,-1.0f    ,0,  0 },
			{ 1.0f,-1.0f,-1.0f     ,1,  0 },
			{ 1.0f,-1.0f, 1.0f     ,1,  1 },
			{ -1.0f,-1.0f, 1.0f    ,0,  1 },

			{ 1.0f,-1.0f,-1.0f ,0,  0 },
			{ 1.0f, 1.0f,-1.0f ,1,  0 },
			{ 1.0f, 1.0f, 1.0f ,1,  1 },
			{ 1.0f,-1.0f, 1.0f ,0,  1 },

			{ -1.0f,-1.0f,-1.0f ,0,  0 },
			{ -1.0f,-1.0f, 1.0f ,1,  0 },
			{ -1.0f, 1.0f, 1.0f ,1,  1 },
			{ -1.0f, 1.0f,-1.0f ,0,  1 },

			{ -size,-1,-size ,0,  0 },
			{ -size,-1, size ,100,  0 },
			{ size, -1, size ,100,  100 },
			{ size, 0,-size ,0,  100 },
		};
		glMatrixMode(GL_MODELVIEW);
		//! Çå³Éµ¥Î»¾ØÕó
		glLoadIdentity();

		static  float angle = 0;
		int r = 10;
		_eye.x = cos(angle*PI / 180)*r;
		_eye.z = sin(angle*PI / 180)*r;
		gluLookAt(_eye.x, _eye.y, _eye.z, _center.x, _center.y, _center.z,0,1,0);
		angle += 1;
		//! ²úÉúÒ»¸ö¾ØÕó
		glTranslatef(0, 0, 0);
	/*	static  float angle = 0;
		glRotatef(angle, 1, 1, 1);
		angle += 1;*/


		glBindTexture(GL_TEXTURE_2D, _texture);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].u);
		glDrawArrays(GL_QUADS, 0, 24);
		//复位坐标系，只针对后面的矩阵
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, _ground);
		glDrawArrays(GL_QUADS,24, 4);
	}

	virtual LRESULT events(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
	{
		switch (msgId)
		{
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_LEFT:
				_eye.x -= 0.1f;
				break;
			case VK_RIGHT:
				_eye.x += 0.1f;
				break;
			case  VK_UP:
				_eye.z -= 0.1f;
				break;
			case VK_DOWN:
				_eye.z += 0.1f;
				break;
			}
		}
		break;
		}
		return  __super::events(hWnd, msgId, wParam, lParam);
	}
};

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{

	SamplerTexture    instance;
	instance.main(800, 600);


	return  0;
}