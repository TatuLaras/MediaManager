#pragma once
#include "Metadata.h"
#include "imgui.h"
#include "TMDB.h"
#include "FsHelpers.h"
#include "Config.h"
#include "PathManager.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


/// <summary>
/// Interface for info panels
/// </summary>
class InfoPanel
{
public:
	virtual void Render() = 0;
};
