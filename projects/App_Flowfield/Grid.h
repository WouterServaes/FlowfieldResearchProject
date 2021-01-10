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
		std::vector<Elite::Vector2> flowDirections{ };
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
	bool IsPointInGrid(const Elite::Vector2& point);
	//adding to grid
	void AddObstacle(const Elite::Vector2& obstaclePos);
	void AddGoal(const Elite::Vector2& goalPos);
	int AmountGoalsAdded()const;
	//goal vector
	void MakeGoalVector();
	bool GoalVectorReady() { return m_MadeGoalVector; };
	int GetNewGoal(int currentGoal) const;
	//grid drawing
	void ToggleDrawGrid() { m_DrawGrid = !m_DrawGrid; };
	void ToggleDrawObstacles() { m_DrawObstacles= !m_DrawObstacles; };
	void ToggleDrawGoals() { m_DrawGoals= !m_DrawGoals; };
	void ToggleDrawDirections() { m_DrawDirections = !m_DrawDirections; };

	size_t GetAmountOfFlowfields()const { return m_Goals.size(); };
	bool SetFlowfieldToDraw(size_t flowfieldNr);
	bool FlowfieldDrawn()const { return m_DrawDirections; };

	bool SaveToFile(const std::string& fileName) const;
	bool SetFromFile(const std::string& fileName);
private:
	void InitGrid();

	//grid drawing
	void DrawGridSqr(size_t idx, const Elite::Color& color, bool fillSqr) const;
	void DrawGrid() const;
	void DrawFlowfield() const;
	size_t m_FlowfieldToDraw{ 0 };

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

	Elite::Color m_GridColor{ 180.f/255.f, 180.f/255.f, 180.f/255.f},
		m_DirectionColor{0.f, 0.f, 1.f},
		m_ObstacleColor{1.f, 0.f, 0.f},
		m_GoalColor{0.f, 1.f, 0.f},
		m_HighlitedGoalColor{ m_DirectionColor };

	bool m_DrawGrid{ true }, m_DrawObstacles{ true }, m_DrawGoals{ true }, m_DrawDirections{false},
		m_MadeGoalVector{ false }, madeFlowFields{ false };

	int m_AddedGoalsAmount{};
};

