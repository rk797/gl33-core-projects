#include "Texture.h"
#include "GLHandles.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

Texture::Texture(std::string path, GLint interpolation, bool sourceIsSRGB)
	: textureID(), path(path), interpolation(interpolation), sourceIsSRGB(sourceIsSRGB)
{
	int numComponents;
	stbi_set_flip_vertically_on_load(true);
	const char* pathData = path.c_str();
	unsigned char* data = stbi_load(pathData, &width, &height, &numComponents, 0);
	if (data != nullptr)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//Set alignment to be 1

		bind();

		//Set number of components by format of the texture
		GLuint format = GL_RGB;
		GLuint internalFormat = GL_SRGB;
		switch (numComponents)
		{
		case 4:
			format = internalFormat = GL_RGBA;
			if (sourceIsSRGB) internalFormat = GL_SRGB_ALPHA;
			break;
		case 3:
			format = internalFormat = GL_RGB;
			if (sourceIsSRGB) internalFormat = GL_SRGB;
			break;
		case 2:
			format = internalFormat = GL_RG;
			break;
		case 1:
			format = internalFormat = GL_RED;
			break;
		default:
			std::cout << "Invalid Texture Format" << std::endl;
			break;
		};
		//Loads texture data into bound texture
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

		// Clean up
		unbind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	//Return to default alignment
		stbi_image_free(data);

	}
	else {
		throw std::runtime_error("Failed to read texture data from file!");
	}
}
