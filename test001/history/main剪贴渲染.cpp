
#include <windows.h>
#include <tchar.h>
#include <math.h>

#include "FreeImage.h"


#include "CELLMath.hpp"

#include "OpenGLWindow.h"
#include "Camera3rd.h"
#include <vector>

using namespace CELL;

struct Vertex
{
	float x, y, z;
	float u, v;
};
class Randerable
{
public:
	void rander() {
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
		};
	
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].u);


		glDrawArrays(GL_QUADS, 0, 24);

	
	}
};
class Node
{
public:
	float3 _trans;
	float3 _scale;
	float3 _rot;
	Randerable* randerable;
	void rander( matrix4 vp) {
		matrix4 mat(1);
		mat.translate(_trans);
		matrix4 mvp = vp*mat;
		glLoadMatrixf(mvp.data());
		randerable->rander();
	}
};
typedef std::vector<Node> ArrayNode;
class   SamplerTexture :public OpenGLWindow
{
	GLuint  _texture;

	GLuint  _texGround;

	Camera3rd  _camera;
	int2    _mouseDown;
	bool    _bDownFlag;


	float3  _rolePos;
	float3  _moveTo;
	ArrayNode _arrayNodes;
	Randerable _rander;
	Frustum _frush;
public:
	SamplerTexture()
	{
		_bDownFlag = false;
		_rolePos = float3(0, 0, 0);
		_moveTo = _rolePos;
		for (int i = 0; i < 100; i++)
		{
			Node tmp;
			tmp._trans.x = rand() % 200 - 100;
			tmp._trans.y= rand() % 10 + 10;
			tmp._trans.z = rand() % 200 - 100;
			tmp.randerable = &_rander;
			_arrayNodes.push_back(tmp);
		}
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
		_texture = createTextureFromImage("2.jpg");
		_texGround = createTextureFromImage("1.jpg");

	}
	virtual void    render()
	{
#define M_PI (3.14159265358979323846)


		float   size = 100;

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

			//! µØÃæÊý¾Ý

			{ -size, -1,    -size       ,0,  0 },
			{ -size, -1,    size        ,100,  0 },
			{ size,  -1,     size        ,100,  100 },
			{ size,  -1,     -size       ,0,  100 },
		};

		//! Ö¸¶¨ÒÔÏÂµÄ²Ù×÷Õë¶ÔÍ¶Ó°¾ØÕó
		glMatrixMode(GL_MODELVIEW);

		_camera.perspective(60, float(_width) / float(_height), 0.1f, 1000.0f);

		float3  dir = normalize(_moveTo - _rolePos);

		if (length(_moveTo - _rolePos) < 1)
		{
			_rolePos = _moveTo;
		}
		else
		{
			_rolePos += dir * (10.0f / 60.0f);
		}

		_camera.updateLookat(_rolePos);
		//! ÉãÏñ»ú¸üÐÂ
		_camera.update();

		matrix4 matVP = _camera.getMVP();
		matrix4 tmp = matVP.transpose();//转置
		_frush.loadFrustum(tmp);//梯形
		glLoadMatrixf(matVP.data());

		//! ²úÉúÒ»¸ö¾ØÕó
		glTranslatef(_rolePos.x, _rolePos.y, _rolePos.z);

		glBindTexture(GL_TEXTURE_2D, _texture);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].x);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &cubeVertices[0].u);


		glDrawArrays(GL_QUADS, 0, 24);
		
		glLoadMatrixf(matVP.data());

		glBindTexture(GL_TEXTURE_2D, _texGround);
		glDrawArrays(GL_QUADS, 24, 4);
		glBindTexture(GL_TEXTURE_2D, _texture);
		int count = 0;
		for (int i = 0; i < _arrayNodes.size(); i++)
		{
			if (_frush.sphereInFrustum(_arrayNodes[i]._trans, 1.424))
			{
				++count;
				_arrayNodes[i].rander(matVP);
			}
		
		}
		char buf[128];
		sprintf(buf, "%d", count);
		SetWindowTextA(_hWnd, buf);
	}

	virtual LRESULT events(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
	{
		switch (msgId)
		{
		case WM_SIZE:
		{
			if (::IsWindow(hWnd))
			{
				RECT    rt;
				GetClientRect(_hWnd, &rt);
				int     w = rt.right - rt.left;
				int     h = rt.bottom - rt.top;
				glViewport(0, 0, w, h);
				_camera.setView(w, h);
			}
		}
		break;
		case WM_LBUTTONDOWN:
		{
			_bDownFlag = true;
			_mouseDown = int2(LOWORD(lParam), HIWORD(lParam));

			CELL::Ray   ray = _camera.createRayFromScreen(_mouseDown.x, _mouseDown.y);
			float3      orth = ray.getOrigin();
			float       tm = (orth.y + 1) / ray.getDirection().y;
			float3      c = ray.getOrigin() + abs(tm) * ray.getDirection();
			_moveTo = c;
		}
		break;
		case WM_LBUTTONUP:
		{
			_bDownFlag = false;
		}
		break;

		case WM_MOUSEWHEEL:
		{
			int delta = (short)HIWORD(wParam);
			if (delta > 0)
			{
				_camera.setRadius(_camera.getRadius() * 1.2f);
			}
			else
			{
				_camera.setRadius(_camera.getRadius() * 0.8f);
			}
		}
		break;

		case WM_MOUSEMOVE:
		{
			if (_bDownFlag)
			{
				int2    mouseCur = int2(LOWORD(lParam), HIWORD(lParam));
				float   xOffset = mouseCur.x - _mouseDown.x;
				_mouseDown = mouseCur;
				_camera.rotateY(xOffset * 0.5f);
			}
		}
		break;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_LEFT:
				_rolePos.x -= 0.1;
				break;
			case VK_RIGHT:
				_rolePos.x += 0.1;
				break;
			case  VK_UP:
				_rolePos.z -= 0.1;
				break;
			case VK_DOWN:
				_rolePos.z += 0.1;
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