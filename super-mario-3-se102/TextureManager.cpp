#include "TextureManager.h"
#include "Utils.h"
#include "Game.h"
#include "Contains.h"

std::string TextureId::WORLD_TILES = "__TEX_W_TILES__";
std::string TextureId::WORLD_MAP_TILES = "__TEX_WM_TILES__";

std::unordered_map<std::string, LPDIRECT3DTEXTURE9> TextureManager::textureById;
std::unordered_map<std::string, Utils::Dimension<int>> TextureManager::textureImgDimById;

void TextureManager::Load(const std::string& id, const std::string& path, const D3DCOLOR& color_key)
{
	D3DXIMAGE_INFO image_info;
	HRESULT result = D3DXGetImageInfoFromFile(path.c_str(), &image_info);
	if (result != D3D_OK)
		throw "Cannot find image from: " + path;


	LPDIRECT3DTEXTURE9 texture;
	result = D3DXCreateTextureFromFileEx(
		Game::GetDirect3DDevice(),
		path.c_str(),		//image path
		image_info.Width,
		image_info.Height,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT,
		D3DX_FILTER_NONE,	//no filtering for pixel art game.
		D3DX_FILTER_NONE,	//no filtering for pixel art game.
		color_key,			//transparent color
		&image_info,
		NULL,
		&texture);			//output

	if (result != D3D_OK)
		throw "Fail to create texture from image, image path: " + path;

	textureById[id] = texture;
	textureImgDimById[id] = { static_cast<int>(image_info.Width), static_cast<int>(image_info.Height) };
}

LPDIRECT3DTEXTURE9 TextureManager::Get(const std::string& id) {
	if (Utils::Contains(id, textureById))
		return textureById[id];
	else {
		std::string msg = "No texture with id=\"" + id + "\" found";
		throw std::exception(msg.c_str());
	}
}

const Utils::Dimension<int>& TextureManager::GetDimensionOf(const std::string& id)
{
	if (Utils::Contains(id, textureImgDimById))
		return textureImgDimById[id];
	else {
		std::string msg = "No texture with id=\"" + id + "\" found";
		throw std::exception(msg.c_str());
	}
}

