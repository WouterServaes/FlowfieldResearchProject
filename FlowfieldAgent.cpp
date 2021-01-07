#include "stdafx.h"
#include "FlowfieldAgent.h"


FlowfieldAgent::FlowfieldAgent(const Elite::Vector2& spawnPos )
{
	m_pAgent = new SteeringAgent();
	m_pBehavior = new Seek();

	m_pAgent->SetSteeringBehavior(m_pBehavior);
	m_pAgent->SetPosition(spawnPos);
	m_pAgent->SetBodyColor(m_AgentColor);
}

FlowfieldAgent::~FlowfieldAgent()
{
	SAFE_DELETE(m_pAgent);
	SAFE_DELETE(m_pBehavior);
}

void FlowfieldAgent::RenderAgent(float deltaTime) const
{
	m_pAgent->Render(deltaTime);
}

void FlowfieldAgent::UpdateAgent(float deltaTime)
{
	m_pAgent->Update(deltaTime);
	
}

void FlowfieldAgent::NewEndGoalTarget(const Elite::Vector2& worldDimensions)
{

}