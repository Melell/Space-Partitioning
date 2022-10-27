/**
* @file mesh.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/20/09
* @brief Contains the class mesh which has a model to world matrix, a VAO,
*        VBO and EBO (if necessary), and the mesh data.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "mesh_data.hpp"
#include "bvh_tree.hpp"

namespace cs350 {

    class shader;   // Forward declaration
    struct transform_data;

    enum primitive_type
    {
        points = 0,
        lines = 1,
        line_loop = 2,
        line_strip = 3,
        triangles = 4,
        triangle_strip = 5,
        triangle_fan = 6,
        quads = 7 
    };


    class mesh
    {
      private:
      
        unsigned mVAO = 0;                  // The Vertex Array Object that stores all the mesh state
        unsigned mPosVBO = 0;               // The buffer for storing the vertex data
        unsigned mEBO = 0;                  // The buffer for storing the vertex indices
        unsigned mIndicesNumber;            // The number of indices the mesh has
        unsigned mVertices;                 // The number of vertices

        bvh_tree mBVH;                      // The bounding volume hierarchy of this mesh. Computed in setup_mesh (in model space)
        mesh_data mData;                    // The mesh data that was used to setup this mesh

      public:

        mesh();
       
        mesh(const mesh_data & vertices_data);

        ~mesh();

        void setup_mesh(const mesh_data & vertices_data);

        void bind() const;
        void unbind() const;

        void render(shader * shader_, transform_data & trData, const primitive_type & primitive = primitive_type::triangles);

        unsigned indices_number() const;
        unsigned vertices_number() const;

        void destroy_resources();

        bvh_tree & get_bvh();
        const mesh_data & get_mesh_data() const;
    };

}