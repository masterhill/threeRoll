
#include <windows.h>
#include <tchar.h>
#include <math.h>

#include "FreeImage.h"


#include "CELLMath.hpp"

#include "OpenGLWindow.h"
#include "Camera3rd.h"
#include "Camera.h"
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
	Camera _camera[3];
	int index;
	bool _bMouseDown;
	int2 _MouseCur;
	float3 _rolePos;
	float3 _moveTo;

public:
	SamplerTexture()
	{
		_bMouseDown = false;
		_rolePos = float3{ 0,0,0 };
		_moveTo = _rolePos;
		_camera[0]._eye = float3(0, 0, 10);
		_camera[0]._lookat = float3(0, 0, 0);

		_camera[1]._eye = float3(10, 0, 10);
		_camera[1]._lookat = float3(-10, 0, -10);

		_camera[2]._eye = float3(10, 10, 10);
		_camera[2]._lookat = float3(-10, 0, -10);
		index = 0;
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
		//glMatrixMode(GL_PROJECTION);
	
		
		//! ½«Í¶Ó°¾ÙÖ¤Çå¿Õ³Éµ¥Î»¾ØÕó
		//glLoadIdentity();

		//gluPerspective(60, double(_width) / double(_height), 0.1, 1000);

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

			{ -size, -1,    -size       ,0,  0 },
			{ -size, -1,    size        ,100,  0 },
			{ size, -1,     size        ,100,  100 },
			{ size, -1,     -size       ,0,  100 },
		};
	
		//! Çå³Éµ¥Î»¾ØÕó
		glMatrixMode(GL_MODELVIEW);
		_camera[index].perspective(60, double(_width) / double(_height), 0.1, 1000);
		_camera[index].update();
		matrix4 matVP = _camera[index].getMVP();
		glLoadMatrixf(matVP.data());
		//! ²úÉúÒ»¸ö¾ØÕó
		//glPushMatrix();
		float3 dir = normalize(_moveTo - _rolePos);
		if (length(_moveTo - _rolePos) < 1)
		{
			_moveTo = _rolePos;
		}
		else {
			_rolePos += dir*(10.0f) / 60.0f;
		}
		glTranslatef(_rolePos.x, _rolePos.y, _rolePos.z);

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
		//glPopMatrix();
		glLoadMatrixf(matVP.data());//重新加载投影矩阵和视口矩阵
		//复位坐标系，只针对后面的矩阵
		//glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, _ground);
		glDrawArrays(GL_QUADS,24, 4);
	}

	virtual LRESULT events(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
	{
		switch (msgId)
		{
		case WM_SIZE:
		{
			if (::IsWindow(hWnd))
			{
				RECT rect;
				GetClientRect(_hWnd, &rect);
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;
				//_camera[index].setViewSize(w, h);
				glViewport(0,0,w, h);
			}
			
		}
		break;
		case WM_RBUTTONDOWN:
		{
			index += 1;
			index %= 3;
		}
		break;
		case WM_LBUTTONDOWN:
		{
			_MouseCur = int2(LOWORD(lParam), HIWORD(lParam));
			_bMouseDown = true;
			Ray ray = _camera[index].createRayFromScreen(_MouseCur.x, _MouseCur.y);
			float3 orgin=ray.getOrigin();
			float tm = (orgin.y + 1) / ray.getDirection().y;//摄像机到地面y的距离:求出 ray.getDirection().y的分量速度，求出时间
			float3 c = orgin + abs(tm)*ray.getDirection();//求出屏幕上的点的位置
			_moveTo = c;

		}
		break;
		case WM_LBUTTONUP:
		{
			_bMouseDown = false;
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (_bMouseDown)
			{
				int2 _MouseOld= int2(LOWORD(lParam), HIWORD(lParam));
				int xoff = _MouseCur.x-_MouseOld.x;
				_camera[index].rotateY(xoff*0.5f);
				_MouseCur = _MouseOld;

			}
		}
		break;
		case WM_MOUSEWHEEL:
		{
			int detal = (short)HIWORD(wParam);
			if (detal > 0)
			{
			//	_camera[index].setRadius(_camera[index].getRadius()*1.2f);
			}
			else {
			//	_camera[index].setRadius(_camera[index].getRadius()*0.8f);
			}
		}
		break;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_LEFT:
				_rolePos.x -= 0.1f;
				break;
			case VK_RIGHT:
				_rolePos.x += 0.1f;
				break;
			case  VK_UP:
				_rolePos.z -= 0.1f;
				break;
			case VK_DOWN:
				_rolePos.z += 0.1f;
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