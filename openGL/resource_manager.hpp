//
//  resource_manager.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef resource_manager_hpp
#define resource_manager_hpp

// Library Includes
#include <OpenGL/gl3.h>

// System Includes
#include <map>
#include <string>

// Local Includes
#include "shader.hpp"
#include "texture.hpp"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager {
public:
	// Resource storage
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture2D> textures;
	
	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader load_shader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
	
	// Retrieves a stored sader
	static Shader get_shader(std::string name);
	
	// Loads (and generates) a texture from file
	static Texture2D load_texture(const char *file, bool alpha, std::string name);
	
	// Retrieves a stored texture
	static Texture2D get_texture(std::string name);
	
	// Properly de-allocates all loaded resources
	static void clear();

private:
	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	ResourceManager() {}
	
	// Loads and generates a shader from file
	static Shader load_shader_from_file(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
	
	// Loads a single texture from file
	static Texture2D load_texture_from_file(const char *file, bool alpha);
};

#endif /* resource_manager_hpp */
