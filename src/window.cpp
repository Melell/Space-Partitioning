/**
* @file window.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/18/09
* @brief Contains the implementation for the window class declared in window.cpp.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "opengl.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "imgui.hpp"

namespace cs350
{
    /**
    * @brief Creates a GLFW window, an OpenGL context, makes it current and attaches it to the window.
    * @param w			    Width of the window to create.
	* @param h			    Height of the window to create.
	* @param window_name	Title of the window to create.
	* @param hidden		    Whether the window should be hidden on creation.
    * @return bool          True if the creation of the window was succesful, false otherwise.
    */
	bool window::create(int w, int h, const char* window_name, bool hidden)
    {
        // Tell glfw which version (4.4) to use for the OpenGL context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

        // Make the context forward compatible and use core profile
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // To enable debug message callbacks
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        // Make the window not resizable for now to simplify the work, and
        // make it invisible on creation if the user specifies it
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, !hidden);

        // Create a new window with an OpenGL context, and make it current

        m_window = glfwCreateWindow(w, h, window_name, nullptr, nullptr);
        if (m_window == nullptr)
        {
            std::cout<<"Failed to create window of dimensions "<<w<<"x"<<h<<" and name "<<window_name<<std::endl;
            return false;
        }

        m_size.x = w;
        m_size.y = h;

        glfwMakeContextCurrent(m_window);

        return true;
    }


    /**
    * @brief Update the window (poll for events, clear and swap buffers etc)
    * @return bool  Returns false if the window should close, true otherwise.
    */
	bool window::update()
    {
        if (!glfwWindowShouldClose(m_window))
        {
            // Process glfw events
            glfwPollEvents();

            // Swap the fron and back buffers
            //glfwSwapBuffers(m_window);

            // Clear the buffer to the clear color
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            return true;
        }

        return false;
    }


    /**
    * @brief Does nothing for now, since the renderer actually destroys the window.
    */
	void window::destroy()
    {

    }
}