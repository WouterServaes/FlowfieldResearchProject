#pragma once
#include "projects/App_Flowfield/SteeringAgent.h"
#include "projects/App_Flowfield/SteeringBehaviors.h"
class FlowfieldAgent
{
public:
	FlowfieldAgent(const Elite::Vector2& spawnPos);
	~FlowfieldAgent();

	void RenderAgent(float deltaTime) const;
	void UpdateAgent(float deltaTime);

	void NewEndGoalTarget(int endGoalIdx) { m_CurrentEndGoalIdx = endGoalIdx; };
	int GetEndGoal()const { return m_CurrentEndGoalIdx; };
	Elite::Vector2 GetCurrentPos()const { return m_pAgent->GetPosition(); };
	Elite::Vector2& CurrentTargetPos() { return m_CurrentTargetPos; };
	void DrawCurrentTarget()const { DEBUGRENDERER2D->DrawPoint(m_CurrentTargetPos, 5.f, { 1, 1, 1 }); };

	void ReachedGoal(bool reachedGoal) { m_ReachedGoal = reachedGoal; };
private:
	Elite::Color m_AgentColor{ 0.f, 0.f, 0.f };

	SteeringAgent* m_pAgent{};
	Seek* m_pBehavior{};
	
	int m_CurrentEndGoalIdx{ 0 };
	Elite::Vector2 m_CurrentTargetPos{0.f,0.f};

	bool m_ReachedGoal{ false };
};

