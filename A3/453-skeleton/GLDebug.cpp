#include "GLDebug.h"
#include "Log.h"

#include <regex>

// We use a global, unordered set for storing IDs, but we set it to "static" so
// that it is not visible outside of this namespace.
static std::unordered_set<GLuint> g_ignoredIds;

void GLDebug::configureIgnoreIds(const std::vector<GLuint>& idsToIgnore) {
    g_ignoredIds.clear();
    for (auto id : idsToIgnore) {
        g_ignoredIds.insert(id);
    }
}

void GLDebug::debugOutputHandler(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei,
	const GLchar *message,
	const void *
) {
    // Commented-out, hard-coded ignoring of non-significant error/warning codes
    //if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    
    // If the ID is in our ignore set, discard the message immediately
    if (g_ignoredIds.find(id) != g_ignoredIds.end()) {
        return;
    }
    
    std::string sourceStr, typeStr, severityStr;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               typeStr = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               typeStr = "Other"; break;
    }

    std::string format = "[OPENGL] [{}] {} #{} -- {}: {}";
    std::string message_str = message;
    message_str = std::regex_replace(message_str, std::regex("^\\s+|\\s+$"), "$1");
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            severityStr = "high";
			Log::error(format.c_str(), sourceStr, severityStr, id, typeStr, message_str);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severityStr = "medium";
			Log::warn(format.c_str(), sourceStr, severityStr, id, typeStr, message_str);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severityStr = "low";
			Log::info(format.c_str(), sourceStr, severityStr, id, typeStr, message_str);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
			Log::debug(format.c_str(), sourceStr, severityStr, id, typeStr, message_str);
            break;
    }
}

void GLDebug::enable() {
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// initialize debug output
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebug::debugOutputHandler, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		Log::info("Enabling debug mode for opengl");
	} else {
		Log::warn("Unable to enable debug mode for opengl");
	}
}

