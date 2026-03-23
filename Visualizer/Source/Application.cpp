#include "Application.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace app
{
	namespace
	{
		GLFWwindow* g_window{ nullptr };
		ImGuiLayerStack g_UILayerStack;

		bool init_window(u32 width, u32 height, const char* title)
		{
			if (!glfwInit())
			{
				pathx::logger::log("Failed to initialize GLFW",
					pathx::LoggerLevel::Error);
				return false;
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			g_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
			if (!g_window)
			{
				pathx::logger::log("Failed to create GLFW window",
					pathx::LoggerLevel::Error);
				glfwTerminate();
				return false;
			}

			glfwMakeContextCurrent(g_window);
			glfwSwapInterval(1);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				pathx::logger::log("Failed to initialize GLAD",
					pathx::LoggerLevel::Error);
				return false;
			}

			return true;
		}

		void shutdown_window()
		{
			glfwDestroyWindow(g_window);
			glfwTerminate();
		}

		bool init_ui()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io{ ImGui::GetIO() };
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			ImGui::StyleColorsDark();
			ImGuiStyle& style{ ImGui::GetStyle() };
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			if (!ImGui_ImplGlfw_InitForOpenGL(g_window, true))
				return false;

			if (!ImGui_ImplOpenGL3_Init("#version 460"))
				return false;

			return true;
		}

		void ui_begin_frame()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void ui_end_frame()
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			ImGuiIO& io{ ImGui::GetIO() };
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup{ glfwGetCurrentContext() };
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup);
			}
		}

		void shutdown_ui()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	} // anonymous

	bool init(u32 width, u32 height, const char* title)
	{
		if (g_window) return false;

		if (!init_window(width, height, title)) return false;
		if (!init_ui()) return false;

		return true;
	}

	void run()
	{
		assert(g_window);

		while (!glfwWindowShouldClose(g_window))
		{
			glfwPollEvents();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ui_begin_frame();
			g_UILayerStack.render();
			ui_end_frame();

			glfwSwapBuffers(g_window);
		}
	}

	void shutdown()
	{
		assert(g_window);

		shutdown_ui();
		shutdown_window();
	}

	void push_ui_layer(ImGuiLayer* layer)
	{
		g_UILayerStack.push_layer(layer);
	}

	void pop_ui_layer(ImGuiLayer* layer)
	{
		g_UILayerStack.pop_layer(layer);
	}
} // app