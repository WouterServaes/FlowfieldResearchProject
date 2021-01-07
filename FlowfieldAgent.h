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

	void NewEndGoalTarget(const Elite::Vector2& worldDimensions);
private:
	Elite::Color m_AgentColor{ 0.f, 0.f, 0.f };

	SteeringAgent* m_pAgent{};
	Seek* m_pBehavior{};
	Elite::Vector2 m_EndGoalTarget{};
};

