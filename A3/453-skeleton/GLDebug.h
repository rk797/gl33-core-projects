#pragma once

#include <glad/glad.h>
#include <vector>
#include <unordered_set> // Added implicitly if using set in cpp, but good to be explicit

//------------------------------------------------------------------------------
// OpenGL has a mechanism where you can turn on debug mode and it will tell you
// all sorts of fun stuff about what you're doing.
//
// We are going to use it (best we can) to give you advanced warning of when you
// are doing something incorrectly.
//------------------------------------------------------------------------------


namespace GLDebug {
	// Says which OpenGL message IDs to ignore; useful if some of them are
	// flooding your output with repeat messages.
    void configureIgnoreIds(const std::vector<GLuint>& ids);
	
	void debugOutputHandler(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei,
		const GLchar *message,
		const void *
	);

	void enable();
}
