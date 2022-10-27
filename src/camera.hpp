/**
* @file camera.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the camera class, which represents a camera that can
*        see a certain part of the scene and render onto a viewport.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once

#include "math.hpp"
namespace cs350
{
    class camera
    {
    public:

        void set_position(const glm::vec3 & newPos);
        void set_target(const glm::vec3 & newTarget);
        void set_projection(float fov_y, const glm::ivec2 & dimensions, float near_z, float far_z);

        glm::vec3 get_position() const;
        glm::vec3 get_target() const;

        glm::vec3 get_view_vec() const;
        glm::vec3 get_right_vec() const;
        glm::vec3 get_up_vec() const;

        glm::mat4 get_view_mtx() const;
        glm::mat4 get_proj_mtx() const;

        void update_view_mtx();     // Update the view matrix of the camera
        void update_persp_mtx();    // Update the perspective matrix of the camera
        void update_all_mtx();      // Update all the camera matrices

    private:
        glm::vec3 mPos;         // Position of the camera
        glm::vec3 mTarget;      // Target for the camera to look to

        // Projection matrix parameters
        float mFovY;                // Vertical field of view
        glm::vec2 mAspectRatio;     // Width and height of the projection window
        float mNearZ;               // The distance to the near plane
        float mFarZ;                // The distance to the far plane

        glm::mat4 mViewMtx;         // The view matrix of this camera (world to view)
        glm::mat4 mProjMtx;         // The perspective projection matrix of this camera
    };
}