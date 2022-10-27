/**
* @file debug.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the implementation of functions for debug drawing different primitives.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "debug.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "bvh_tree.hpp"

namespace glm {
    std::istream& operator>>(std::istream& is, vec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec2 const& p)
    {
        os << p.x << " " << p.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, vec3& v)
    {
        is >> v.x >> v.y >> v.z;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec3 const& p)
    {
        os << p.x << " " << p.y << " " << p.z;
        return os;
    }
}

namespace cs350
{
    /**
    * @brief Draws a debug point immediately with the attributes provided as parameters.
    * @param pt         The world space coordinates of the point.
    * @param color      The color to draw it with.
    */
    void debug_draw_point(glm::vec3 pt, glm::vec4 color)
    {
        // Assemble the mesh data (just one position and index)
        mesh_data pointData;
        pointData.positions.push_back(pt);
        pointData.mPosIndices.push_back(0u);

        // Construct the mesh
        mesh pointMesh(pointData);

        // Take advantage of the fact that we are only using one shader, and set the color uniform now
        glUniform4f(1, color.x, color.y, color.z, color.w);

        glPointSize(4);

        transform_data defaultTr{};

        // Render as GL_POINTS
        pointMesh.render(renderer::instance().get_shader(), defaultTr, primitive_type::points);
    }


    /**
    * @brief Draws a debug mesh with using points as primitive.
    * @param mesh       The mesh to debug render.
    * @param m2w        The model to world transformation of this mesh.
    * @param color      The color to draw this mesh with.
    */
    void debug_draw_points(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        // Set its color as uniform
        glUniform4f(1, color.x, color.y, color.z, color.w);

        // Set the transformation matrix as uniform
        camera & mainCam = renderer::instance().get_camera();
        glm::mat4 finalMtx = mainCam.get_proj_mtx() * mainCam.get_view_mtx() * m2w;
        glUniformMatrix4fv(0, 1, GL_FALSE, &finalMtx[0][0]);

        // Bind the VAO and draw the mesh
        mesh->bind();
        glDrawArrays(GL_POINTS, 0, mesh->vertices_number());
    }


    /**
    * @brief Draws a debug segment immediately with the positions
    *        provided in s, and the color provided.
    * @param s          The segment vertex information to form the mesh.
    * @param color      The color it will be drawn with.
    */
    void debug_draw_segment(segment const& s, glm::vec4 color)
    {
        // Assemble the mesh data
        mesh_data data;
        data.positions.resize(2);
        data.mPosIndices.resize(2);
        data.positions[0] = s.mPos1;
        data.positions[1] = s.mPos2;
        data.mPosIndices[0] = 0;
        data.mPosIndices[1] = 1;

        // Since I haven't provided transform data, m2w will
        // be the identity (points are already in world space)
        mesh segmentMesh(data);

        // Take advantage of the fact that we are only using one shader, and set the color uniform now
        glUniform4f(1, color.x, color.y, color.z, color.w);

        transform_data defaultTr{};

        segmentMesh.render(renderer::instance().get_shader(), defaultTr, primitive_type::lines);
    }


    /**
    * @brief Draws a mesh in wireframe mode.
    * @param mesh       The mesh to draw.
    * @param m2w        The model to world transformation of the mesh.
    * @param color      The color to draw the mesh with.
    */
    void debug_draw_segments(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        // Set the color to use
        glUniform4f(1, color.x, color.y, color.z, color.w);

        camera & mainCam = renderer::instance().get_camera();
        glm::mat4 finalMtx = mainCam.get_proj_mtx() * mainCam.get_view_mtx() * m2w;
        glUniformMatrix4fv(0, 1, GL_FALSE, &finalMtx[0][0]);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);

        // Bind the VAO and draw the mesh
        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->indices_number(), GL_UNSIGNED_INT, nullptr);

        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    /**
    * @brief Draws a debug triangle immediately with the attributes provided as parameters.
    * @param t          The geometry information of the triangle.
    * @param color      The color to draw it with.
    * @param draw_type  The type of draw to make.
    */
    void debug_draw_triangle(triangle const& t, glm::vec4 color, debug_draw_type draw_type)
    {
        // Assemble the mesh data
        mesh_data triangleData;
        triangleData.positions.resize(3);
        triangleData.mPosIndices.resize(3);
        triangleData.positions[0] = t.mPos1;
        triangleData.positions[1] = t.mPos2;
        triangleData.positions[2] = t.mPos3;
        triangleData.mPosIndices[0] = 0;
        triangleData.mPosIndices[1] = 1;
        triangleData.mPosIndices[2] = 2;

        // Construct the mesh with the mesh data
        mesh triangleMesh(triangleData);

        debug_draw(draw_type, &triangleMesh, glm::mat4(1.0f), color);
    }


    /**
    * @brief Draws a debug plane immediately with the attributes provided as parameters.
    * @param pt         A point in world space that is in the plane.
    * @param n          The normal of the plane.
    * @param color      The color to draw the plane with.
    * @param scale      
    */
    void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale)
    {
        renderer & instance = renderer::instance();
        mesh & quadMesh = instance.resources().meshes.quad;

        glUniform4f(1, color.x, color.y, color.z, color.w);

        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 rightVec = glm::cross(n, worldUp);
        glm::vec3 localUp = glm::normalize(glm::cross(rightVec, n));
        glm::mat4 trRot = glm::inverse(glm::lookAt(pt, pt + glm::normalize(n), localUp));
        glm::mat4 modelMtx = glm::scale(trRot, glm::vec3(scale, scale, scale));

        camera & mainCam = renderer::instance().get_camera();
        glm::mat4 finalMtx = mainCam.get_proj_mtx() * mainCam.get_view_mtx() * modelMtx;

        // Set the transformation uniform for the vertex shader
        // (hardcode the 0 because we know it is at location 0)
        glUniformMatrix4fv(0, 1, GL_FALSE, &finalMtx[0][0]);

        // Bind the VAO
        quadMesh.bind();

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, quadMesh.indices_number(), GL_UNSIGNED_INT, nullptr);

        // Set another color for the backface, and render the only backfaces
        glUniform4f(1, 0.4f, 0.4f, 0.4f, 1.0f);
        glCullFace(GL_FRONT);
        glDrawElements(GL_TRIANGLES, quadMesh.indices_number(), GL_UNSIGNED_INT, nullptr);

        // Reset to backface culling
        glCullFace(GL_BACK);

        // Draw the normals
        debug_draw_segment(segment(pt, pt + n), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }


    /**
    * @brief Draws a debug aabb (cube) immediately with the attributes provided as parameters.
    * @param a          The geometry information of the aabb.
    * @param color      The color to draw it with.
    * @param draw_type  Determines the way to render the shape.
    */
    void debug_draw_aabb(aabb const& a, glm::vec4 color, debug_draw_type draw_type)
    {
        renderer & instance = renderer::instance();
        mesh & cubeMesh = instance.resources().meshes.cube;
        
        // Get the center of the cube in world space
        glm::vec3 diagonal = a.mMaxPos - a.mMinPos;
        glm::vec3 halfDiagonal = diagonal / 2.0f;
        glm::vec3 center = a.mMinPos + halfDiagonal;

        transform_data trData;

        // Prepare the model matrix (ignore the rotation since it is a aabb)
        trData.mPos = center;
        trData.mScale.x = glm::abs(diagonal.x);
        trData.mScale.y = glm::abs(diagonal.y);
        trData.mScale.z = glm::abs(diagonal.z);

        // Call the debug drawing "dispatcher"
        debug_draw(draw_type, &cubeMesh, trData.get_model_mtx(), color);
    }


    /**
    * @brief Draws a debug sphere immediately with the attributes provided as parameters.
    * @param s          The geometry information of the sphere.
    * @param color      The color to draw it with.
    * @param draw_type  Determines the way to render the shape.
    */
    void debug_draw_sphere(sphere const& s, glm::vec4 color, debug_draw_type draw_type)
    {
        // Get the obj loaded sphere
        renderer & instance = renderer::instance();
        mesh & sphere = instance.resources().meshes.sphere;

        // Update its transform data
        transform_data trData;
        trData.mPos = s.mCenter;
        trData.mScale.x = s.mRadius;
        trData.mScale.y = s.mRadius;
        trData.mScale.z = s.mRadius;

        // Call the debug drawing "dispatcher"
        debug_draw(draw_type, &sphere, trData.get_model_mtx(), color);
    }


    /**
    * @brief Draws a mesh both with fill and wireframe mode.
    * @param mesh       The mesh to draw.
    * @param m2w        The model to world transformation of the mesh.
    * @param color      The color to draw the mesh with.
    */
    void debug_draw_fancy(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        // Render the mesh with a plain color
        debug_draw_plain_color(mesh, m2w, color);

        // Render it in wireframe mode (disable depth testing and
        // backface culling to be able to see through)
        glUniform4f(1, 0.0f, 0.0f, 0.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, mesh->indices_number(), GL_UNSIGNED_INT, nullptr);

        // Enable depth tesing and backface culling again, and reset drawing mode
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    /**
    * @brief Draws a mesh using just the color passes as parameter.
    * @param mesh       The mesh to draw.
    * @param m2w        The model to world transformation of the mesh.
    * @param color      The color to draw the mesh with.
    */
    void debug_draw_plain_color(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 const& color)
    {
        // Set the color to use
        glUniform4f(1, color.x, color.y, color.z, color.w);

        camera & mainCam = renderer::instance().get_camera();
        glm::mat4 finalMtx = mainCam.get_proj_mtx() * mainCam.get_view_mtx() * m2w;
        glUniformMatrix4fv(0, 1, GL_FALSE, &finalMtx[0][0]);

        // Bind the VAO and draw the mesh
        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->indices_number(), GL_UNSIGNED_INT, nullptr);
    }


    /**
    * @brief Draws the tree passed as parameter with the properties passed as parameter.
    * @param positions      The vector of model space positions to index.
    * @param renderBV       Whether to render the BV of the root node.
    * @param bvColor        The color to draw the bv with (wireframe).
    * @param m2w            The model to world transformation of the mesh.
    * @param color          The color to draw the mesh with.
    * @param draw_type      The policy to draw the triangles (fancy by default).
    */
    void debug_draw_bvh_tree(const std::vector<glm::vec3> & positions,
							bvh_node * tree,
                            bool renderBV,
							const glm::vec4 & bvColor,
							const glm::mat4 & m2w,
							const glm::vec4 & color,
							debug_draw_type draw_type)
    {
        if (tree == nullptr)
            return;

        // Render the bv if enabled
        if (renderBV)
        {
            aabb worldBV;
            worldBV.mMinPos = glm::vec3(m2w * glm::vec4(tree->mBV.mMinPos, 1.0f));
            worldBV.mMaxPos = glm::vec3(m2w * glm::vec4(tree->mBV.mMaxPos, 1.0f));
            debug_draw_aabb(worldBV, bvColor, debug_draw_type::wireframe);
        }
        
        if (tree->mNodeType == tree_node_type::leaf)
        {
            for(size_t currentIndex = 0; currentIndex < tree->mIndices.size(); currentIndex += 3)
            {
                const glm::vec3 & worldPos1 = glm::vec3(m2w * glm::vec4(positions[tree->mIndices[currentIndex]], 1.0f));
                const glm::vec3 & worldPos2 = glm::vec3(m2w * glm::vec4(positions[tree->mIndices[currentIndex + 1]], 1.0f));
                const glm::vec3 & worldPos3 = glm::vec3(m2w * glm::vec4(positions[tree->mIndices[currentIndex + 2]], 1.0f));

                triangle currentTri(worldPos1, worldPos2, worldPos3);
                debug_draw_triangle(currentTri, color, debug_draw_type::fancy);
            }
        }

        debug_draw_bvh_tree(positions, tree->mLeft, false, bvColor, m2w, color, draw_type);
        debug_draw_bvh_tree(positions, tree->mRight, false, bvColor, m2w, color, draw_type);
    }


    /**
    * @brief Draws a mesh using the procedure passes in the first parameter.
    * @param draw_type  Determines the way to render the shape.
    * @param mesh       The mesh to draw.
    * @param m2w        The model to world transformation of the mesh.
    * @param color      The color to draw the mesh with.
    */
    void debug_draw(debug_draw_type draw_type, mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        switch (draw_type)
        {
        case debug_draw_type::fancy:
            debug_draw_fancy(mesh, m2w, color);
            break;

        case debug_draw_type::plain_color:
            debug_draw_plain_color(mesh, m2w, color);
            break;

        case debug_draw_type::points:
            debug_draw_points(mesh, m2w, color);
            break;

        case debug_draw_type::wireframe:
            debug_draw_segments(mesh, m2w, color);
            break;
        
        default:
            std::cout<<"DEBUG : DEBUG_DRAW : NONE OF THE OPTIONS WAS SELECTED\n";
            break;
        }
    }
}