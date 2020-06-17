#include "CELLMath.hpp"
#include <vector>
using namespace CELL;
class ObjLoader {
public:
	struct face {
		int v1, v2, v3;
		int n1, n2, n3;
		int uv1, uv2, uv3;
	};
	struct Material {
		float3 ambient;
		float3 diffuse;
		float3 specal;
		char szTex[128];
		GLuint texId;
	};
	//一个面的数据
	struct ObjVert
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	struct ObjIndex
	{
		unsigned short p1, p2, p3;
	};

	Material _material;
	std::vector<ObjVert> _objvert;
	std::vector<ObjIndex> _objindex;
	ObjLoader() {
		memset(&_material, 0, sizeof(_material));
	}
	~ObjLoader() {

	}
	virtual bool Load(const char* filename)
	{
		FILE* fp = fopen(filename, "rt");
		if (fp == 0)
		{
			return false;
		}
		std::vector<face> _faces;
		std::vector<CELL::float3> _vertexs;
		std::vector<CELL::float3> _normals;
		std::vector<CELL::float2> _uvs;
		while (feof(fp) == 0)
		{
			char databuf[1024] = {};
			fgets(databuf, sizeof(databuf), fp);
			if (strncmp(databuf,"mtllib",6)==0)
			{
				char tmpdata[128];
				sscanf(databuf, "mtllib %s", tmpdata);
				parseMaterial(tmpdata);
			}
			else if (databuf[0] == 'v'&&databuf[1] == ' ')
			{
				CELL::float3 pos;
				sscanf(databuf, "v  %f %f %f", &pos.x, &pos.y, &pos.z);
				_vertexs.push_back(pos);
			}
			else if (databuf[0] == 'v'&&databuf[1] == 'n')
			{
				CELL::float3 pos;
				sscanf(databuf, "vn  %f %f %f", &pos.x, &pos.y, &pos.z);
				_normals.push_back(pos);
			}
			else if (databuf[0] == 'v'&&databuf[1] == 't')
			{
				CELL::float2 pos;
				sscanf(databuf, "vt  %f %f", &pos.x, &pos.y);
				_uvs.push_back(pos);
			}
			else if (databuf[0] == 'f'&&databuf[1] == ' ')
			{
				face tmp;
				sscanf(databuf, "f %d/%d/%d %d/%d/%d %d/%d/%d"
					, &tmp.v1
					, &tmp.uv1
					, &tmp.n1
					, &tmp.v2
					, &tmp.uv2
					, &tmp.n2
					, &tmp.v3
					, &tmp.uv3
					, &tmp.n3
				);
				tmp.v1 -= 1;
				tmp.v2 -= 1;
				tmp.v3 -= 1;

				tmp.n1 -= 1;
				tmp.n2 -= 1;
				tmp.n3 -= 1;

				tmp.uv1 -= 1;
				tmp.uv2 -= 1;
				tmp.uv3 -= 1;
				_faces.push_back(tmp);
			}
		
		}

		

		for (int i = 0; i < _faces.size(); i++)
		{
			ObjIndex index;
			{
				ObjVert tmpObj;
				face& tmpface = _faces[i];

				float3 v1 = _vertexs[tmpface.v1];
				float3 n1 = _normals[tmpface.n1];
				float2 uv1 = _uvs[tmpface.uv1];

				tmpObj.x = v1.x;
				tmpObj.y = v1.y;
				tmpObj.z = v1.z;

				tmpObj.nx = n1.x;
				tmpObj.ny = n1.y;
				tmpObj.nz = n1.z;

				tmpObj.u = uv1.x;
				tmpObj.v = uv1.y;

				int pos = find(tmpObj);
				if (pos == -1)
				{
					pos = _objvert.size();
					_objvert.push_back(tmpObj);
				}
				index.p1 = pos;
			}
			{
				ObjVert tmpObj;
				face& tmpface = _faces[i];

				float3 v1 = _vertexs[tmpface.v2];
				float3 n1 = _normals[tmpface.n2];
				float2 uv1 = _uvs[tmpface.uv2];

				tmpObj.x = v1.x;
				tmpObj.y = v1.y;
				tmpObj.z = v1.z;

				tmpObj.nx = n1.x;
				tmpObj.ny = n1.y;
				tmpObj.nz = n1.z;

				tmpObj.u = uv1.x;
				tmpObj.v = uv1.y;

				int pos = find(tmpObj);
				if (pos == -1)
				{
					pos = _objvert.size();
					_objvert.push_back(tmpObj);
				}
				index.p2 = pos;
			}
			{
				ObjVert tmpObj;
				face& tmpface = _faces[i];

				float3 v1 = _vertexs[tmpface.v3];
				float3 n1 = _normals[tmpface.n3];
				float2 uv1 = _uvs[tmpface.uv3];

				tmpObj.x = v1.x;
				tmpObj.y = v1.y;
				tmpObj.z = v1.z;

				tmpObj.nx = n1.x;
				tmpObj.ny = n1.y;
				tmpObj.nz = n1.z;

				tmpObj.u = uv1.x;
				tmpObj.v = uv1.y;

				int pos = find(tmpObj);
				if (pos == -1)
				{
					pos = _objvert.size();
					_objvert.push_back(tmpObj);
				}
				index.p3 = pos;
			}
			_objindex.push_back(index);
		

			
		}
		fclose(fp);
		return true;
	}
	int find(ObjVert curVer)
	{
		for (int i = 0; i < _objvert.size(); i++)
		{
			ObjVert tmp = _objvert[i];
			if (curVer.x == tmp.x
				&&	curVer.y == tmp.y
				&&	curVer.z == tmp.z
				&&	curVer.nx == tmp.nx
				&&	curVer.ny == tmp.ny
				&&	curVer.nz == tmp.nz
				&&	curVer.u == tmp.u
				&&	curVer.v == tmp.v
				)
			{
				return i;
			}
		}
		return -1;
	}
	bool parseMaterial(const char* filename)
	{
		FILE* fp = fopen(filename, "rt");
		if (fp == 0)
		{
			return false;
		}
		while (feof(fp) == 0)
		{
			char databuf[1024] = {};
			fgets(databuf, sizeof(databuf), fp);
			if (strncmp(databuf, "Ka", 2) == 0)
			{
				sscanf(databuf, "Ka  %f %f %f"
					, &_material.ambient.x
					, &_material.ambient.y
					, &_material.ambient.z
				);
			}
			else if (strncmp(databuf, "Kd", 2) == 0)
			{
				sscanf(databuf, "Kd  %f %f %f"
					, &_material.diffuse.x
					, &_material.diffuse.y
					, &_material.diffuse.z
				);
			}
			else if (strncmp(databuf, "Kd", 2) == 0)
			{
				sscanf(databuf, "Ks  %f %f %f"
					, &_material.specal.x
					, &_material.specal.y
					, &_material.specal.z
				);
			}
			else if (strncmp(databuf, "map_Kd", 6) == 0)
			{
				
				sscanf(databuf, "map_Kd %s"
					, &_material.szTex);
			}
			
		}
		return true;
	}
};
class ObjRender :public ObjLoader
{
public:
	/**
	*   Ê¹ÓÃFreeImage¼ÓÔØÍ¼Æ¬
	*/
	unsigned        createTextureFromImage(const char* fileName)
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

	unsigned        createTexture(int w, int h, const void* data)
	{
		unsigned    texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return  texId;
	}
	bool  Load(const char* filename)
	{
		bool res = ObjLoader::Load(filename);
		if (res&&strlen(_material.szTex) > 0)
		{
			_material.texId=createTextureFromImage(_material.szTex);
		}
		return res;
	}
	void render() {
		if (_material.texId != 0)
		{
			glBindTexture(GL_TEXTURE_2D, _material.texId);
		}
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(ObjVert), &_objvert[0].x);
		glNormalPointer(GL_FLOAT, sizeof(ObjVert), &_objvert[0].nx);
		glTexCoordPointer(2, GL_FLOAT, sizeof(ObjVert), &_objvert[0].u);
		//索引绘制
		glDrawElements(GL_TRIANGLES, _objindex.size() * 3, GL_UNSIGNED_SHORT, &_objindex.front());
	}
};