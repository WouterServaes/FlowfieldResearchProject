#ifndef STEERINGBEHAVIORS_APPLICATION_H
#define STEERINGBEHAVIORS_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "FlowfieldAgent.h"
#include "Grid.h"

class FlowfieldAgent;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_Flowfield final : public IApp
{
public:
	//Constructor & Destructor
	App_Flowfield() = default;
	virtual ~App_Flowfield();
	
	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;
	
private:
	struct Spawner
	{
		float elapsedTime{};
		Elite::Vector2 location{};
		int agentsSpawned{};
	};

	void SpawnAgents();
	void SpawnAgent(const Elite::Vector2& pos);
	void ContinueAgentSpawning(const Elite::Vector2& pos);
	void HandleImGui();
	void HandleMouseInput();
	void HandleKeyboardInput();
	void HandleAgentUpdate(float deltaTime);

	void AddSpawners();
	void UseSpawners(float deltaTime);

	void SaveToFile();
	void ReadFromFile();

	void ResetAgents();
	std::vector<FlowfieldAgent*>* m_pAgents{};
	Grid* m_pGrid;


	bool m_TrimWorld{ false }, m_SpawnAgents{ false }, m_UseSpawners{ false }, m_KillAtGoal{ true }, m_AddAgentsWithMouse{false},
		m_MadeObstacles{ false }, m_MadeGoals{ false }, m_MadeFlowfield{ false };

	float m_TrimWorldSize{}, m_AgentSpawnWithMouseElapsedSec{};

	int m_StartAmountAgents{ 5 }, m_FlowfieldToDraw{ 0 }, m_AmountOfGoals{};

	Grid::SquareType m_TypeToPlace{ Grid::SquareType::Default };
	std::vector< Spawner*> m_pSpawners{};
	const float m_TimePerSpawn{ .2f };
	
};
#endif