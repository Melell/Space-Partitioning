/**
* @file renderer.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/18/09
* @brief Contains the renderer class that will be used for
*		 all assignments for OpenGL graphics.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once

#include "math.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "geometry.hpp"
#include "mesh.hpp"

namespace cs350{

	class shader;

	// Callback function for GLFW errors.
	void glfw_error_callback(int errorCode, const char * description);

	// We have disabled resizing of the window by the user, but just in case I change it in the future,
	// set a callback if the framebuffer size changes.
	void framebuffer_resize_callback(GLFWwindow * window, int newWidth, int newHeight);


	/**
	 * Contains all the necessary information for the assignment. It's a Singleton
	 *
	 * Keeps track of the needed systems.
	 */
	class renderer {
	private:

		struct demo_resources
		{
			struct demo_meshes
			{
				mesh quad;
				mesh triangle;
				mesh cube;
				mesh sphere;
				mesh bunny;
				mesh gourd;
				mesh sponza;
			};

			struct demo_shaders
			{
				shader * color;
				shader * phong;
			};

			demo_meshes meshes;
			demo_shaders shaders;
		};

		window mWindow;					// The only window that we will use for gfx assignments
		camera mCamera;					// For now, only one camera
		demo_resources mResources;		// Store all the meshes and shaders inside a resources struct

	public:
		static renderer& instance()
		{
			static renderer inst;
			return inst;
		}
		~renderer() {destroy();}

		// Core features
		void create(unsigned w, unsigned h, const char* title, bool hidden);
		void destroy();
		void load_resources();
		void destroy_resources();

		cs350::window & get_window();
		cs350::camera & get_camera();
		cs350::shader * get_shader();
		demo_resources & resources();

	private:
		renderer() = default;
		renderer(const renderer& rhs) = delete;
		renderer& operator=(const renderer& rhs) = delete;

		void initialize_imgui() const;
		void terminate_imgui() const;
	};
}
