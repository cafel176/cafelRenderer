#include <shapes/model.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

CAFEL_NAMESPACE_BEGIN

Intersection NodeMesh::intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const
{
	double t0, t1;
	AABB real(range.min*scale + position, range.max*scale + position);
	if(!real.intersect(r, t0, t1))
		return Intersection(t_nohit, r);

	double t = inf;
	dVector n, u_dir, v_dir;
	dVector2 uv;
	for (unsigned int i = 0; i < indices.size()/3; i++)
	{
		dVector v0 = vertices.at(indices.at(i * 3)).Position * scale + position;
		dVector n0 = vertices.at(indices.at(i * 3)).Normal;
		dVector2 v0_uv = vertices.at(indices.at(i * 3)).TexCoords;
		dVector t0 = vertices.at(indices.at(i * 3)).Tangent;
		dVector bt0 = vertices.at(indices.at(i * 3)).Bitangent;

		dVector v1 = vertices.at(indices.at(i * 3 +1)).Position * scale + position;
		dVector2 v1_uv = vertices.at(indices.at(i * 3 + 1)).TexCoords;
		dVector t1 = vertices.at(indices.at(i * 3 + 1)).Tangent;
		dVector bt1 = vertices.at(indices.at(i * 3 + 1)).Bitangent;

		dVector v2 = vertices.at(indices.at(i * 3 +2)).Position * scale + position;
		dVector2 v2_uv = vertices.at(indices.at(i * 3 + 2)).TexCoords;
		dVector t2 = vertices.at(indices.at(i * 3 + 2)).Tangent;
		dVector bt2 = vertices.at(indices.at(i * 3 + 2)).Bitangent;

		// 射线与三角形平行? 
		dVector u = v1 - v0;    // edge1
		dVector v = v2 - v0;    // edge2
		dVector norm = u % v;  // normal
		norm.norm();
		dVector norm_l = (n0 == Vector_zero(double, 3) || norm.dot(n0)) < 0 ? norm : norm * -1;
		
		triangleData re = Warp::get()->intersectTriangle(r, norm_l, u, v, v0);

		if (re.t > 0 && re.t < t)
		{
			t = re.t;
			n = norm_l;

			dVector2 uv10 = v1_uv - v0_uv;
			dVector2 uv20 = v2_uv - v0_uv;
			uv = uv10 * re.u + uv20 * re.v + v0_uv;
			Warp::get()->getTangent(u, v, uv10, uv20, norm_l, u_dir, v_dir);
		}
	}

	if (abs(t - inf) < eps)
		t = t_nohit;
	return Intersection(t, r, n, u_dir, v_dir, uv);
}

Intersection Model::intersect(const Ray &r, const dVector &position, const dVector &rotation, const dVector &scale) const
{
	Intersection its(inf,r);
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		Intersection a = meshes.at(i).intersect(r, getRealPosition(position,rotation), rotation, getRealScale(scale));
		if (a.t > 0 && a.t < its.t)
		{
			its = a;
		}
	}

	if (abs(its.t - inf) < eps)
		its.t = t_nohit;
	return its;
}

void Model::generateCode(std::ostringstream &oss)
{
	Shape::generateCode(oss);
	oss << "void main(){" << std::endl
		<< "    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));" << std::endl
		<< "    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;" << std::endl
		<< "    vs_out.TexCoords = aTexCoords;" << std::endl
		<< "    gl_Position = projection * view * model * vec4(aPos, 1.0);" << std::endl
		<< "}" << std::endl;
}

/*  Functions   */
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		printf("ERROR::ASSIMP:: ");
		printf(importer.GetErrorString());
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	std::vector<Vertex> v;
	unsigned int start = 0;
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		std::vector<Vertex> re = meshes.at(i).getTriangles();
		v.insert(v.end(), re.begin(), re.end());
		std::vector<unsigned int> re2 = meshes.at(i).getIndices();
		for (unsigned int j = 0; j < re2.size(); j++)
			indices.push_back(re2.at(j) + start);

		start += re.size();
	}

	for (unsigned int i = 0; i < v.size(); i++)
	{
		Vertex vertex = v.at(i);
		vertices.push_back(vertex.Position.x);
		vertices.push_back(vertex.Position.y);
		vertices.push_back(vertex.Position.z);
		vertices.push_back(vertex.Normal.x);
		vertices.push_back(vertex.Normal.y);
		vertices.push_back(vertex.Normal.z);
		vertices.push_back(vertex.TexCoords.x);
		vertices.push_back(vertex.TexCoords.y);
	}
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		NodeMesh nodeMesh = processMesh(mesh, scene);
		if (nodeMesh.range.min.x < min.x)
			min.x = nodeMesh.range.min.x;
		if (nodeMesh.range.max.x > max.x)
			max.x = nodeMesh.range.max.x;
		if (nodeMesh.range.min.y < min.y)
			min.y = nodeMesh.range.min.y;
		if (nodeMesh.range.max.y > max.y)
			max.y = nodeMesh.range.max.y;
		if (nodeMesh.range.min.z < min.z)
			min.z = nodeMesh.range.min.z;
		if (nodeMesh.range.max.z > max.z)
			max.z = nodeMesh.range.max.z;

		meshes.push_back(nodeMesh);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

NodeMesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	dVector min = Vector_single(double,inf,3);
	dVector max = Vector_single(double, -inf, 3);

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		dVector vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

		// positions
		if (mesh->mVertices!=nullptr)
		{
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			if (vector.x < min.x)
				min.x = vector.x;
			if (vector.x > max.x)
				max.x = vector.x;
			if (vector.y < min.y)
				min.y = vector.y;
			if (vector.y > max.y)
				max.y = vector.y;
			if (vector.z < min.z)
				min.z = vector.z;
			if (vector.z > max.z)
				max.z = vector.z;
		}

		// normals
		if (mesh->mNormals!=nullptr)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		else 
		{
			vertex.Normal = Vector_zero(double, 3);
		}

		// texture coordinates
		if (mesh->mTextureCoords[0]!=nullptr) // does the mesh contain texture coordinates?
		{
			dVector2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = Vector_zero(double,2);

		// tangent
		if (mesh->mTangents != nullptr)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}

		// bitangent
		if (mesh->mBitangents != nullptr)
		{
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return NodeMesh(vertices,AABB(min,max), indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

CAFEL_NAMESPACE_END