/**
* @file mesh_data.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the class mesh_data which represents the vertex attributes
*        of a mesh (position, normals, uvs etc), as well as some obj loading functions.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "geometry.hpp" // triangle

namespace cs350 {
    struct mesh_data
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        std::vector<unsigned int> mPosIndices;
        std::vector<unsigned int> mUVsIndices;
        std::vector<unsigned int> mNormalsIndices;
    };

    mesh_data load_obj(const char* filename);
    std::vector<mesh_data> load_objs(const char* filename);
}