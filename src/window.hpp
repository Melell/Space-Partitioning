/**
* @file window.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/18/09
* @brief Contains the window class, which is a wrapper around a GLFWwindow
*		 pointer, and it represents the window created for the gfx assignments
*		 and the OpenGL context attached to it.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "math.hpp"

namespace cs350{
	class window {
	private:
		GLFWwindow* m_window = {};
		glm::ivec2 m_size = {};

	public:
		~window() {destroy();};
		bool create(int w, int h, const char* window_name, bool hidden);
		bool update();
		void destroy();

		[[nodiscard]] glm::ivec2 size() const noexcept {return m_size;}
		[[nodiscard]] GLFWwindow* handle() const noexcept {return m_window;}
	};
}