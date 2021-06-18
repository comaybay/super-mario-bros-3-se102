#pragma once
#include <string>
#include <unordered_map>
#include <d3dx9.h>

class TextureManager
{
public:
	static void Load(std::string id, std::string path, D3DCOLOR colorKey);
	static LPDIRECT3DTEXTURE9 Get(std::string id);
private:
	static std::unordered_map<std::string, LPDIRECT3DTEXTURE9> textureById;
};

class TextureID {
public:
	static std::string TILES;
};