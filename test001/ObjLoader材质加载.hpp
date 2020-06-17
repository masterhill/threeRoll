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
	std::vector<CELL::float3> _vertexs;
	std::vector<CELL::float3> _normals;
	std::vector<CELL::float2> _uvs;
	Material _material;
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
		_vertexs.clear();
		_normals.clear();
		_uvs.clear();
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

		std::vector<CELL::float3> vertexs;
		std::vector<CELL::float3> normals;
		std::vector<CELL::float2> uvs;

		for (int i = 0; i < _faces.size(); i++)
		{
			face& tmpface = _faces[i];

			float3 v1 = _vertexs[tmpface.v1];
			float3 v2 = _vertexs[tmpface.v2];
			float3 v3 = _vertexs[tmpface.v3];

			vertexs.push_back(v1);
			vertexs.push_back(v2);
			vertexs.push_back(v3);

			float3 n1 = _normals[tmpface.n1];
			float3 n2 = _normals[tmpface.n2];
			float3 n3 = _normals[tmpface.n3];

			normals.push_back(v1);
			normals.push_back(v2);
			normals.push_back(v3);

			float2 uv1 = _uvs[tmpface.uv1];
			float2 uv2 = _uvs[tmpface.uv2];
			float2 uv3 = _uvs[tmpface.uv3];

			uvs.push_back(uv1);
			uvs.push_back(uv2);
			uvs.push_back(uv3);
		}

		_vertexs = vertexs;
		_normals = normals;
		_uvs = uvs;
		fclose(fp);
		return true;
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
		glVertexPointer(3, GL_FLOAT, 0, &_vertexs.front());
		glNormalPointer(GL_FLOAT, 0, &_normals.front());
		glTexCoordPointer(2, GL_FLOAT,0, &_uvs.front());

		glDrawArrays(GL_TRIANGLES, 0, _vertexs.size());
	}
};