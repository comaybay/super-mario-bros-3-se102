#pragma once
#include <string>
#include <unordered_map>
#include <d3dx9.h>
#include "Utils.h"

class TextureManager
{
public:
	static void Load(const std::string&, const std::string& path, const D3DCOLOR& colorKey);
	static LPDIRECT3DTEXTURE9 Get(const std::string& id);
	static const Utils::Dimension<float>& GetDimensionOf(const std::string& id);
private:
	static std::unordered_map<std::string, LPDIRECT3DTEXTURE9> textureById;
	static std::unordered_map<std::string, Utils::Dimension<float>> textureImgDimById;
};

class TextureId {
public:
	static std::string WORLD_TILES;
	static std::string WORLD_MAP_TILES;
};