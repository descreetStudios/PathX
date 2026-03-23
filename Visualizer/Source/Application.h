#pragma once

#include <PathXAPI/Core.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ImGuiLayerStack.h"

namespace app
{
	bool init(u32 width, u32 height, const char* title);
	void run();
	void shutdown();

	void push_ui_layer(ImGuiLayer* layer);
	void pop_ui_layer(ImGuiLayer* layer);
} // app