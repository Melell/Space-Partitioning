/**
* @file geometry.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the implementation of the methods in the geometric
*        primitives' structs declared in geometry.hpp.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "geometry.hpp"

namespace cs350
{
    transform_data::transform_data(const glm::vec3 & init_pos,
                                   const glm::vec3 & init_orientation,
                                   const glm::vec3 & init_scale)
        :   mPos(init_pos),
            mOrientation(init_orientation),
            mScale(init_scale),
            mModelMtx(glm::mat4(1.0f))
    {
    }

    /**
    * @brief For now, this is called every frame in the render function
    *        of each mesh, and updates the model to world matrix.
    */
    void transform_data::update_model_mtx()
    {
        mModelMtx = glm::mat4(1.0f);
        mModelMtx = glm::translate(mModelMtx, mPos);
        mModelMtx = glm::rotate(mModelMtx, glm::radians(mOrientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mModelMtx = glm::rotate(mModelMtx, glm::radians(mOrientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mModelMtx = glm::rotate(mModelMtx, glm::radians(mOrientation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        mModelMtx = glm::scale(mModelMtx, mScale);
    }


    /**
    * @brief Returns the internal model to world matrix corresponding to this transform data.
    * @return glm::mat4      The internal 4x4 model to world matrix.
    */
    const glm::mat4 & transform_data::get_model_mtx()
    {
        update_model_mtx();
        return mModelMtx;
    }


    /**
    * @brief Initializes the segment endpoints to the values passed
    *        as parameter (they default to (0,0,0) and (1,0,0))
    * @param start     The first endpoint position.
    * @param end       The second endpoint position.
    */
    segment::segment(const glm::vec3 & pos1, const glm::vec3 & pos2)
        :   mPos1(pos1),
            mPos2(pos2)
    {
    }


    /**
    * @brief Non-const overload of the subscript operator, to
    *        access a certain endpoint of the segment.
    * @param index          0 for the first endpoint, 1 for the second.
    * @return glm::vec3&    The first endpoint if index is 0, the second endpoint if it is 1.
    */
    glm::vec3 & segment::operator[](unsigned index)
    {
        // Sanity check
        assert(index < 2);

        return index == 0 ? mPos1 : mPos2;
    }


    /**
    * @brief Const overload of the subscript operator, to
    *        access a certain endpoint of the segment.
    * @param index              0 for the first endpoint, 1 for the second.
    * @return const glm::vec3&  The first endpoint if index is 0, the second endpoint if it is 1.
    */
    const glm::vec3 & segment::operator[](unsigned index) const
    {
        // Sanity check
        assert(index < 2);

        return index == 0 ? mPos1 : mPos2;
    }


    /**
    * @brief Overload of the input operator to be able to read "segments"
    *        (two endpoints) from file.
    * @param is                 The input stream from which we will read.
    * @param rhs                The segment object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, segment & rhs)
    {
        is>>rhs.mPos1>>rhs.mPos2;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "segments"
    *        (two endpoints) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The segment object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const segment & rhs)
    {
        os<<rhs.mPos1<<" "<<rhs.mPos2<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the triangle vertices' positions to the values passed
    *        as parameter (they default to (-0.5,-0.5,0), (0.5,-0.5,0) and (0,0.5,0)).
    * @param pos1     The first vertex position.
    * @param pos2     The second vertex position.
    * @param pos3     The third vertex position.
    */
    triangle::triangle(const glm::vec3 & pos1, const glm::vec3 & pos2, const glm::vec3 & pos3)
        :   mPos1(pos1),
            mPos2(pos2),
            mPos3(pos3)
    {
    }


    /**
    * @brief Overload of the input operator to be able to read "triangles"
    *        (three vertices) from file.
    * @param is                 The input stream from which we will read.
    * @param rhs                The triangle object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, triangle & rhs)
    {
        is>>rhs.mPos1>>rhs.mPos2>>rhs.mPos3;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "triangles"
    *        (three vertices) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The triangle object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const triangle & rhs)
    {
        os<<rhs.mPos1<<" "<<rhs.mPos2<<" "<<rhs.mPos3<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the plane's point and normal to the values passed
    *        as parameter (they default to (0,0,0) and (0,1,0)).
    * @param point      The position in world space of the point in the plane.
    * @param normal     The normal of the plane.
    */
    plane::plane(const glm::vec3 & point, const glm::vec3 & normal)
        :   mPoint(point),
            mNormal(normal)
    {
    }


    /**
    * @brief Overload of the input operator to be able to read "planes"
    *        (point and a normal) from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The plane object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, plane & rhs)
    {
        is>>rhs.mPoint>>rhs.mNormal;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "planes"
    *        (point and a normal) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The plane object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const plane & rhs)
    {
        os<<rhs.mPoint<<" "<<rhs.mNormal<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the aabb's min and max vertices' positions to the values passed
    *        as parameter (they default to (-0.5,-0.5,-0.5) and (0.5,0.5,0.5).
    * @param min     The minimum vertex position (bottom left back).
    * @param max     The maximum vertex position (top right front).
    */
    aabb::aabb(const glm::vec3 & min, const glm::vec3 & max)
        :   mMinPos(min),
            mMaxPos(max)
    {
    }


    /**
    * @brief Overload of the input operator to be able to read "aabbs"
    *        (min and max points) from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The aabb object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, aabb & rhs)
    {
        is>>rhs.mMinPos>>rhs.mMaxPos;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "aabbs"
    *        (min and max points) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The aabb object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const aabb & rhs)
    {
        os<<rhs.mMinPos<<" "<<rhs.mMaxPos<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the sphere's center and radius to the values passed as parameter,
    *        which default to (0,0,0) and 0.5 respectively.
    * @param center     The center that the sphere will have.
    * @param radius     The radius that the sphere will have.
    */
    sphere::sphere(const glm::vec3 & center , float radius)
        :   mCenter(center),
            mRadius(radius)     // Actually, this could be the diameter, depending on the range the mesh's vertices positions are
    {
    }

    /**
    * @brief Overload of the input operator to be able to read "spheres"
    *        (point and a radius) from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The sphere object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, sphere & rhs)
    {
        is>>rhs.mCenter>>rhs.mRadius;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "spheres"
    *        (point and a radius) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The sphere object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const sphere & rhs)
    {
        os<<rhs.mCenter<<" "<<rhs.mRadius<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the ray's origin and direction to the values passed as parameter,
    *        which default to (0,0,0) and (1,0,0) respectively.
    * @param origin     The origin of the ray.
    * @param dir        The direction the ray follows.
    */
    ray::ray(const glm::vec3 & origin, const glm::vec3 dir)
        :   mOrigin(origin),
            mDir(dir)
    {
    }


    /**
    * @brief Overload of the input operator to be able to read "ray"s
    *        (origin and direction) from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The ray object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, ray & rhs)
    {
        is>>rhs.mOrigin>>rhs.mDir;
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "ray"s
    *        (origin and a direction) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The ray object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const ray & rhs)
    {
        os<<rhs.mOrigin<<" "<<rhs.mDir<<std::endl;
        return os;
    }


    /**
    * @brief Initializes the frustum planes with the ones passed as parameter.
    * @param left     The left plane of the frustum.
    * @param right    The right plane of the frustum.
    * @param bottom   The bottom plane of the frustum.
    * @param top      The top plane of the frustum.
    * @param near     The near plane of the frustum.
    * @param far      The far plane of the frustum.
    */
    frustum::frustum(const plane & left, const plane & right,
                     const plane & bottom, const plane & top,
                     const plane & near, const plane & far)
    {
        mPlanes[0] = left;
        mPlanes[1] = right;
        mPlanes[2] = bottom;
        mPlanes[3] = top;
        mPlanes[4] = near;
        mPlanes[5] = far;
    }


    /**
    * @brief Overload of the input operator to be able to read "frustum"s
    *        (six planes) from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The frustum object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, frustum & rhs)
    {
        for(int i = 0; i < 6; ++i)
        {
            is>>rhs.mPlanes[i];
            is.ignore();
        }

        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write "ray"s
    *        (origin and a direction) to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The ray object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const frustum & rhs)
    {
        for(int i = 0; i < 6; ++i)
            os<<rhs.mPlanes[i];

        return os;
    }


    /**
    * @brief Overload of the input operator to be able to read classification_t
    *        objects from file.
    * @param is         The input stream from which we will read.
    * @param rhs        The classification_t object we will write to.
    * @return std::istream&     The input stream passed as parameter.
    */
    std::istream & operator>>(std::istream & is, classification_t & rhs)
    {
        // Read into a temporary string
        std::string temp;
        is>>temp;

        // Set the enum class according to what was read from file
        if (temp == "inside")
            rhs = classification_t::inside;
        else if (temp == "outside")
            rhs = classification_t::outside;
        else
            rhs = classification_t::overlapping;
        
        return is;
    }


    /**
    * @brief Overload of the output operator to be able to write classification_t
    *        objects to std::cout.
    * @param os         The output stream to which we will write.
    * @param rhs        The classification_t object we will read from.
    * @return std::ostream&     The output stream passed as parameter.
    */
    std::ostream & operator<<(std::ostream & os, const classification_t & rhs)
    {
        if (rhs == classification_t::inside)
            os<<"inside\n";
        else if (rhs == classification_t::outside)
            os<<"outside\n";
        else
            os<<"overlapping\n";
        
        return os;
    }


    /**
    * @brief Finds and returns the point in the plane that is
    *        closest to the point passed as parameter.
    * @param point      The point outside the plane.
    * @param plane      The object representing the plane (has point and normal).
    * @return glm::vec3 The projected point in the plane.
    */
    glm::vec3 closest_point_plane(const glm::vec3 & point, const plane & plane_)
    {
        float planeNormalLengthSq = glm::length2(plane_.mNormal);
        assert(!are_equal(planeNormalLengthSq, 0.0f));
        
        return point - (glm::dot((point - plane_.mPoint), plane_.mNormal) / planeNormalLengthSq) * plane_.mNormal;
    }


    /**
    * @brief Finds and returns the smallest segment that joins both segments passed as parameter.
    * @param seg1       The first segment to join.
    * @param seg2       The second segment to join.
    * @return segment   The segment formed by the closest points between seg1 and seg2.
    */
    segment closest_segment_segment(const segment & seg1, const segment & seg2)
    {
        if ((seg1.mPos1 == seg2.mPos1 && seg1.mPos2 == seg2.mPos2) ||
            (seg1.mPos1 == seg2.mPos2 && seg1.mPos2 == seg2.mPos1))
            return segment(seg1.mPos2, seg2.mPos1);

        // Get the direction vector for each segment
        const glm::vec3 & dir1 = seg1.mPos2 - seg1.mPos1;
        const glm::vec3 & dir2 = seg2.mPos2 - seg2.mPos1;

        // Vector from that joins the first endpoint of both segments (from seg2 to seg1)
        const glm::vec3 & startsVec = seg1.mPos1 - seg2.mPos1;
        
        // Length squared of each segment
        float seg1LengthSq = glm::length2(dir1);
        float seg2LengthSq = glm::length2(dir2);

        // Dot product between first endpoints vector and the dir vector of seg2
        float dotStartsDir2 = glm::dot(dir2, startsVec);

        // If both segments are just a point of length, return the segment that joins them
        if (are_equal(seg1LengthSq, 0.0f) && are_equal(seg2LengthSq, 0.0f))
        {
            return segment(seg1.mPos1, seg2.mPos1);
        }

        // Range [0, 1] to be able to find the closest points inside each segment
        float s = 0.0f;
        float t = 0.0f;
        segment result;     // The resulting segment that we will return

        // If only the first segment is just a point
        if (are_equal(seg1LengthSq, 0.0f))
        {
            s = 0.0f;
            t = dotStartsDir2 / seg2LengthSq;
            t = std::clamp(t, 0.0f, 1.0f);
        }
        else
        {
            // Get the dot product between the vector that joins the first
            // endpoint of both segments, and the direction of the first segment
            float dotStartsDir1 = glm::dot(dir1, startsVec);
            
            // Else if only the second segment is just a point
            if (are_equal(seg2LengthSq, 0.0f))
            {
                t = 0.0f;
                s = std::clamp(-dotStartsDir1 / seg1LengthSq, 0.0f, 1.0f);
            }
            // Else if none of them are just a point
            else
            {
                // Get the dot product between both directions
                float dotDirs = glm::dot(dir1, dir2);
                float denominator = seg1LengthSq * seg2LengthSq - dotDirs * dotDirs;

                // Becareful with division by 0
                // Case not parallel
                if (!are_equal(denominator, 0.0f))
                {
                    float numerator = dotDirs * dotStartsDir2 - dotStartsDir1 * seg2LengthSq;
                    float value = numerator / denominator;
                    s = std::clamp(value, 0.0f, 1.0f);
                }
                // Case parallel segments, pick any point in segment
                else
                    s = 0.0f;

                // Compute t
                t = (dotDirs * s + dotStartsDir2) / seg2LengthSq;
                
                // Clamp t and if it had to be clamped, recompute s
                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = std::clamp(-dotStartsDir1 / seg1LengthSq, 0.0f, 1.0f);
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = std::clamp((dotDirs - dotStartsDir1) / seg1LengthSq, 0.0f, 1.0f);
                }
            }    
        }
        
        // Now that we have s and t (clamped), compute the closest points on each segment
        // and add them to the result segment
        result.mPos1 = seg1.mPos1 + s * dir1;
        result.mPos2 = seg2.mPos1 + t * dir2;

        return result;
    }


    /**
    * @brief Returns true if point is inside sphere, false otherwise.
    * @param point     The point to check intersection with the sphere.
    * @param sphere    The sphere to check if point is inside.
    * @return bool     True if they intersect, false otherwise.
    */
    bool intersection_point_sphere(const glm::vec3 & point, const sphere & sphere_)
    {
        return glm::length2(point - sphere_.mCenter) <= (sphere_.mRadius * sphere_.mRadius);
    }


    /**
    * @brief Returns a classification_t object specifying whether the point passed as
    *        parameter is inside, outside, or overlapping the plane passed as parameter.
    * @param plane_             The plane with respect to which point is classified.
    * @param point              The point that is classified with respect to plane_.
    * @param plane_thickness    The thickness of plane_.
    * @return classification_t  An object containing the information of whether point is
    *                           insise, outside, or overlapping plane_.
    */
    classification_t classify_plane_point(const plane & plane_, const glm::vec3 & point, float plane_thickness)
    {
        float planeNormalLengthSq = glm::length2(plane_.mNormal);
        assert(!are_equal(planeNormalLengthSq, 0.0f));

        // Project the plane point to point vector onto the normal of the plane
        const glm::vec3 & pointVec = point - plane_.mPoint;
        const glm::vec3 & projVecOnNormal = (glm::dot(pointVec, plane_.mNormal) / planeNormalLengthSq) * plane_.mNormal;
        
        // Compute the length squared of the projected vector, and the square of half the plane thickness
        float projVecLengthSq = glm::length2(projVecOnNormal);
        float thicknessSq = plane_thickness * plane_thickness;

        // It is overlapping if the projection length squared is less than the half thickness squared
        if (projVecLengthSq <= thicknessSq)
            return classification_t::overlapping;

        // Do a dot product with the projection and the normal, to check whether it is inside or outside
        float dotResult = glm::dot(projVecOnNormal, plane_.mNormal);
        return dotResult < 0 ? classification_t::inside : classification_t::outside;
    }


    /**
    * @brief Returns a classification_t object specifying whether the triangle passed as
    *        parameter is inside, outside, or overlapping the plane passed as parameter.
    * @param plane_             The plane with respect to which triangle_ is classified.
    * @param triangle_          The triangle that is classified with respect to plane_.
    * @param plane_thickness    The thickness of plane_.
    * @return classification_t  An object containing the information of whether triangle_ is
    *                           insise, outside, or overlapping plane_.
    */
    classification_t classify_plane_triangle(const plane & plane_, const triangle & triangle_, float plane_thickness)
    {
        // Get the classification of each vertex of the triangle with respect to the plane
        classification_t results[3];
        results[0] = classify_plane_point(plane_, triangle_.mPos1, plane_thickness);
        results[1] = classify_plane_point(plane_, triangle_.mPos2, plane_thickness);
        results[2] = classify_plane_point(plane_, triangle_.mPos3, plane_thickness);

        // If they all have the same classification, then their classification is the one we are looking for
        if (results[0] == results[1] && results[0] == results[2])
            return results[0];

        // Inside at index 0, outside at index 1, overlapping at index 2
        int counters[3] = {0,0,0};
        ++counters[static_cast<int>(results[0])];
        ++counters[static_cast<int>(results[1])];
        ++counters[static_cast<int>(results[2])];

        // If we have at least one inside, and one outside, then they are overlapping
        if(counters[0] > 0 && counters[1] > 0)
            return classification_t::overlapping;
        // If we have at least one inside, but no outside, then it is inside
        else if (counters[0] > 0 && counters[1] == 0)
            return classification_t::inside;

        // Otherwise (at least one outside but no inside), it is outside
        return classification_t::outside;
    }


    /**
    * @brief Returns a classification_t object specifying whether the aabb passed as
    *        parameter is inside, outside, or overlapping the plane passed as parameter.
    * @param plane_             The plane with respect to which box is classified.
    * @param box                The aabb that is classified with respect to plane_.
    * @param epsilon            The epsilon to use as plane thickness.
    * @return classification_t  An object containing the information of whether box is
    *                           insise, outside, or overlapping plane_.
    */
    classification_t classify_plane_aabb(const plane & plane_, const aabb & box, float epsilon)
    {
        // Compute the center and the half extent of the box
        const glm::vec3 & center = (box.mMaxPos + box.mMinPos) / 2.0f;
        const glm::vec3 & halfExtent = box.mMaxPos - center;

        float r = halfExtent.x * glm::abs(plane_.mNormal.x) +
                  halfExtent.y * glm::abs(plane_.mNormal.y) +
                  halfExtent.z * glm::abs(plane_.mNormal.z);

        float dist = glm::dot(plane_.mNormal, center) - glm::dot(plane_.mPoint, plane_.mNormal);

        if (glm::abs(dist) <= r)
            return classification_t::overlapping;
        
        // If the vector from plane point to cube center has more
        // than 90deg with the normal, then it is inside
        if (glm::dot(center - plane_.mPoint, plane_.mNormal) < 0)
            return classification_t::inside;

        // Otherwise, it is outside
        return classification_t::outside;
    }


    /**
    * @brief Returns a classification_t object specifying whether the sphere passed as
    *        parameter is inside, outside, or overlapping the plane passed as parameter.
    * @param plane_             The plane with respect to which sphere is classified.
    * @param sphere_            The sphere that is classified with respect to plane_.
    * @param epsilon            The epsilon to use as plane thickness.
    * @return classification_t  An object containing the information of whether sphere_ is
    *                           insise, outside, or overlapping plane_.
    */
    classification_t classify_plane_sphere(const plane & plane_, const sphere & sphere_, float epsilon)
    {
        // Get the classification of the center of the sphere with respect to the plane
        const classification_t & centerClassif = classify_plane_point(plane_, sphere_.mCenter, epsilon);

        // If the center is overlapping, then the sphere is overlapping
        if (centerClassif == classification_t::overlapping)
            return classification_t::overlapping;

        // Get the ditance squared from the center of the sphere to the plane
        float distSq = glm::length2(sphere_.mCenter - closest_point_plane(sphere_.mCenter, plane_));
        float radiusSq = sphere_.mRadius * sphere_.mRadius;
        float thicknessSq = epsilon * epsilon;

        // If the distance from the center of the sphere to the plane is smaller than the radius, overlapping
        if (distSq - thicknessSq <= radiusSq)
            return classification_t::overlapping;

        // Otherwise, the classification of the center is the classification of the sphere
        return centerClassif;
    }


    /**
    * @brief Returns true if the two spheres passed as parameteres intersect.
    * @param sphere1    One of the spheres to check intersection.
    * @param sphere2    The other sphere to check intersection.
    * @return bool     True if they intersect, false otherwise.
    */
    bool intersection_sphere_sphere(const sphere & sphere1, const sphere & sphere2)
    {
        sphere tempSphere(sphere1.mCenter, sphere1.mRadius + sphere2.mRadius);
        return intersection_point_sphere(sphere2.mCenter, tempSphere);
    }


    /**
    * @brief Returns true if the point passed as parameter is inside the aabb passed as parameter.
    * @param point    The point to check intersection.
    * @param aabb     The aabb to check intersection.
    * @return bool    True if they intersect, false otherwise.
    */
    bool intersection_point_aabb(const glm::vec3 & point, const aabb & aabb_)
    {
        // If they don't overlap on x axis
        if (point.x < aabb_.mMinPos.x || point.x > aabb_.mMaxPos.x)
            return false;

        // If they don't overlap on y axis
        if (point.y < aabb_.mMinPos.y || point.y > aabb_.mMaxPos.y)
            return false;

        // If they don't overlap on z axis
        if (point.z < aabb_.mMinPos.z || point.z > aabb_.mMaxPos.z)
            return false;

        // At this point, we know they are intersecting
        return true;
    }


    /**
    * @brief Returns true if the aabbs passed as parameter are intersecting.
    * @param box1     One of the aabbs to check intersection with.
    * @param box2     The other aabb to check intersection with.
    * @return bool    True if they intersect, false otherwise.
    */
    bool intersection_aabb_aabb(const aabb & box1, const aabb & box2)
    {
        // If they don't overlap on x axis
        if (box2.mMinPos.x > box1.mMaxPos.x || box2.mMaxPos.x < box1.mMinPos.x)
            return false;

        // If they don't overlap on y axis
        if (box2.mMinPos.y > box1.mMaxPos.y || box2.mMaxPos.y < box1.mMinPos.y)
            return false;

        // If they don't overlap on z axis
        if (box2.mMinPos.z > box1.mMaxPos.z || box2.mMaxPos.z < box1.mMinPos.z)
            return false;

        // At this point, we know they are intersecting
        return true;
    }


    /**
    * @brief Returns the t parameter of the equation O + tv, being O the origin
    *        of the ray, and v its direction. Returns -1 if the ray is parallel
    *        to the plane, or the intersection obtained is a false positive
    *        (intersection but in the opposite direction of the ray).
    * @param ray_       The ray object we will find its intersection with the plane.
    * @param plane_     The plane object whose intersection with the ray we will find.
    * @return float     The t parameter. Positive for a valid t, -1 for an invalid
    *                   value (parallel or false positive).
    */
    float intersection_ray_plane(const ray & ray_, const plane & plane_)
    {
        float dirNormalDot = glm::dot(ray_.mDir, plane_.mNormal);

        // If the ray is parallel to the plane, they will never intersect (avoid division by 0)
        if (dirNormalDot <= 0 + cEpsilon && dirNormalDot >= 0 - cEpsilon)
            return -1.0f;
        
        float pointNormalDot = glm::dot(plane_.mPoint, plane_.mNormal);
        float originNormalDot = glm::dot(ray_.mOrigin, plane_.mNormal);

        float result = (pointNormalDot - originNormalDot) / dirNormalDot;

        return result < 0 ? -1.0f : result;
    }


    /**
    * @brief Returns the t parameter of the equation O + tv (intersection with other primitive),
    *        being O the origin of the ray, and v its direction. Returns -1 if the ray doesn't
    *        intersect the aabb or is a false positive.
    * @param ray_       The ray object we will find its intersection with the aabb.
    * @param box        The aabb object whose intersection with the ray we will find.
    * @return float     The t parameter (minimum one). Positive for a valid t, -1 for an invalid
    *                   value.
    */
    float intersection_ray_aabb(const ray & ray_, const aabb & box)
    {
        float tMin = 0.0f;
        float tMax = FLT_MAX;

        // For each slab
        for (int i = 0; i < 3; ++i)
        {
            // If the ray's dir is parallel to the current slab (avoid division by 0)
            if (are_equal(ray_.mDir[i], 0.0f))
            {
                // If the ray is parallel and not inside this slab, then there is no ray intersection
                if (ray_.mOrigin[i] < box.mMinPos[i] || ray_.mOrigin[i] > box.mMaxPos[i])
                    return -1.0f;
            }
            // If the ray is not parallel to the current slab
            else
            {
                float tEnter = (box.mMinPos[i] - ray_.mOrigin[i]) / ray_.mDir[i];   // Intersection t with first slab
                float tExit = (box.mMaxPos[i] - ray_.mOrigin[i]) / ray_.mDir[i];    // Intersection t with second slab

                // Just in case, make sure tEnter represents the t intersection with first slab of current axis
                if (tEnter > tExit)
                    std::swap(tEnter, tExit);

                // Update the min and max ts
                if (tEnter > tMin)
                    tMin = tEnter;
                if (tExit < tMax)
                    tMax = tExit;

                // And if you reach one axis where this holds, that means that one
                // axis' slab doesn't intersect, so the ray doesn't intersect
                if (tMin > tMax)
                    return -1.0f;
            }
        }

        // Regular intersection, enter and exit
        if (tMin > 0.0f)
            return tMin;

        // Ray starts inside aabb
        return 0.0f;
    }


    /**
    * @brief Returns the t parameter of the equation O + tv (intersection with other primitive),
    *        being O the origin of the ray, and v its direction. Returns -1 if the ray doesn't
    *        intersect the other primitive or is a false positive.
    * @param ray_       The ray object we will find its intersection with the other primitive.
    * @param sphere_    The sphere object whose intersection with the ray we will find.
    * @return float     The t parameter (minimum one). Positive for a valid t, -1 for an invalid
    *                   value.
    */
    float intersection_ray_sphere(const ray & ray_, const sphere & sphere_)
    {
        // Compute all the necessary variables for the t formula
        const glm::vec3 & originToCenter = ray_.mOrigin - sphere_.mCenter;

        float a = glm::length2(ray_.mDir);

        // Just in case, to avoid dividing by zero
        if (are_equal(a, 0.0f))
        {
            std::cout<<"WARNING : Direction vector of the ray is a zero vector.\n";
            return -1;
        }

        float b = 2 * (glm::dot(ray_.mDir, originToCenter));
        float c = glm::length2(originToCenter) - sphere_.mRadius * sphere_.mRadius;

        float insideSqrt = b*b - 4*a*c;

        // If the value inside the sqrt is negative, there is no
        // solution (that is, the ray doesn't intersect)
        if (insideSqrt < 0)
            return -1;

        // Compute both possible t values
        float t1 = (-b + sqrt(insideSqrt)) / (2*a);
        float t2 = (-b - sqrt(insideSqrt)) / (2*a);

        // One solution, tangent intersection
        if (are_equal(t1, t2))
        {
            if (t1 >= 0)
                return t1;
            else
                return -1;
        }
        
        // Two solutions

        if (t1 < 0)
        {
            if(t2 < 0)
                return -1;      // Both of them false positive
            else
                return 0.0f;    // t1 false positive, t2 real solution (return 0 as ray starts inside the sphere)
        }
        else if (t2 < 0)
            return 0.0f;        // t2 false positive, t1 real solution (return 0 as ray start inside the sphere)
        
        // Both of them real solution, need to return minimum time (first intersection)
        return t1 < t2 ? t1 : t2;
    }


    /**
    * @brief Returns the t parameter of the equation O + tv (intersection with other primitive),
    *        being O the origin of the ray, and v its direction. Returns -1 if the ray doesn't
    *        intersect the other primitive or is a false positive.
    * @param ray_       The ray object we will find its intersection with the other primitive.
    * @param tri        The triangle object whose intersection with the ray we will find.
    * @return float     The t parameter (minimum one). Positive for a valid t, -1 for an invalid
    *                   value.
    */
    float intersection_ray_triangle(const ray & ray_, const triangle & tri)
    {
        const glm::vec3 & normal = glm::cross(tri.mPos2 - tri.mPos1, tri.mPos3 - tri.mPos1);
        float rayPlane_t = intersection_ray_plane(ray_, plane(tri.mPos1, normal));

        // If false positive or paralell
        if (rayPlane_t < 0)
            return -1.0f;

        const glm::vec3 & intersection = ray_.mOrigin + rayPlane_t * ray_.mDir;
        
        if (intersection_point_triangle(intersection, tri))
            return rayPlane_t;

        return -1.0f;
    }


    /**
    * @brief Checks containment of point in the triangle tri.
    * @param point      The point to check containment with.
    * @param tri        The triangle to check containment in.
    * @return bool      True if point is contained in tri, false otherwise.
    */
    bool intersection_point_triangle(const glm::vec3 & point, const triangle & tri)
    {
        // Compute the vectors from the point to each of the vertices of the triangle
        const glm::vec3 & pToA = tri.mPos1 - point;
        const glm::vec3 & pToB = tri.mPos2 - point;
        const glm::vec3 & pToC = tri.mPos3 - point;

        // Compute two of the cross products
        const glm::vec3 & cross1 = glm::cross(pToA, pToB);
        const glm::vec3 & cross2 = glm::cross(pToB, pToC);

        // If the resulting vectors have different directions, then the point is outside
        if (glm::dot(cross1, cross2) < 0.0f)
            return false;

        // Otherwise, compute the last cross product and check to see
        // if all the resulting vectors have the same direction
        const glm::vec3 & cross3 = glm::cross(pToC, pToA);

        if (glm::dot(cross1, cross3) < 0.0f)
            return false;

        // Otherwise, the point is contained in the triangle
        return true;
    }


    /**
    * @brief Returns a classification_t object specifying whether the sphere passed as
    *        parameter is inside, outside, or overlapping the frustum passed as parameter.
    * @param frustum_           The frustum with respect to which sphere_ is classified.
    * @param sphere_            The sphere that is classified with respect to frustum_.
    * @return classification_t  An object containing the information of whether sphere_ is
    *                           insise, outside, or overlapping frustum_.
    */
    classification_t classify_frustum_sphere_naive(const frustum & frustum_, const sphere & sphere_)
    {
        // Store the number of occurrences of each classification between
        // each of the planes of the frustum, and the sphere

        // Index 0 for inside, index 1 for outside, index 2 for overlapping
        int counters[3] = {0,0,0};
        for(int i = 0; i < 6; ++i)
            ++counters[static_cast<int>(classify_plane_sphere(frustum_.mPlanes[i], sphere_, cEpsilon))];
        
        // Outside if outside at least one plane is outside
        if (counters[1] > 0)
            return classification_t::outside;
        // Only inside if inside all planes
        else if (counters[0] == 6)
            return classification_t::inside;

        // Otherwise, it is overlapping
        return classification_t::overlapping;
    }


    /**
    * @brief Returns a classification_t object specifying whether the aabb passed as
    *        parameter is inside, outside, or overlapping the frustum passed as parameter.
    * @param frustum_           The frustum with respect to which box is classified.
    * @param box                The aabb that is classified with respect to frustum_.
    * @return classification_t  An object containing the information of whether box is
    *                           insise, outside, or overlapping frustum_.
    */
    classification_t classify_frustum_aabb_naive(const frustum & frustum_, const aabb & box)
    {
        // Store the number of occurrences of each classification between
        // each of the planes of the frustum, and the aabb

        // Index 0 for inside, index 1 for outside, index 2 for overlapping
        int counters[3] = {0,0,0};
        for(int i = 0; i < 6; ++i)
            ++counters[static_cast<int>(classify_plane_aabb(frustum_.mPlanes[i], box, cEpsilon))];
        
        // Outside if outside at least one plane is outside
        if (counters[1] > 0)
            return classification_t::outside;
        // Only inside if inside all planes
        else if (counters[0] == 6)
            return classification_t::inside;

        // Otherwise, it is overlapping
        return classification_t::overlapping;
    }
}