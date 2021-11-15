#pragma once
#include <string>
#include "Mesh.h"

class HeightMap : public Mesh {
public:	
	HeightMap(const std::string& name);
	~HeightMap() {};

	Vector3 GetHeightmapSize() const { return heightmapSize; }
private:
	Vector3 heightmapSize;
};