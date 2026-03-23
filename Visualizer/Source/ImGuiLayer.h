#pragma once

class ImGuiLayer
{
public:
	ImGuiLayer() = default;
	virtual ~ImGuiLayer() = default;

	virtual void on_attach() {}
	virtual void on_detach() {}
	virtual void on_imgui_render() {}
};