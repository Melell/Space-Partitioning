/**
* @file opengl.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the implementation for the opengl debug callback function
*		 and some screenshot taking/saving functions.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "opengl.hpp"
#include <lodepng.h>

namespace{
	/**
	 * OpenGL callback for debugging. Source: https://learnopengl.com/In-Practice/Debugging
	 * @param source
	 * @param type
	 * @param id
	 * @param severity
	 * @param length
	 * @param message
	 * @param userParam
	 */
	void APIENTRY openglCallbackFunction(GLenum source,
										 GLenum type,
										 GLuint id,
										 GLenum severity,
										 GLsizei /* length */,
										 const GLchar* message,
										 const void* /* userParam */)
	{
		if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

    	std::cout << "---------------" << std::endl;
    	std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    	switch (source)
    	{
    	    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    	    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    	    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    	    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    	    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    	    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    	} std::cout << std::endl;

    	switch (type)
    	{
    	    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    	    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    	    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
    	    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    	    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    	    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    	    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    	    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    	    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    	} std::cout << std::endl;
	
    	switch (severity)
    	{
    	    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    	    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    	    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    	    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    	} std::cout << std::endl;
    	std::cout << std::endl;
	}
}

namespace cs350{
	/**
	 * Enables OpenGL callbacks. This callbacks will be useful to intercept errors.
	 */
	void enable_gl_callbacks()
	{
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    	{
    	    glEnable(GL_DEBUG_OUTPUT);
    	    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // To be able to put breakpoints
    	    glDebugMessageCallback(openglCallbackFunction, nullptr);
    	}
		else
			std::cout<<"GL callbacks are not supported\n";
		
	}

	/**
	 * Takes an screenshot
	 * @param width
	 * @param height
	 * @return
	 */
	std::vector<glm::vec<4, unsigned char>> take_screenshoot(unsigned width, unsigned height)
	{
		std::vector<glm::vec<4, unsigned char>> result;
		result.resize(width * height, {});
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, result.data());
		// Flip vertically (inefficiently)
		for (unsigned x = 0; x < width; ++x){
			for (unsigned y = 0; y < height / 2; ++y){
				std::swap(result.at(y * width + x), result.at((height - 1 - y) * width + x));
			}
		}
		return result;
	}

	/**
	 * Saves an screenshoot to a BMP
	 * @param width
	 * @param height
	 * @param filename
	 * @return
	 */
	std::vector<glm::vec<4, unsigned char>> save_screenshoot(unsigned width, unsigned height, std::string const& filename)
	{
		auto pixels = take_screenshoot(width, height);

		auto data = &(pixels.front()[0]);
		auto err  = lodepng::encode(filename, data, width, height);
		if (err){
			std::cerr << "Lodepng failed to save file " << filename;
			exit(-1);
		}		
		return pixels;
	}
}