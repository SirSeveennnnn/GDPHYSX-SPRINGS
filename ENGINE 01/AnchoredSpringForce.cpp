#include "AnchoredSpringForce.h"

AnchoredSpringForce::AnchoredSpringForce(glm::vec3 anchorPoint, float springConstant, float restLength)
{
	this->anchor = anchorPoint;
	this->springConstant = springConstant;
	this->restLength = restLength;
}

void AnchoredSpringForce::updateForce(Particle* particle, float time)
{
	glm::vec3 pos = particle->GetPosition();

	glm::vec3 force = pos - anchor;

	float magnitude = glm::length(force);
	magnitude = abs(magnitude - restLength);
	magnitude *= springConstant;

	//glm::vec3 normalizedForce = glm::normalize(force); //These are commented out because it makes the values nand/-nand
	//normalizedForce *= -magnitude;
	glm::vec3 normalizedForce;
	normalizedForce.x = force.x / magnitude;
	normalizedForce.y = force.y / magnitude;
	normalizedForce.z = force.z / magnitude;

	normalizedForce *= -magnitude;


	particle->AddForce(normalizedForce);

	//cout << "Magnitude: " << magnitude << endl;
	//cout << "pos: x:" << pos.x << " y:" << pos.y << " z:" << pos.z << endl;
	//cout << "otherpos: x:" << otherpos.x << " y:" << otherpos.y << " z:" << otherpos.z << endl;
	//cout << "force: x:" << force.x << " y:" << force.y << " z:" << force.z << endl;
}
