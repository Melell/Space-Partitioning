/**
* @file renderer.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/18/09
* @brief Implementation for the renderer class declared in renderer.hpp.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "shader.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "imgui.hpp"

namespace cs350{

	/**
    * @brief Callback function for GLFW errors.
    * @param errorCode		ID or code of the error.
	* @param description	A bigger and more readable description of the error.
    */
	void glfw_error_callback(int errorCode, const char * description)
	{
		std::cout<<"GLFW error "<<errorCode<<": "<<description<<std::endl;
		std::exit(EXIT_FAILURE);
	}

	/**
    * @brief We have disabled resizing of the window by the user, but just in caseI change it
	* 		 in the future, set a callback to change the viewport if the framebuffer size changes.
    * @param window		A pointer to the glfw window whose framebuffer has been resized.
	* @param newWidth	The new height in pixels of window's framebuffer.
	* @param newHeight	The new height in pixels of window's framebuffer.
    */
	void framebuffer_resize_callback(GLFWwindow * window, int newWidth, int newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}


	/**
    * @brief Initializes GLFW, creates a window of the specified dimensions
	* 		 and attaches an OpenGL rendering context to it.
    * @param w			Width of the window to create.
	* @param h			Height of the window to create.
	* @param title		Title of the window to create.
	* @param hidden		Whether the window created should be hidden on creation.
    */
	void renderer::create(unsigned w, unsigned h, const char* title, bool hidden)
	{
		// Set the glfw error callback
		glfwSetErrorCallback(&glfw_error_callback);

		// Initialize glfw
        if (!glfwInit())
		{
			std::cout<<"Failed to initialize GLFW\n";
			std::exit(EXIT_FAILURE);
		}

		// Create the window with a context, and make it current
		if (!mWindow.create(w, h, title, hidden))
		{
			glfwTerminate();
			std::exit(EXIT_FAILURE);	// Debug message is inside create of window
		}

		// Load the OpenGL functions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout<<"Failed to load OpenGL functions\n";
			mWindow.destroy();
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		// Set the OpenGL error callback
		enable_gl_callbacks();

		// Initialize ImGui
		initialize_imgui();

		// If this fails, the window handle has been invalidated for some reason
		GLFWwindow * windowHandle = mWindow.handle();
		assert(windowHandle != nullptr);

		// Set a callback for the resize of the framebuffer (in case I change resize attribute in the future)
		glfwSetFramebufferSizeCallback(windowHandle, &framebuffer_resize_callback);

		// Retrieve the size in pixels of the framebuffer
		int fbWidth = w;
		int fbHeight = h;
		glfwGetFramebufferSize(windowHandle, &fbWidth, &fbHeight);

		// Set the viewport for OpenGL rendering
		glViewport(0, 0, fbWidth, fbHeight);

		// Set the color to clear the framebuffer to
		glClearColor(0.2f, 0.4f, 0.6f, 0.4f);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);

		// Enable culling of backfaces (CCW are front faces)
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		
		// Enable vsync to avoid tearing
		glfwSwapInterval(1);

		load_resources();
	}


	/**
    * @brief Destroys the current window and the context attached to it,
	* 		 releases all resources of the renderer and terminates GLFW.
    */
	void renderer::destroy()
	{
		destroy_resources();

		// Cleanup for ImGui
    	terminate_imgui();

		// Destroy the window here instead of in the destroy of the window class because
		// if so, there would be problems of the window being destroyed twice
		if (mWindow.handle())
			glfwDestroyWindow(mWindow.handle());
		glfwTerminate();
	}


	/**
    * @brief Loads all the resources necessary for the renderer, like for example, the shaders.
	* 		 This function is called
    */
	void renderer::load_resources()
	{
		// Compile and link into a shader program the vertex and fragment shader for uniform color
		mResources.shaders.color = shader::CreateShaderProgram("../resources/shaders/color.vert", "../resources/shaders/color.frag");
		mResources.shaders.phong = shader::CreateShaderProgram("../resources/shaders/phong.vert", "../resources/shaders/phong.frag");

		// Set the shader to be used for now, as we only have one
		mResources.shaders.color->Use();

		// MESHES
		mResources.meshes.quad.setup_mesh(load_obj("../resources/meshes/quad.obj"));
		mResources.meshes.triangle.setup_mesh(load_obj("../resources/meshes/triangle.obj"));
		mResources.meshes.cube.setup_mesh(load_obj("../resources/meshes/cube.obj"));
		mResources.meshes.sphere.setup_mesh(load_obj("../resources/meshes/sphere.obj"));
		mResources.meshes.bunny.setup_mesh(load_obj("../resources/meshes/bunny.obj"));
		mResources.meshes.gourd.setup_mesh(load_obj("../resources/meshes/gourd.obj"));
		//mResources.meshes.sponza.setup_mesh(load_obj("../resources/meshes/sponza.obj"));
	}


	/**
    * @brief Destroys all the resources that were loaded for the renderer, in the function load_resources.
    */
	void renderer::destroy_resources()
	{
		// Destroy the meshes
		mResources.meshes.quad.destroy_resources();
		mResources.meshes.triangle.destroy_resources();
		mResources.meshes.cube.destroy_resources();
		mResources.meshes.sphere.destroy_resources();
		mResources.meshes.bunny.destroy_resources();
		mResources.meshes.gourd.destroy_resources();
		//mResources.meshes.sponza.destroy_resources();

		// Destroy the shader
		mResources.shaders.color->delete_program();
	}


	/**
    * @brief Getter that returns a reference to the internal window object this renderer renders onto.
	* @return window &		A reference to the internal window object.
    */
	cs350::window & renderer::get_window()
	{
		return mWindow;
	}

	/**
    * @brief Getter that returns a reference to the internal camera object of the scene.
	* @return camera &		A reference to the internal camera object.
    */
	cs350::camera & renderer::get_camera()
	{
		return mCamera;
	}


	/**
    * @brief Shortcut for getting the color shader. When we add more shaders this will have to be eliminated.
	* @return shader *		A pointer to the internal shader object.
    */
	cs350::shader * renderer::get_shader()
	{
		return mResources.shaders.color;
	}


	/**
    * @brief Getter that returns a reference to the internal resources object of the scene.
	* 		 This object contains meshes, shaders etc.
	* @return demo_resources &		A reference to the internal resources object.
    */
	renderer::demo_resources & renderer::resources()
	{
		return mResources;
	}


	/**
    * @brief Calls all the necessary imgui initialization functions to enable imgui rendering.
    */
	void renderer::initialize_imgui() const
	{
		// Setup Dear ImGui context
    	IMGUI_CHECKVERSION();
    	ImGui::CreateContext();
    	ImGuiIO& io = ImGui::GetIO(); (void)io;

    	// Setup Dear ImGui style
    	ImGui::StyleColorsDark();

    	// Setup Platform/Renderer bindings
    	ImGui_ImplGlfw_InitForOpenGL(mWindow.handle(), true);
    	ImGui_ImplOpenGL3_Init("#version 440");
	}


	/**
    * @brief Calls all the necessary imgui termination functions.
    */
	void renderer::terminate_imgui() const
	{
		// Cleanup
    	ImGui_ImplOpenGL3_Shutdown();
    	ImGui_ImplGlfw_Shutdown();
    	ImGui::DestroyContext();
	}
}
