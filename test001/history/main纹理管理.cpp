
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

struct Vertex
{
	float   x, y, z;
	float   u, v;
};
//class TextureManager :public std::map<std::string,GLuint>{
//public:
//	GLuint getTexture(const char* fileName)
//	{
//		iterator iter = find(fileName);
//		if (iter != end())
//		{
//			return iter->second;
//		}
//		else {
//			GLuint res= createTextureFromImage(fileName);
//			if (res != 0)
//			{
//				(*this)[fileName] = res;
//				return res;
//			}
//		}
//		
//	}
//	void releaseTexture(GLuint texId) {
//		for (iterator iter = begin(); iter != end(); iter++)
//		{
//			if (iter->second == texId)
//			{
//				glDeleteTextures(1, &iter->second);
//				erase(iter);
//				break;
//			}
//		}
//	}
//	unsigned    createTexture(int w, int h, const void* data, GLenum type)
//	{
//		unsigned    texId;
//		glGenTextures(1, &texId);
//		glBindTexture(GL_TEXTURE_2D, texId);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, data);
//
//
//		return  texId;
//	}
//	/**
//	*   使用FreeImage加载图片
//	*/
//	unsigned    createTextureFromImage(const char* fileName)
//	{
//		//1 获取图片格式
//		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
//		if (fifmt == FIF_UNKNOWN)
//		{
//			return  0;
//		}
//		//2 加载图片
//		FIBITMAP    *dib = FreeImage_Load(fifmt, fileName, 0);
//
//		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);
//
//		//! 获取数据指针
//		FIBITMAP*   temp = dib;
//		dib = FreeImage_ConvertTo32Bits(dib);
//		FreeImage_Unload(temp);
//
//		BYTE*   pixels = (BYTE*)FreeImage_GetBits(dib);
//		int     width = FreeImage_GetWidth(dib);
//		int     height = FreeImage_GetHeight(dib);
//
//		for (int i = 0; i < width * height * 4; i += 4)
//		{
//			BYTE temp = pixels[i];
//			pixels[i] = pixels[i + 2];
//			pixels[i + 2] = temp;
//		}
//
//		unsigned    res = createTexture(width, height, pixels, GL_RGBA);
//		FreeImage_Unload(dib);
//		return      res;
//	}
//};
typedef std::map<std::string, GLuint> TextureMap;
typedef std::map<std::string, TextureMap> GroupMap;
class TextureManager :public GroupMap {
public:
	GLuint getTexture(const char* fileName,const char* groupName="default")
	{
		TextureMap& tmpTexture = (*this)[groupName];
		TextureMap::iterator iter = tmpTexture.find(fileName);
		if (iter != tmpTexture.end())
		{
			return iter->second;
		}
		else {
			GLuint res = createTextureFromImage(fileName);
			if (res != 0)
			{
				(*this)[groupName][fileName] = res;
				return res;
			}
		}

	}
	void releaseGroup(const char* groupName = "defaut") {
		TextureMap& tmpTexture = (*this)[groupName];
		TextureMap::iterator iter = tmpTexture.begin();
		for (; iter != tmpTexture.end(); iter++)
		{
			glDeleteTextures(1, &iter->second);
		}
		tmpTexture.clear();
	}
	void releaseTexture(GLuint texId, const char* groupName = 0) {
		if (groupName == 0) {
			iterator groupiter = begin();
			for (; groupiter != end(); groupiter++) {
				TextureMap& tmp= groupiter->second;
				TextureMap::iterator iter = tmp.begin();
				for (; iter != tmp.end(); iter++)
				{
					if (iter->second == texId)
					{
						glDeleteTextures(1, &iter->second);
						tmp.erase(iter);
						return;
					}
				}
			}
		}
		else {
			TextureMap& tmpTexture = (*this)[groupName];
			TextureMap::iterator iter = tmpTexture.begin();
			for (; iter != tmpTexture.end(); iter++)
			{
				if (iter->second == texId)
				{
					glDeleteTextures(1, &iter->second);
					tmpTexture.erase(iter);
					break;
				}
			}
		}
		
	}
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
};
class   Sprite 
	:public OpenGLWindow
	,TextureManager
{
public:
	GLuint      _texture1;
	GLuint      _texture2;
	GLuint      _texture3;
	GLuint List1;
public:

public:
	virtual void    onInitGL()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, double(_width) / double(_height), 0.1f, 100.0f);

		_texture1 = TextureManager::getTexture("floor.bmp");
		_texture2 = TextureManager::getTexture("CrossHair.bmp");
		_texture3 = TextureManager::getTexture("CrossHairMask.bmp");
	/*	List1=1;
		glNewList(List1, GL_COMPILE);
		renderCube();
		glEndList();*/


	}
	virtual void    onShutdownGL()
	{
		releaseGroup();
		//glDeleteLists(List1, 1);
	}

	void    renderCube()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, double(_width) / double(_height), 0.1f, 100.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5);

		glDisableClientState(GL_VERTEX_ARRAY);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// Bind the wall texture to the wall quads
		glBindTexture(GL_TEXTURE_2D, _texture1);

		// Display a 4 long quads to represent walls
		glBegin(GL_QUADS);

		////////////////////// BACK WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, -15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, -15);

		////////////////////// FRONT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, 15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, 15);

		////////////////////// LEFT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(-15, 1, 15);


		////////////////////// RIGHT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, 15);

		// Stop drawing the walls
		glEnd();

		// Bind this texture to the floor quad
		glBindTexture(GL_TEXTURE_2D, _texture1);

		// Display a huge quad to represent a floor
		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 16.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(16.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(16.0f, 16.0f);	glVertex3f(15, -1, -15);

		// Stop drawing the floor
		glEnd();

	}
	void    renderOrth()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, _width, _height, 0, -100, 100);
		Vertex  vert[] =
		{
			{ 0, 0 ,            0,  0.0f, 1.0f },
			{ 0, _height ,      0,  0.0f, 0.0f },
			{ _width, _height , 0,  1.0f, 0.0f },
			{ _width, 0 ,       0,  1.0f, 1.0f },
		};
		if (0)
			glColor4f(0, 1, 0, 1);
		else
			glColor4f(1, 1, 1, 1);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glEnable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, _texture3);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vert[0].x);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vert[0].u);

		glDrawArrays(GL_QUADS, 0, 4);

		glBlendFunc(GL_ONE, GL_ONE);

		glBindTexture(GL_TEXTURE_2D, _texture2);
		glDrawArrays(GL_QUADS, 0, 4);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	virtual void render(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//! 立方体
		renderCube();
		//! 遮罩
		renderOrth();
		//glCallList(List1);
	}

};
Sprite   context;

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{

	context.main(800, 600);


	return  0;
}