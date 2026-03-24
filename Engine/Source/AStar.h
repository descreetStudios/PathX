#pragma once

#include <EngineCommon.h>

namespace pathx::astar
{
	template<typename TNode, typename TCost = f32>
	struct Result
	{
		bool found{ false };
		Vector<TNode> path{};
		TCost totalCost{};
	};

	template<
		typename TNode,
		typename TCost = f32,
		typename TNeighbours,
		typename TCostFn,
		typename THeuristic>
	[[nodiscard]]
	Result<TNode, TCost> search(
		const TNode& start,
		const TNode& goal,
		TNeighbours&& getNeighbours,
		TCostFn&& getCost,
		THeuristic&& heuristic)
	{
		using QueueEntry = Pair<TCost, TNode>;
		using MinHeap = std::priority_queue<
			QueueEntry,
			Vector<QueueEntry>,
			std::greater<QueueEntry>>;

		MinHeap openSet;
		Map<TNode, TNode> cameFrom;
		Map<TNode, TCost> gScore;
		Map<TNode, TCost> fScore;

		gScore[start] = TCost{};
		fScore[start] = heuristic(start, goal);
		openSet.emplace(fScore[start], start);

		while (!openSet.empty())
		{
			auto [_, current] = openSet.top();
			openSet.pop();

			if (current == goal)
			{
				Vector<TNode> path;
				TNode node{ goal };
				while (cameFrom.count(node))
				{
					path.push_back(node);
					node = cameFrom.at(node);
				}
				path.push_back(node);
				std::reverse(path.begin(), path.end());
				return { true, std::move(path), gScore[goal] };
			}

			for (const TNode& neighbour : getNeighbours(current))
			{
				TCost tentativeG{ gScore[current] + getCost(current, neighbour) };

				auto it = gScore.find(neighbour);
				if (it != gScore.end() && tentativeG >= it->second)
					continue;

				cameFrom[neighbour] = current;
				gScore[neighbour] = tentativeG;
				fScore[neighbour] = tentativeG + heuristic(neighbour, goal);
				openSet.emplace(fScore[neighbour], neighbour);
			}
		}

		return { false, {}, TCost{} };
	}
} // pathx