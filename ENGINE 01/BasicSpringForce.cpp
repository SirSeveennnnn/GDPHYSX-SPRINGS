#include "BasicSpringForce.h"

BasicSpringForce::BasicSpringForce(Particle* other, float springConstant, float restLength)
{
	this->other = other;
	this->springConstant = springConstant;
	this->restLength = restLength;

}

void BasicSpringForce::updateForce(Particle* particle, float time)
{
	glm::vec3 pos = particle->GetPosition();
	glm::vec3 otherpos = other->GetPosition();

	glm::vec3 force = pos - otherpos;

	float magnitude = glm::length(force);

	magnitude = abs(magnitude - restLength);
	magnitude *= springConstant;

	//glm::vec3 normalizedForce = glm::normalize(force); //These are commented out because it makes the values nand/-nand
	glm::vec3 normalizedForce;
	normalizedForce.x = force.x / magnitude;
	normalizedForce.y = force.y / magnitude;
	normalizedForce.z = force.z / magnitude;

	

	normalizedForce *= -magnitude;

	//this should be normalizedForce but i cant make  glm::normalize to work properly
	particle->AddForce(normalizedForce);

	//cout << "Magnitude: " << magnitude << endl;
	//cout << "pos: x:" << pos.x << " y:" << pos.y << " z:" << pos.z << endl;
	//cout << "otherpos: x:" << otherpos.x << " y:" << otherpos.y << " z:" << otherpos.z << endl;
	//cout << "force: x:" << force.x << " y:" << force.y << " z:" << force.z << endl;

}
