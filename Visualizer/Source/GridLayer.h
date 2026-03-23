#pragma once
#include <PathXAPI/Core.h>
#include "ImGuiLayer.h"
#include <imgui.h>

class GridLayer : public ImGuiLayer
{
public:
	GridLayer(f32 margin = 20.0f, f32 targetCellSize = 32.0f)
		: m_margin(margin), m_targetCellSize(targetCellSize)
	{
	}

	void on_imgui_render() override
	{
		begin_window();
		update_dimensions();
		draw_grid();
		draw_route(); // TODO: Link with engine
		ImGui::End();
	}

private:
	f32 m_margin;
	f32 m_targetCellSize;
	u32 m_width{ 0u };
	u32 m_height{ 0u };

	s32 m_startX{ -1 }, m_startY{ -1 };
	s32 m_endX{ -1 }, m_endY{ -1 };

	Vector<Vector<bool>> m_grid;
	Vector<ImVec2> m_route;

private:
	void begin_window()
	{
		ImGuiWindowFlags flags{
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus };

		ImGuiViewport* viewport{ ImGui::GetMainViewport() };

		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("FullscreenGrid", nullptr, flags);

		ImGui::PopStyleVar(3);
	}

	void update_dimensions()
	{
		ImVec2 avail{ ImGui::GetContentRegionAvail() };

		f32 usableW{ avail.x - m_margin * 2.0f };
		f32 usableH{ avail.y - m_margin * 2.0f };

		u32 newWidth{ max(1, (u32)std::floor(usableW / m_targetCellSize)) };
		u32 newHeight{ max(1, (u32)std::floor(usableH / m_targetCellSize)) };

		if (newWidth != m_width || newHeight != m_height)
		{
			Vector<Vector<bool>> newGrid;
			newGrid.resize(newHeight, Vector<bool>(newWidth, false));

			for (u32 y{ 0 }; y < min(m_height, newHeight); y++)
				for (u32 x{ 0 }; x < min(m_width, newWidth); x++)
					newGrid[y][x] = m_grid[y][x];

			m_grid = std::move(newGrid);

			m_width = newWidth;
			m_height = newHeight;

			if (m_startX >= (s32)m_width || m_startY >= (s32)m_height)
				m_startX = m_startY = -1;

			if (m_endX >= (s32)m_width || m_endY >= (s32)m_height)
				m_endX = m_endY = -1;

			compute_route();
		}
	}

	void draw_grid()
	{
		ImVec2 avail{ ImGui::GetContentRegionAvail() };

		f32 usableW{ avail.x - m_margin * 2.0f };
		f32 usableH{ avail.y - m_margin * 2.0f };

		f32 cellSizeX{ usableW / m_width };
		f32 cellSizeY{ usableH / m_height };
		f32 cellSize{ (cellSizeX < cellSizeY) ? cellSizeX : cellSizeY };

		ImDrawList* drawList{ ImGui::GetWindowDrawList() };
		ImVec2 origin{ ImGui::GetCursorScreenPos() };
		origin.x += m_margin;
		origin.y += m_margin;

		m_cellSize = cellSize;
		m_origin = origin;

		for (u32 y{ 0 }; y < m_height; y++)
		{
			for (u32 x{ 0 }; x < m_width; x++)
			{
				ImVec2 cellMin{ origin.x + x * cellSize, origin.y + y * cellSize };
				ImVec2 cellMax{ cellMin.x + cellSize, cellMin.y + cellSize };

				ImGui::SetCursorScreenPos(cellMin);
				ImGui::PushID(y * m_width + x);
				ImGui::InvisibleButton("cell", { cellSize, cellSize });

				bool hovered{ ImGui::IsItemHovered() };
				bool clicked{ ImGui::IsItemClicked() };

				if (clicked)
				{
					bool shift{ ImGui::GetIO().KeyShift };
					bool ctrl{ ImGui::GetIO().KeyCtrl };

					if (shift)
					{
						m_startX = x;
						m_startY = y;
						compute_route();
					}
					else if (ctrl)
					{
						m_endX = x;
						m_endY = y;
						compute_route();
					}
					else
					{
						m_grid[y][x] = !m_grid[y][x];
					}
				}

				ImU32 color{ m_grid[y][x]
					? IM_COL32(60, 60, 60, 255)
					: IM_COL32(200, 200, 200, 255) };

				if ((s32)x == m_startX && (s32)y == m_startY)
					color = IM_COL32(50, 120, 255, 255); // blue

				if ((s32)x == m_endX && (s32)y == m_endY)
					color = IM_COL32(255, 80, 80, 255); // red

				if (hovered)
					color = IM_COL32(150, 150, 255, 255);

				drawList->AddRectFilled(cellMin, cellMax, color);
				drawList->AddRect(cellMin, cellMax, IM_COL32(0, 0, 0, 255));

				ImGui::PopID();
			}
		}

		ImGui::Dummy({ avail.x, avail.y });
	}

	void draw_route()
	{
		if (m_route.size() < 2)
			return;

		ImDrawList* drawList{ ImGui::GetWindowDrawList() };

		for (size_t i = 0; i + 1 < m_route.size(); i++)
		{
			drawList->AddLine(
				m_route[i],
				m_route[i + 1],
				IM_COL32(255, 255, 0, 255), // yellow
				3.0f
			);
		}
	}

	void compute_route()
	{
		m_route.clear();

		if (m_startX < 0 || m_endX < 0)
			return;

		int x0 = m_startX;
		int y0 = m_startY;
		int x1 = m_endX;
		int y1 = m_endY;

		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		int err = dx - dy;

		while (true)
		{
			m_route.push_back(cell_center(x0, y0));

			if (x0 == x1 && y0 == y1)
				break;

			int e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x0 += sx; }
			if (e2 < dx) { err += dx; y0 += sy; }
		}
	}

	ImVec2 cell_center(int x, int y)
	{
		return ImVec2(
			m_origin.x + x * m_cellSize + m_cellSize * 0.5f,
			m_origin.y + y * m_cellSize + m_cellSize * 0.5f
		);
	}

	ImVec2 m_origin;
	f32 m_cellSize{ 0.0f };
};
