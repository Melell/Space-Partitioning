/**
* @file geometry.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the declaration of some structs that represent
*        certain geometric primitives.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "math.hpp"

namespace cs350 {

    struct transform_data
    {
    public:

        transform_data(const glm::vec3 & init_pos = glm::vec3(0.0f, 0.0f, 0.0f),
                       const glm::vec3 & init_orientation = glm::vec3(0.0f, 0.0f, 0.0f),
                       const glm::vec3 & init_scale = glm::vec3(1.0f, 1.0f, 1.0f));

        const glm::mat4 & get_model_mtx();

        // Transform data, public for convenience
        glm::vec3 mPos;
        glm::vec3 mOrientation;   // Represented as euler angles (degrees) for now for simplicity
        glm::vec3 mScale;

    private:
        glm::mat4 mModelMtx;    // The model to world matrix of this mesh (it is updated automatically before drawing)

        void update_model_mtx();
    };


    struct segment
    {
        segment(const glm::vec3 & pos1 = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3 & pos2 = glm::vec3(1.0f, 0.0f, 0.0f));

        glm::vec3 & operator[](unsigned index);
        const glm::vec3 & operator[](unsigned index) const;

        friend std::istream & operator>>(std::istream & is, segment & rhs);
        friend std::ostream & operator<<(std::ostream & os, const segment & rhs);

        glm::vec3 mPos1;
        glm::vec3 mPos2;
    };


    struct triangle
    {
        triangle(const glm::vec3 & pos1 = glm::vec3(-0.5f, -0.5f, 0.0f),
                 const glm::vec3 & pos2 = glm::vec3(0.5, -0.5f, 0.0f),
                 const glm::vec3 & pos3 = glm::vec3(0.0f, 0.5f, 0.0f));

        friend std::istream & operator>>(std::istream & is, triangle & rhs);
        friend std::ostream & operator<<(std::ostream & os, const triangle & rhs);

        glm::vec3 mPos1;
        glm::vec3 mPos2;
        glm::vec3 mPos3;
    };


    struct plane
    {
        plane(const glm::vec3 & point = glm::vec3(0.0f, 0.0f, 0.0f),
              const glm::vec3 & normal = glm::vec3(0.0f, 1.0f, 0.0f));

        friend std::istream & operator>>(std::istream & is, plane & rhs);
        friend std::ostream & operator<<(std::ostream & os, const plane & rhs);

        glm::vec3 mPoint;
        glm::vec3 mNormal;
    };


    struct aabb
    {
        aabb(const glm::vec3 & min = glm::vec3(-0.5f, -0.5f, -0.5f),
             const glm::vec3 & max = glm::vec3(0.5f, 0.5f, 0.5f));

        friend std::istream & operator>>(std::istream & is, aabb & rhs);
        friend std::ostream & operator<<(std::ostream & os, const aabb & rhs);

        glm::vec3 mMinPos;  // Bottom left and back vertex position of the box
        glm::vec3 mMaxPos;  // Top right and front vertex position of the box
    };


    struct sphere
    {
        sphere(const glm::vec3 & center = glm::vec3(0.0f, 0.0f, 0.0f), float radius = 0.5f);

        friend std::istream & operator>>(std::istream & is, sphere & rhs);
        friend std::ostream & operator<<(std::ostream & os, const sphere & rhs);

        glm::vec3 mCenter;
        float mRadius;
    };


    struct ray
    {
        ray(const glm::vec3 & origin = glm::vec3(0.0f, 0.0f, 0.0f),
            const glm::vec3 dir = glm::vec3(1.0f, 0.0f, 0.0f));

        friend std::istream & operator>>(std::istream & is, ray & rhs);
        friend std::ostream & operator<<(std::ostream & os, const ray & rhs);

        glm::vec3 mOrigin;
        glm::vec3 mDir;
    };


    struct frustum
    {
        frustum(const plane & left = plane(), const plane & right = plane(),
                const plane & bottom = plane(), const plane & top = plane(),
                const plane & near = plane(), const plane & far = plane());

        // Store the planes in a union to be able to access them both individually and through an array
        //union
        //{
        //    struct
        //    {
        //        plane mLeft;
        //        plane mRight;
        //        plane mBottom;
        //        plane mTop;
        //        plane mNear;
        //        plane mFar;
        //    };

            plane mPlanes[6];
        //};

        friend std::istream & operator>>(std::istream & is, frustum & rhs);
        friend std::ostream & operator<<(std::ostream & os, const frustum & rhs);
    };


    enum class classification_t
    {
        inside,
        outside,
        overlapping
    };

    std::istream& operator>>(std::istream & is, cs350::classification_t & rhs);
    std::ostream& operator<<(std::ostream & os, const cs350::classification_t & rhs);

    glm::vec3 closest_point_plane(const glm::vec3 & point, const plane & plane_);
    segment closest_segment_segment(const segment & seg1, const segment & seg2);
    bool intersection_point_sphere(const glm::vec3 & point, const sphere & sphere_);
    classification_t classify_plane_point(const plane & plane_, const glm::vec3 & point, float plane_thickness);
    classification_t classify_plane_triangle(const plane & plane_, const triangle & triangle_, float plane_thickness);
    classification_t classify_plane_aabb(const plane & plane_, const aabb & box, float epsilon);
    classification_t classify_plane_sphere(const plane & plane_, const sphere & sphere_, float epsilon);
    bool intersection_sphere_sphere(const sphere & sphere1, const sphere & sphere2);
    bool intersection_point_aabb(const glm::vec3 & point, const aabb & aabb_);
    bool intersection_aabb_aabb(const aabb & box1, const aabb & box2);
    float intersection_ray_plane(const ray & ray_, const plane & plane_);
    float intersection_ray_aabb(const ray & ray_, const aabb & box);
    float intersection_ray_sphere(const ray & ray_, const sphere & sphere_);
    float intersection_ray_triangle(const ray & ray_, const triangle & tri);
    bool intersection_point_triangle(const glm::vec3 & point, const triangle & tri);
    classification_t classify_frustum_sphere_naive(const frustum & frustum_, const sphere & sphere_);
    classification_t classify_frustum_aabb_naive(const frustum & frustum_, const aabb & box);
}