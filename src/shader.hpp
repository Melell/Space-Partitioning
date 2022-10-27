/**
* @file shader.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the declaration of the shader class, used to create,
*        compile and link shader files into a shader program that can be used.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "math.hpp"
#include "opengl.hpp"
#include <string>

namespace cs350 {

  namespace GLSLShader
  {
    enum GLSLShaderType
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESS_CONTROL,
        TESS_EVALUATION,
        COMPUTE
    };
  }

  /*
   * @brief   Class encapsulating a GLSL Program
   *
   * @detail  Class encapsulating a GLSL Program that also adds some member functions to set uniform variables.
   * This class was mostly copied from the Cookbook code.
   */
  class shader
  {
    public:
      static shader * CreateShaderProgram(const std::string & vertShader,
                                               const std::string & fragShader);

      shader();
      ~shader();

      void delete_program();

      bool CompileShaderFromFile(const char * fileName, GLSLShader::GLSLShaderType type);
      bool CompileShaderFromString(const std::string & source, GLSLShader::GLSLShaderType type);
      bool Link();
      bool Validate();
      void Use() const;

      std::string Log() const;

      int  GetHandle() const;
      bool IsLinked() const;

      void BindAttribLocation(GLuint location, const char * name);
      void BindFragDataLocation(GLuint location, const char * name);

      void SetUniform(const std::string & name, float x, float y, float z) const;
      void SetUniform(const std::string & name, const glm::vec2 & v) const;
      void SetUniform(const std::string & name, const glm::vec3 & v) const;
      void SetUniform(const std::string & name, const glm::vec4 & v) const;
      void SetUniform(const std::string & name, const glm::mat4 & m) const;
      void SetUniform(const std::string & name, const glm::mat3 & m) const;
      void SetUniform(const std::string & name, float val) const;
      void SetUniform(const std::string & name, int val) const;
      void SetUniform(const std::string & name, bool val) const;
      void SetSubroutineUniform(const std::string & name, const std::string & funcName) const;

      void PrintActiveUniforms() const;
      void PrintActiveAttribs() const;

    private:
      int  GetUniformLocation(const std::string & name) const;

      int         handle_;
      bool        linked_;
      std::string log_string_;
  };
}