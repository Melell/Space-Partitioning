/**
* @file mesh.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/20/09
* @brief Contains the implementation for the mesh class declared in mesh.hpp.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "renderer.hpp"

namespace cs350
{
    /**
    * @brief Constructs a default mesh with no data. Later on, setup_mesh needs
    *        to be called to send the data to the GPU.
    */
    mesh::mesh()
    {
    }


    /**
    * @brief Constructs the mesh with the data provided by setting up all the
    *        buffers and objects needed to pass the data to the GPU.
    *        This function assumes that the mesh_data uses indices.
    * @param  vertices_data      The data of the vertices and all their attributes.
    *                            IMPORTANT: This data should not be used after calling this
    *                            function, since it moves it to an internal mesh_data.
    */
    mesh::mesh(const mesh_data & vertices_data)
    {
        setup_mesh(vertices_data);
    }


    /**
    * @brief Creates the VAO and VBOs necessary, and passes the vertex data to the GPU.
    * @param  vertices_data      The data of the vertices and all their attributes.
    *                            IMPORTANT: This data should not be used after calling this
    *                            function, since it moves it to an internal mesh_data.
    */
    void mesh::setup_mesh(const mesh_data & vertices_data)
    {
        // Generate a Vertex Array Object and bind it
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        // Generate the Vertex Buffer Object, and bind it to the GL_ARRAY_BUFFER target
        glGenBuffers(1, &mPosVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mPosVBO);

        // Send the vertex data to the GPU memory (in the buffer currently bound to GL_ARRAY_BUFFER target)
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices_data.positions.size(), vertices_data.positions.data(), GL_STATIC_DRAW);

        // Tell OpenGL how to navigate the Vertex Buffer Object to use the data in the vertex shader

        // Position attribute (location 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));

        // Generat the EBO, bind it and send the data to the GPU
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_data.mPosIndices.size() * sizeof(unsigned), vertices_data.mPosIndices.data(), GL_STATIC_DRAW);

        mVertices = static_cast<unsigned>(vertices_data.positions.size());
        mIndicesNumber = static_cast<unsigned>(vertices_data.mPosIndices.size());

        // Unbind the VAO
        unbind();

        // Use move assignment operator for cheap move, rather than expensive copy
        mData = std::move(vertices_data);

        // Construct the bvh with the appropriate method
        mBVH.construct_bvh(mData, bvh_construction_method::top_down);
    }


    /**
    * @brief Destructor of the mesh. Deletes all the buffers and the VAO used.
    */
    mesh::~mesh()
    {
        
    }


    /**
    * @brief This is called every frame per mesh, in order to render it in the window using the shader provided.
    * @param shader_to_use      The shader that this mesh will be rendered with.
    * @param trData             The transform data from which we will obtain the model to world matrix.
    * @param primitive          What primitive type to use to draw (triangles, lines, points etc).
    */
    void mesh::render(shader * shader_, transform_data & trData, const primitive_type & primitive)
    {
        // Don't draw anything if there is no shader provided
        if (shader_ == nullptr)
            return;

        // Get the model to world mtx from the parameter transform data
        const glm::mat4 & modelMtx = trData.get_model_mtx();

        // Don't use the shader for now, since there is only one, and we have bound it at the beginning
        //shader_->Use();

        camera & mainCam = renderer::instance().get_camera();
        glm::mat4 finalMtx = mainCam.get_proj_mtx() * mainCam.get_view_mtx() * modelMtx;

        // Set the transformation uniform for the vertex shader
        // (hardcode the 0 because we know it is at location 0)
        glUniformMatrix4fv(0, 1, GL_FALSE, &finalMtx[0][0]);

        // Bind the VAO
        bind();

        // Draw the mesh
        glDrawElements(GL_POINTS + primitive, mIndicesNumber, GL_UNSIGNED_INT, nullptr);
    }


    /**
    * @brief Binds the VAO corresponding to this mesh.
    */
    void mesh::bind() const
    {
        glBindVertexArray(mVAO);
    }
        

    /**
    * @brief Unbinds the VAO corresponding to this mesh.
    */
    void mesh::unbind() const
    {
        glBindVertexArray(0);
    }


    /**
    * @brief Returns the number of indices in the mesh.
    * @return unsigned      The internal variable that holds the number of indices.
    */
    unsigned mesh::indices_number() const
    {
        return mIndicesNumber;
    }


    /**
    * @brief Returns the number of vertices in the mesh.
    * @return unsigned      The internal variable that holds the number of vertices.
    */
    unsigned mesh::vertices_number() const
    {
        return mVertices;
    }

    void mesh::destroy_resources()
    {
        unbind();
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mPosVBO);
        glDeleteBuffers(1, &mEBO);      // If mEBO is 0, it is silently ignored
    }


    /**
    * @brief Getter that returns the bounding volume hierarchy of this mesh.
    * @return bvh_tree&      Returns a ref to the bvh_tree representing the bvh.
    */
    bvh_tree & mesh::get_bvh()
    {
        return mBVH;
    }


    /**
    * @brief Getter that returns the internal data of this mesh.
    * @return const mesh_data&      Returns a const ref to the mesh_data representing this mesh.
    */
    const mesh_data & mesh::get_mesh_data() const
    {
        return mData;
    }
}