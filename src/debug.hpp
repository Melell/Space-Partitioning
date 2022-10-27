/**
* @file debug.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the declaration of functions for debug drawing different primitives.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "math.hpp"
#include "geometry.hpp"

// IO
namespace glm{
    std::istream& operator>>(std::istream& is, vec2& v);
    std::ostream& operator<<(std::ostream& os, vec2 const& p);
	std::istream& operator>>(std::istream& is, vec3& v);
	std::ostream& operator<<(std::ostream& os, vec3 const& p);
}


namespace cs350{

	struct bvh_node;

	enum class debug_draw_type
	{
		fancy,
		wireframe,
		plain_color,
		points
	};

	// Debug drawing
	void debug_draw_point(glm::vec3 pt, glm::vec4 color);
	void debug_draw_points(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_segment(segment const& s, glm::vec4 color);
	void debug_draw_segments(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_triangle(triangle const& t, glm::vec4 color, debug_draw_type draw_type);
	void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale = 1.0f);
	void debug_draw_aabb(aabb const& a, glm::vec4 color, debug_draw_type draw_type);
	void debug_draw_sphere(sphere const& s, glm::vec4 color, debug_draw_type draw_type);
    void debug_draw_sphere_discs(sphere const& s, glm::vec4 color);
    void debug_draw_frustum(frustum const& s, glm::vec4 color);
	void debug_draw_fancy(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_plain_color(mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 const& color);

	void debug_draw_bvh_tree(const std::vector<glm::vec3> & positions,
							bvh_node * tree,
							bool renderBV,
							const glm::vec4 & bvColor,
							const glm::mat4 & m2w,
							const glm::vec4 & color,
							debug_draw_type draw_type = debug_draw_type::fancy);

	void debug_draw(debug_draw_type draw_type, mesh * const& mesh, glm::mat4 const& m2w, glm::vec4 color);
}