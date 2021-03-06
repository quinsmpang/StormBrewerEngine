#pragma once

#include "Foundation/Pathfinding/Pathfinding.h"



template <typename GridData, typename CostVar, typename MoveEvaluator, bool AllowDiagonals = false, bool DiagonalCostIncrease = true>
class PathfindingBasicGrid2D
{
public:
  using NodeId = uint32_t;
  using CostType = CostVar;
  static const NodeId kInvalidNodeId = -1;

  PathfindingBasicGrid2D(const Vector2 & size) :
    m_Grid(std::make_unique<GridContainerData[]>(size.x * size.y)),
    m_Width(size.x),
    m_Height(size.y)
  {

  }

  PathfindingBasicGrid2D(const PathfindingBasicGrid2D & rhs) = delete;
  PathfindingBasicGrid2D(PathfindingBasicGrid2D && rhs) = delete;

  PathfindingBasicGrid2D & operator =(const PathfindingBasicGrid2D & rhs) = delete;
  PathfindingBasicGrid2D & operator =(PathfindingBasicGrid2D && rhs) = delete;

  Vector2 NodeIdToLocation(NodeId node_id) const
  {
    return Vector2{ (int)(node_id % m_Width), (int)(node_id / m_Width) };
  }

  NodeId LocationToNodeId(const Vector2 & pos) const
  {
    return (NodeId)(pos.y * m_Width + pos.x);
  }

  std::size_t GetNumNodes() const
  {
    return m_Width * m_Height;
  }

  void SetGridData(int x, int y, const GridData & data)
  {
    m_Grid[x + y * m_Width].m_Data = data;
  }

  const GridData & GetGridData(int x, int y) const
  {
    return m_Grid[x + y * m_Width].m_Data;
  }

  void LinkNode(NodeId node_id, NodeId src_node, CostVar cost)
  {
    auto & grid_data = m_Grid[node_id];

    grid_data.m_PrevNode = src_node;
    grid_data.m_Cost = cost;
  }

  bool LinkNodeCheck(NodeId node_id, NodeId src_node, CostVar cost)
  {
    auto & grid_data = m_Grid[node_id];

    if (cost < grid_data.m_Cost)
    {
      grid_data.m_PrevNode = src_node;
      grid_data.m_Cost = cost;
      return true;
    }

    return false;
  }

  NodeId GetLinkedNode(NodeId node_id)
  {
    auto & grid_data = m_Grid[node_id];
    return grid_data.m_PrevNode;
  }

  CostVar EvalHeuristic(NodeId dest_node_id, NodeId src_node) const
  {
    auto src_location = NodeIdToLocation(src_node);
    auto dst_location = NodeIdToLocation(dest_node_id);
    if (AllowDiagonals)
    {
      auto dx = abs(src_location.x - dst_location.x);
      auto dy = abs(src_location.y - dst_location.y);

      auto mx = std::max(dx, dy);

      if (DiagonalCostIncrease)
      {
        auto mn = std::min(dx, dy);
        auto diag_dist = (CostVar)mn * kSqrt2;

        return diag_dist + (CostVar)(mx - mn);
      }
      else
      {
        return (CostVar)mx;
      }
    }
    else
    {
      return (CostVar)ManhattanDist(src_location, dst_location);
    }
  }

