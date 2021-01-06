//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "SteeringAgent.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};

	steering.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	//DEBUG RENDERING
	if (pAgent->CanRenderBehavior())
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	
	return steering;
}

//FLEE (base > SEEK)
//******
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//TODO: add this to evade
	auto distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	if (distanceToTarget > m_FleeRadius)
	{
		SteeringOutput steering;
		steering.IsValid = false;
		return steering;
	}
	//same as seek, just inverse the target position
	SteeringOutput fleeing{};

	fleeing.LinearVelocity = -(m_Target).Position - pAgent->GetPosition();
	fleeing.LinearVelocity.Normalize();
	fleeing.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	
	return fleeing;
}

//ARRIVE (base > SEEK)
//******
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput arriving{};
	float maxSpeed{ pAgent->GetMaxLinearSpeed() },
		speed{},
		distance{ Elite::Distance((m_Target).Position, pAgent->GetPosition()) };
	arriving.LinearVelocity = (m_Target).Position - pAgent->GetPosition(); //Desired Velocity
	if (distance <= m_SlowDownDistance)
		speed = maxSpeed * (distance/m_SlowDownDistance);
	else
		speed = maxSpeed;
	arriving.LinearVelocity.Normalize(); //Normalize Desired Velocity
	arriving.LinearVelocity *= speed; //Rescale to Max Speed
	return arriving;
}

//FACE (base > SEEK)
//******
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput facing{};	
	facing.AngularVelocity = Elite::OrientationToVector(pAgent->GetOrientation()).AngleWith(m_Target.Position-pAgent->GetPosition());
	facing.AngularVelocity *= pAgent->GetMaxAngularSpeed();
	
	return facing;
}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	float angleOffset{ randomFloat(m_AngleChange / 2, -m_AngleChange / 2) };
	m_WanderAngle += angleOffset;

	Elite::Vector2 circlePos{pAgent->GetPosition() + pAgent->GetDirection() * m_Offset },
		randomPoint{};

	randomPoint.x = circlePos.x + cos(m_WanderAngle) * m_Radius;
	randomPoint.y = circlePos.y + sin(m_WanderAngle) * m_Radius;
	
	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawCircle(circlePos, m_Radius, { 1, 0, 0 }, 0.4f);
		DEBUGRENDERER2D->DrawPoint(randomPoint, 5.f, { 0, 0, 1 });
	}

	SteeringOutput wandering{};
	wandering.LinearVelocity = randomPoint - pAgent->GetPosition(); //Desired Velocity
	wandering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	wandering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed

	return wandering;
}
