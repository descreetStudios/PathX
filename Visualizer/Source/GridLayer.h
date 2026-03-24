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
		draw_route();
		ImGui::End();
	}

private:
	f32 m_margin;
	f32 m_targetCellSize;
	u32 m_width{ 0u };
	u32 m_height{ 0u };

	s32 m_startX{ -1 }, m_startY{ -1 };
	s32 m_endX{ -1 }, m_endY{ -1 };

	bool m_allowDiagonals{ false };

	Vector<Vector<bool>> m_grid;
	Vector<ImVec2>       m_route;
	bool                 m_routeFound{ false };

private:
	using Cell = Pair<s32, s32>;

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

		u32 newWidth{ max(1u, (u32)std::floor(usableW / m_targetCellSize)) };
		u32 newHeight{ max(1u, (u32)std::floor(usableH / m_targetCellSize)) };

		if (newWidth != m_width || newHeight != m_height)
		{
			Vector<Vector<bool>> newGrid(newHeight, Vector<bool>(newWidth, false));

			for (u32 y{ 0u }; y < min(m_height, newHeight); y++)
				for (u32 x{ 0u }; x < min(m_width, newWidth); x++)
					newGrid[y][x] = m_grid[y][x];

			m_grid = std::move(newGrid);
			m_width = newWidth;
			m_height = newHeight;

			if (m_startX >= (s32)m_width || m_startY >= (s32)m_height) m_startX = m_startY = -1;
			if (m_endX >= (s32)m_width || m_endY >= (s32)m_height) m_endX = m_endY = -1;

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

		draw_toolbar();

		for (u32 y{ 0u }; y < m_height; y++)
		{
			for (u32 x{ 0u }; x < m_width; x++)
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
						m_startX = (s32)x;
						m_startY = (s32)y;
						compute_route();
					}
					else if (ctrl)
					{
						m_endX = (s32)x;
						m_endY = (s32)y;
						compute_route();
					}
					else if ((s32)x != m_startX || (s32)y != m_startY)
					{
						if ((s32)x != m_endX || (s32)y != m_endY)
						{
							m_grid[y][x] = !m_grid[y][x];
							compute_route();
						}
					}
				}

				ImU32 color;

				if ((s32)x == m_startX && (s32)y == m_startY)
					color = IM_COL32(50, 120, 255, 255);  // blue  — start
				else if ((s32)x == m_endX && (s32)y == m_endY)
					color = IM_COL32(255, 80, 80, 255);   // red   — end
				else if (m_grid[y][x])
					color = IM_COL32(60, 60, 60, 255);    // dark  — wall
				else
					color = IM_COL32(200, 200, 200, 255); // light — open

				if (hovered && !m_grid[y][x]
					&& ((s32)x != m_startX || (s32)y != m_startY)
					&& ((s32)x != m_endX || (s32)y != m_endY))
					color = IM_COL32(150, 150, 255, 255); // hover tint

				drawList->AddRectFilled(cellMin, cellMax, color);
				drawList->AddRect(cellMin, cellMax, IM_COL32(0, 0, 0, 255));

				ImGui::PopID();
			}
		}

		ImGui::Dummy({ avail.x, avail.y });
	}

	void draw_toolbar()
	{
		ImVec2 toolbarPos{ m_origin.x, m_origin.y - m_margin };
		ImGui::SetCursorScreenPos(toolbarPos);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));

		if (ImGui::Checkbox("Diagonals", &m_allowDiagonals))
			compute_route();

		ImGui::SameLine(0, 16);

		if (m_startX < 0 || m_endX < 0)
			ImGui::TextDisabled("Set start (Shift+click) and end (Ctrl+click)");
		else if (!m_routeFound)
			ImGui::TextColored(ImVec4(1.f, 0.4f, 0.4f, 1.f), "No path found");
		else
			ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f),
				"Path: %zu steps", m_route.size());

		ImGui::SameLine(0, 16);

		if (ImGui::SmallButton("Clear walls"))
		{
			for (auto& row : m_grid)
				std::fill(row.begin(), row.end(), false);
			compute_route();
		}

		ImGui::SameLine(0, 8);

		if (ImGui::SmallButton("Reset"))
		{
			for (auto& row : m_grid)
				std::fill(row.begin(), row.end(), false);
			m_startX = m_startY = m_endX = m_endY = -1;
			m_route.clear();
			m_routeFound = false;
		}

		ImGui::PopStyleVar();
	}

	void draw_route()
	{
		if (m_route.size() < 2)
			return;

		ImDrawList* drawList{ ImGui::GetWindowDrawList() };

		for (size_t i{ 0 }; i + 1 < m_route.size(); i++)
		{
			drawList->AddLine(
				m_route[i],
				m_route[i + 1],
				IM_COL32(255, 220, 0, 255), // yellow
				3.0f);
		}

		// Draw dots at each waypoint
		for (const ImVec2& pt : m_route)
			drawList->AddCircleFilled(pt, 3.5f, IM_COL32(255, 180, 0, 255));
	}

	void compute_route()
	{
		m_route.clear();
		m_routeFound = false;

		if (m_startX < 0 || m_endX < 0)
			return;

		Cell start{ m_startX, m_startY };
		Cell goal{ m_endX,   m_endY };

		// Neighbours: 4-directional or 8-directional
		auto getNeighbours = [this](const Cell& cell) -> Vector<Cell>
			{
				const s32 cx{ cell.first };
				const s32 cy{ cell.second };

				Vector<Cell> neighbours;
				neighbours.reserve(m_allowDiagonals ? 8 : 4);

				constexpr Pair<s32, s32> cardinals[4] = { {0,-1},{0,1},{-1,0},{1,0} };
				constexpr Pair<s32, s32> diagonals[4] = { {-1,-1},{1,-1},{-1,1},{1,1} };

				auto try_add = [&](s32 nx, s32 ny)
					{
						if (nx < 0 || ny < 0
							|| nx >= (s32)m_width
							|| ny >= (s32)m_height
							|| m_grid[ny][nx])
							return;
						neighbours.emplace_back(nx, ny);
					};

				for (auto [dx, dy] : cardinals)
					try_add(cx + dx, cy + dy);

				if (m_allowDiagonals)
					for (auto [dx, dy] : diagonals)
						try_add(cx + dx, cy + dy);

				return neighbours;
			};

		// Cost: 1.0 for cardinals, sqrt(2) for diagonals
		auto getCost = [](const Cell& a, const Cell& b) -> f32
			{
				s32 dx{ abs(a.first - b.first) };
				s32 dy{ abs(a.second - b.second) };
				return (dx + dy == 2) ? 1.41421356f : 1.0f;
			};

		// Heuristic: octile distance (admissible for both 4- and 8-directional)
		auto heuristic = [this](const Cell& a, const Cell& b) -> f32
			{
				f32 dx{ (f32)abs(a.first - b.first) };
				f32 dy{ (f32)abs(a.second - b.second) };

				if (!m_allowDiagonals)
					return dx + dy; // Manhattan

				// Octile
				constexpr f32 sqrt2minus1{ 0.41421356f };
				return (dx > dy)
					? dx + sqrt2minus1 * dy
					: dy + sqrt2minus1 * dx;
			};

		auto result = pathx::astar::search(
			start, goal, getNeighbours, getCost, heuristic);

		if (!result.found)
			return;

		m_routeFound = true;
		m_route.reserve(result.path.size());
		for (const Cell& cell : result.path)
			m_route.push_back(cell_center(cell.first, cell.second));
	}

	ImVec2 cell_center(s32 x, s32 y) const
	{
		return ImVec2(
			m_origin.x + x * m_cellSize + m_cellSize * 0.5f,
			m_origin.y + y * m_cellSize + m_cellSize * 0.5f);
	}

	// Cache
	ImVec2 m_origin;
	f32 m_cellSize{ 0.0f };
};