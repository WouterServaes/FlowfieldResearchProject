#pragma once
#include "Project/App_Flowfield/SteeringAgent.h"
#include "Project/App_Flowfield/SteeringBehaviors.h"

class FlowfieldAgent
{
public:
	FlowfieldAgent(const Elite::Vector2& spawnPos, int amountOfGoals);
	~FlowfieldAgent();

	void RenderAgent(float deltaTime, int activeFlowfield) const; //renders the agent
	void UpdateAgent(float deltaTime); //updates the agent

	//goals
	void NewEndGoalTarget(int endGoalIdx) { m_CurrentEndGoalIdx = endGoalIdx; }; //sets a new goal target
	int GetEndGoal()const { return m_CurrentEndGoalIdx; }; //returns current goal
	void SetReachedGoal(bool reachedGoal) { m_ReachedGoal = reachedGoal; };
	bool GetReachedGoal()const { return m_ReachedGoal; };

	//position
	Elite::Vector2 GetCurrentPos()const { return m_pAgent->GetPosition(); }; //returns current position
	Elite::Vector2& CurrentTargetPos() { return m_CurrentTargetPos; }; //returns current target position

	//removing agent
	bool IsMarkedForRemove()const { return m_MarkForRemove; };
private:
	SteeringAgent* m_pAgent{}; 
	Seek* m_pBehavior{}; 

	Elite::Color m_AgentDefaultColor{ 0.f, 0.f, 0.f },
		m_AgentHightlightColor{ 0.f, 0.f, 1.f };

	int m_CurrentEndGoalIdx{ 0 };
	Elite::Vector2 m_CurrentTargetPos{ 0.f, 0.f };

	bool m_ReachedGoal{ false };

	//removing agent
	bool m_MarkForRemove{ false };
	float m_DeleteElapsedSec{};
	const float m_DeleteTime{ .2f };
};
