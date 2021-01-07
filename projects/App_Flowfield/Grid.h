#pragma once
#include "framework/EliteMath/EMath.h"
class Grid
{
public:
	enum class SquareType
	{
		Default, Obstacle, Goal
	};

	struct GridSquare
	{
		size_t row{}, column{};
		Elite::Vector2 botLeft{};
		std::vector<Elite::Vector2> flowDirections{ { 1.f, 0.f } };
		SquareType squareType{ SquareType::Default };
	};

	Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution);
	~Grid();

	void Render(float deltaTime) const;
	void Update(float deltaTime);

	bool MoveSqr(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, int goalNr, bool firstMove);
	bool AgentReachedGoal(const Elite::Vector2& agentPos, int agentGoal);
	Elite::Vector2 GetValidRandomPos();
	void MakeFlowfield();
	//adding to grid
	void AddObstacle(const Elite::Vector2& obstaclePos);
	void AddGoal(const Elite::Vector2& goalPos);
	//goal vector
	void MakeGoalVector();
	bool GoalVectorReady() { return m_MadeGoalVector; };
	int GetNewGoal(int currentGoal) const;
	//grid drawing
	void ToggleDrawGrid() { m_DrawGrid = !m_DrawGrid; };
	void ToggleDrawObstacles() { m_DrawObstacles= !m_DrawObstacles; };
	void ToggleDrawGoals() { m_DrawGoals= !m_DrawGoals; };
	void ToggleDrawDirections() { m_DrawDirections = !m_DrawDirections; };
private:
	void InitGrid();

	//grid drawing
	void DrawGridSqr(size_t idx, const Elite::Color& color, bool fillSqr) const;
	void DrawGrid() const;
	void DrawDirections() const;
	void DrawObstacles() const ;
	void DrawGoals() const;
	Elite::Vector2 GetMidOfSquare(size_t idx) const { //get the mid position of a square
		return m_pGrid->at(idx).botLeft + (m_SquareSize / 2.f);
	};
	size_t GetGridSqrIdxAtPos(const Elite::Vector2& pos) const;
	

	

	Elite::Vector2 m_WorldDimensions, m_GridResolution, m_SquareSize;
	std::vector<GridSquare>* m_pGrid{ nullptr };
	std::vector<Elite::Vector2> m_Goals{};

	float m_MindDistanceFromTarget{ 2.f };

	Elite::Color m_GridColor{ 0.f, 0.f, 1.f },
		m_DirectionColor{ 0.f, 0.f, 0.f },
		m_ObstacleColor{1.f, 0.f, 0.f},
		m_GoalColor{0.f, 1.f, 0.f};

	bool m_DrawGrid{ true }, m_DrawObstacles{ true }, m_DrawGoals{ true }, m_DrawDirections{true},
		m_MadeGoalVector{ false };
};