  template <typename Visitor>
  void VisitNeighbors(NodeId node_id, Visitor && visitor)
  {
    auto location = NodeIdToLocation(node_id);
    auto target_location = NodeIdToLocation(node_id);
    auto & src_grid = GetGridData(location.x, location.y);

    auto zero = CostVar(0);

    if (location.x > 0)
    {
      auto dest_node = LocationToNodeId(Vector2(location.x - 1, location.y));
      auto & dest_data = GetGridData(location.x - 1, location.y);
      auto cost = m_Eval(src_grid, dest_data);

      if (cost >= zero)
      {
        visitor(dest_node, node_id, cost);
      }
    }

    if (location.x < m_Width - 1)
    {
      auto dest_node = LocationToNodeId(Vector2(location.x + 1, location.y));
      auto & dest_data = GetGridData(location.x + 1, location.y);
      auto cost = m_Eval(src_grid, dest_data);

      if (cost >= zero)
      {
        visitor(dest_node, node_id, cost);
      }
    }

    if (location.y > 0)
    {
      auto dest_node = LocationToNodeId(Vector2(location.x, location.y - 1));
      auto & dest_data = GetGridData(location.x, location.y - 1);
      auto cost = m_Eval(src_grid, dest_data);

      if (cost >= zero)
      {
        visitor(dest_node, node_id, cost);
      }
    }

    if (location.y < m_Height - 1)
    {
      auto dest_node = LocationToNodeId(Vector2(location.x, location.y + 1));
      auto & dest_data = GetGridData(location.x, location.y + 1);
      auto cost = m_Eval(src_grid, dest_data);

      if (cost >= zero)
      {
        visitor(dest_node, node_id, cost);
      }
    }

    if (AllowDiagonals)
    {
      if (location.x > 0 && location.y > 0)
      {
        auto dest_node = LocationToNodeId(Vector2(location.x - 1, location.y - 1));
        auto & dest_data = GetGridData(location.x - 1, location.y - 1);
        auto cost = m_Eval(src_grid, dest_data);

        if (DiagonalCostIncrease)
        {
          cost *= kSqrt2;
        }

        if (cost >= zero)
        {
          visitor(dest_node, node_id, cost);
        }
      }

      if (location.x > 0 && location.y < m_Height - 1)
      {
        auto dest_node = LocationToNodeId(Vector2(location.x - 1, location.y + 1));
        auto & dest_data = GetGridData(location.x - 1, location.y + 1);
        auto cost = m_Eval(src_grid, dest_data);

        if (DiagonalCostIncrease)
        {
          cost *= kSqrt2;
        }

        if (cost >= zero)
        {
          visitor(dest_node, node_id, cost);
        }
      }

      if (location.x < m_Width - 1 && location.y > 0)
      {
        auto dest_node = LocationToNodeId(Vector2(location.x + 1, location.y - 1));
        auto & dest_data = GetGridData(location.x + 1, location.y - 1);
        auto cost = m_Eval(src_grid, dest_data);

        if (DiagonalCostIncrease)
        {
          cost *= kSqrt2;
        }

        if (cost >= zero)
        {
          visitor(dest_node, node_id, cost);
        }
      }

      if (location.x < m_Width - 1 && location.y < m_Height - 1)
      {
        auto dest_node = LocationToNodeId(Vector2(location.x + 1, location.y + 1));
        auto & dest_data = GetGridData(location.x + 1, location.y + 1);
        auto cost = m_Eval(src_grid, dest_data);

        if (DiagonalCostIncrease)
        {
          cost *= kSqrt2;
        }

        if (cost >= zero)
        {
          visitor(dest_node, node_id, cost);
        }
      }
    }
  }

  template <typename Visitor>
  void VisitNodes(Visitor && visitor)
  {
    int num_nodes = (int)m_Width * m_Height;
    for (int node = 0; node < num_nodes; ++node)
    {
      visitor(node, m_Grid[node].m_Data);
    }
  }

private:
  struct GridContainerData
  {
    GridData m_Data = {};
    NodeId m_PrevNode;
    CostVar m_Cost;
  };

  std::unique_ptr<GridContainerData[]> m_Grid;
  std::size_t m_Width;
  std::size_t m_Height;

  MoveEvaluator m_Eval;

};

template <typename CostVar>
struct PathfindingBasicTraversalGridEval
{
  CostVar operator()(bool src, bool dst)
  {
    return src || dst ? CostVar(-1) : CostVar(1);
  }
};


template <bool AllowDiagonals = false, bool DiagonalCostIncrease = true>
using PathfindingBasicTraversalGrid = PathfindingBasicGrid2D<bool, float, PathfindingBasicTraversalGridEval<float>, AllowDiagonals, DiagonalCostIncrease>;

