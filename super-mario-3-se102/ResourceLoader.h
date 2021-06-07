#pragma once
#include <string>

class ResourceLoader
{
public:
	ResourceLoader(std::string rootDirectory);
	void Load() const;
private:
	void LoadTextures(std::string configPath) const;
	void LoadAnimations(std::string configPath) const;
	void LoadTiles(std::string configPath)const;
	std::string root;
};

