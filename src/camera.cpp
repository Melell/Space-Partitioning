/**
* @file camera.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the implementation for the camera class.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "camera.hpp"

namespace cs350
{
    /**
    * @brief Sets the position of the camera. No need to call update for position.
    * @param newPos     The position to set the camera to.
    */
    void camera::set_position(const glm::vec3 & newPos)
    {
        mPos = newPos;
    }


    /**
    * @brief Sets the target position the camera will look to. Need to call update
    *        afterwards to update the view matrix.
    * @param newTarget     The target position for the camera to look to.
    */
    void camera::set_target(const glm::vec3 & newTarget)
    {
        mTarget = newTarget;
    }


    /**
    * @brief Sets the parameters to construct the perspective projection matrix of this camera.
    *        Need to call update after this in order to update the projection matrix.
    * @param fov_y          The vertical field of view of the camera.
    * @param dimensions     The width and height of the projection window.
    * @param near_z         The distance to the near plane.
    * @param far_z          The distance to the far plane.
    */
    void camera::set_projection(float fov_y, const glm::ivec2 & dimensions, float near_z, float far_z)
    {
        mFovY = fov_y;
        mAspectRatio = dimensions;
        mNearZ = near_z;
        mFarZ = far_z;
    }


    /**
    * @brief Update the view matrix of the camera.
    */
    void camera::update_view_mtx()
    {
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 viewVec = mTarget - mPos;
        glm::vec3 rightVec = glm::cross(viewVec, worldUp);
        glm::vec3 localUp = glm::cross(rightVec, viewVec);

        mViewMtx = glm::lookAt(mPos, mTarget, localUp);
    }


    /**
    * @brief Update the perspective matrix of the camera.
    */
    void camera::update_persp_mtx()
    {
        mProjMtx = glm::perspective(glm::radians(mFovY), mAspectRatio.x / mAspectRatio.y, mNearZ, mFarZ);
    }


    /**
    * @brief Updates the view and projection matrices of this camera.
    */
    void camera::update_all_mtx()
    {
        update_view_mtx();
        update_persp_mtx();
    }


    /**
    * @brief Getter that returns the world to view matrix associated with this camera.
    * @return glm::mat4     Returns the world to camera space matrix.
    */
    glm::mat4 camera::get_view_mtx() const
    {
        return mViewMtx;
    }


    /**
    * @brief Getter that returns the perspective projection matrix associated with this camera.
    * @return glm::mat4     Returns the perspective projection matrix.
    */
    glm::mat4 camera::get_proj_mtx() const
    {
        return mProjMtx;
    }


    /**
    * @brief Returns the view vector of the camera.
    * @return glm::vec3     The view vector normalized.
    */
    glm::vec3 camera::get_view_vec() const
    {
        return glm::normalize(mTarget - mPos);
    }

    /**
    * @brief Returns the right vector of the camera.
    * @return glm::vec3     The right vector normalized.
    */
    glm::vec3 camera::get_right_vec() const
    {
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 viewVec = mTarget - mPos;
        return glm::normalize(glm::cross(viewVec, worldUp));
    }

    /**
    * @brief Returns the up vector of the camera.
    * @return glm::vec3     The up vector normalized.
    */
    glm::vec3 camera::get_up_vec() const
    {
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 viewVec = mTarget - mPos;
        glm::vec3 rightVec = glm::cross(viewVec, worldUp);
        return glm::normalize(glm::cross(rightVec, viewVec));
    }


    /**
    * @brief Returns the position of the camera.
    * @return glm::vec3     The position.
    */
    glm::vec3 camera::get_position() const
    {
        return mPos;
    }

    /**
    * @brief Returns the target position of the camera.
    * @return glm::vec3     The target position.
    */
    glm::vec3 camera::get_target() const
    {
        return mTarget;
    }
}