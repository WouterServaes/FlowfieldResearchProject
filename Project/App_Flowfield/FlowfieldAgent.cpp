#include "stdafx.h"
#include "FlowfieldAgent.h"


FlowfieldAgent::FlowfieldAgent(const Elite::Vector2& spawnPos, int amountOfGoals)
{
	m_pAgent = new SteeringAgent();
	m_pBehavior = new Seek();

	m_pAgent->SetSteeringBehavior(m_pBehavior);
	m_pAgent->SetPosition(spawnPos);
	m_pAgent->SetBodyColor(m_AgentDefaultColor);

	
	m_CurrentEndGoalIdx = Elite::randomInt(amountOfGoals);
	
}

FlowfieldAgent::~FlowfieldAgent()
{
	SAFE_DELETE(m_pAgent);
	SAFE_DELETE(m_pBehavior);
}

void FlowfieldAgent::RenderAgent(float deltaTime, int activeFlowfield) const
{
	if (m_CurrentEndGoalIdx == activeFlowfield)
		m_pAgent->SetBodyColor(m_AgentHightlightColor);
	else
		m_pAgent->SetBodyColor(m_AgentDefaultColor);
	m_pAgent->Render(deltaTime);
	//DrawCurrentTarget();
}

void FlowfieldAgent::UpdateAgent(float deltaTime)
{

	if (m_ReachedGoal)
	{
		m_pAgent->SetLinearVelocity({ 0, 0 });

		if (m_DeleteElapsedSec < m_DeleteTime)
			m_DeleteElapsedSec += deltaTime;
		else
			m_MarkForRemove = true;
		

	}

	m_NeedsInitalMove = false;
		
	
	m_pAgent->GetSteeringBehavior()->SetTarget(m_CurrentTargetPos);
	m_pAgent->Update(deltaTime);
}
