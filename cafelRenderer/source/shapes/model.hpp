#pragma once

#if !defined(_CAFEL_MODEL_HPP_)
#define _CAFEL_MODEL_HPP_

#include <cafel/basic/AABB.hpp>
#include <cafel/components/shape.h>
#include <sampler/texture.hpp>

#include <assimp/scene.h>

#include <string>
#include <vector>

CAFEL_NAMESPACE_BEGIN

struct Vertex 
{
	// position
	dVector Position;
	// normal
	dVector Normal;
	// texCoords
	dVector2 TexCoords;
	// tangent
	dVector Tangent;
	// bitangent
	dVector Bitangent;
};

class NodeMesh
{
public:
	/*  Functions  */
// constructor
	NodeMesh(std::vector<Vertex> vertices_, AABB aabb_, std::vector<unsigned int> indices_, std::vector<Texture> textures_):vertices(vertices_), indices(indices_), textures(textures_),range(aabb_)
	{}

	Intersection intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale = Vector_one(double, 3)) const;
	std::vector<Vertex> getTriangles() const { return vertices; }
	std::vector<unsigned int> getIndices() const { return indices; }

	AABB range;
private:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

class Model : public Shape
{
public:
	/*  Functions   */
// constructor, expects a filepath to a 3D model.
	Model(SceneNode *node) :Shape(node)
	{
		if (node->hasParam("gammaCorrection"))
		{
			gammaCorrection = true;
		}
		else
			gammaCorrection = false;

		std::string path = node->getChildFirst("path")->getParam("value");
		loadModel(path);
	}

	Intersection intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const;
	std::vector<Texture> getTextures() { return textures_loaded; }
	void setTextureID(unsigned int id, unsigned int index = 0) { textures_loaded[index].id = id; }
	dVector getMin(const dVector &position, const dVector &rotation, const dVector &scale) const 
	{ 
		return min * getRealScale(scale) + getRealPosition(position, rotation);
	}
	dVector getMax(const dVector &position, const dVector &rotation, const dVector &scale) const 
	{ 
		return max * getRealScale(scale) + getRealPosition(position,rotation); 
	}

	std::string tostring() const { return "NodeMesh:(:)"; }
	void generateCode(std::ostringstream &oss);

private:
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<NodeMesh> meshes;
	dVector min, max;
	std::string directory;
	bool gammaCorrection;

	void loadModel(std::string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	NodeMesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

CAFEL_NAMESPACE_END

#endif 