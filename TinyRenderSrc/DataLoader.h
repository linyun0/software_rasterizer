#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "geometry.h"
//class Texture;
//class Mesh;
class Model
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
   Model(std::string const& path= "D:\\Download\\Learning-master\\Learning-master\\GAMES101\\Homework\\Hw3\\Assignment3\\Code\\models\\spot\\spot_triangulated_good.obj", bool gamma = false);
    //Model(std::string const& path="D:\\Download\\backpack\\backpack.obj", bool gamma = false);
  //  Model(std::string const& path= "D:\\Download\\Learning-master\\Learning-master\\GAMES101\\Homework\\Hw3\\Assignment3\\Code\\models\\cube\\cube.obj", bool gamma = false);
    std::vector<Mesh> GetMeshs();
    // draws the model, and thus all its meshes

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.

};
