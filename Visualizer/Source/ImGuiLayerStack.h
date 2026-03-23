#pragma once

#include <EngineCommon.h>
#include "ImGuiLayer.h"

class ImGuiLayerStack
{
public:
	void push_layer(ImGuiLayer* layer)
	{
		m_layers.push_back(layer);
		layer->on_attach();
	}

	void pop_layer(ImGuiLayer* layer)
	{
		auto it{ std::find(m_layers.begin(),
			m_layers.end(), layer) };
		if (it != m_layers.end())
		{
			(*it)->on_detach();
			m_layers.erase(it);
		}
	}

	void render()
	{
		for (auto* layer : m_layers)
			layer->on_imgui_render();
	}

private:
	Vector<ImGuiLayer*> m_layers;
};