/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/

#include <iostream> // for std::cout
#include <glm\gtc\random.hpp> // for glm::linearRand
#include <TTK\GraphicsUtils.h> // for drawing utilities

#include "AnimationMath.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
	: m_pParticles(nullptr),
	m_pNumParticles(0),
	playing(true),
	interpolateColour(false)
{

}

ParticleEmitter::~ParticleEmitter()
{
	freeMemory();
}

void ParticleEmitter::initialize(unsigned int numParticles)
{
	freeMemory(); // destroy any existing particles

	if (numParticles > 0)
	{
		m_pParticles = new Particle[numParticles];
		memset(m_pParticles, 0, sizeof(Particle) * numParticles);
		m_pNumParticles = numParticles;
	}
}

void ParticleEmitter::reset()
{
	if (m_pParticles && playing) // make sure memory is initialized and system is playing
	{
		// loop through each particle
		Particle* particle = m_pParticles;

		for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
		{
			particle->life = 0.0f;
		}
	}
}

void ParticleEmitter::freeMemory()
{
	if (m_pParticles) // if not a null pointer
	{
		delete[] m_pParticles;
		m_pParticles = nullptr;
		m_pNumParticles = 0;
	}
}

void ParticleEmitter::update(float dt)
{
	if (m_pParticles && playing) // make sure memory is initialized and system is playing
	{
		// loop through each particle
		Particle* particle = m_pParticles;

		int erate = parEmitSettings.rate.y;

		for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
		{
			//std::cout << i %  erate << std::endl;
			//if (i %  erate) {

				if (particle->life <= 0) // if particle has no life remaining
				{
					// Respawn particle
					// Note: we are not freeing memory, we are "Recycling" the particles
					particle->acceleration = parEmitSettings.acceleration;
					float randomTval1 = glm::linearRand(0.0f, 1.0f);
					//particle->colour = Math::lerp(parEmitSettings.colorStart, parEmitSettings.colorEnd, randomTval1);
					particle->colour = parEmitSettings.colorEnd;
					float randomTval2 = glm::linearRand(0.0f, 1.0f);

					//if (parEmitSettings.size.x < parEmitSettings.size.y)
					particle->life = Math::lerp(parEmitSettings.lifetime.x, parEmitSettings.lifetime.y, randomTval2);
					//else
						//particle->life = Math::lerp(parEmitSettings.lifetime.y, parEmitSettings.lifetime.x, randomTval2);

					//particle->life = parEmitSettings.lifetime.y;
					
					float randomTval3 = glm::linearRand(0.0f, 1.0f);
					particle->mass = Math::lerp(parEmitSettings.mass.x, parEmitSettings.mass.y, randomTval3);

					float randomTval4 = glm::linearRand(0.0f, 1.0f);
					particle->position = emitterPosition + parEmitSettings.offset + Math::lerp(0.0f, 5.0f, randomTval4);
					//particle->position.x += Math::lerp(-30.0f, 30.0f, randomTval4); //DoNotUseThisFile
					particle->position += Math::lerp(parEmitSettings.minParPos, parEmitSettings.maxParPos, randomTval4);

					float randomTval5 = glm::linearRand(0.0f, 1.0f);
					particle->size = Math::lerp(parEmitSettings.size.x, parEmitSettings.size.y, randomTval5);
					particle->size = parEmitSettings.size.y;
					float randomTval6 = glm::linearRand(0.0f, 1.0f);
					particle->velocity = Math::lerp(parEmitSettings.minVelocity, parEmitSettings.maxVelocity, randomTval6);
				}
			//}

			// Update acceleration (basic Newtonian physics)
			glm::vec3 gravityValue;

			if (parEmitSettings.gravity)
				gravityValue = glm::vec3(0.0f, parEmitSettings.gravityForce, 0.0f);
			else
				gravityValue = glm::vec3(0.0f);

			particle->acceleration = particle->force / particle->mass;
			//particle->acceleration += gravityValue;

			if (parEmitSettings.attract || parEmitSettings.repel) {
				particle->acceleration = glm::vec3(0.0f);
				particle->position += particle->velocity * dt;
			}
			else {
				particle->acceleration += gravityValue;
				particle->velocity = particle->velocity + (particle->acceleration * dt);
				particle->position = particle->position + particle->velocity * dt;
			}

			// We've applied the force, let's remove it so it does not get applied next frame
			particle->force = glm::vec3(0.0f);

			// Decrease particle life
			particle->life -= dt;

			float tVal = particle->life / parEmitSettings.lifetime.y;
			particle->colour = Math::lerp(parEmitSettings.colorStart, parEmitSettings.colorEnd, tVal);
			particle->size = Math::lerp(parEmitSettings.size.x, parEmitSettings.size.y, tVal); // big to small
			//particle->size = Math::lerp(parEmitSettings.size.y, parEmitSettings.size.x, particle->life / parEmitSettings.lifetime.x);
			//std::cout << "In here!!!" << std::endl;

			
			// Update visual properties
			if (interpolateColour)
			{
				// calculate t value
				float tVal = Math::invLerp(particle->life, lifeRange.x, lifeRange.y);
				particle->colour = Math::lerp(parEmitSettings.colorStart, parEmitSettings.colorEnd, tVal);
			}
		}
	}
}

void ParticleEmitter::draw()
{
	// Draw the emitter position
	// Note: not necessary
	//TTK::Graphics::DrawCube(emitterPosition, 50.0f, glm::vec4(1.0f));
	
	Particle* p = m_pParticles;
	for (int i = 0; i < m_pNumParticles; ++i, ++p)
	{
		if (p->life >= 0.0f) // if particle is alive, draw it
		{
			TTK::Graphics::DrawCube(p->position, p->size, p->colour);
			//TTK::Graphics::DrawTeapot(p->position, p->size, p->colour); // low fps alert!! use with low particle count
			//TTK::Graphics::DrawPoint(p->position, p->size, p->colour);
			// You can draw anything!
			// ...but you should stick to points in this framework since it uses GL 1.0
		}
	}
}

void ParticleEmitter::applyForceToParticle(unsigned int idx, glm::vec3 force)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::applyForceToParticle ERROR: idx " << idx << "out of range!" << std::endl;
		return;
	}

	m_pParticles[idx].force = force;
}

void ParticleEmitter::activateMagnet(unsigned int idx, glm::vec3 pos, float val)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::activateMagnet ERROR: idx " << idx << "out of range!" << std::endl;
		return;
	}

	if(val > 0.5)
		m_pParticles[idx].position += (pos *0.05f); 
	else if (val > 0.4)
		m_pParticles[idx].position += (pos *0.04f);
	else if(val > 0.3)
		m_pParticles[idx].position += (pos *0.03f);
	else if (val > 0.2)
		m_pParticles[idx].position += (pos *0.02f);
	else
		m_pParticles[idx].position += (pos *0.01f);
}

glm::vec3 ParticleEmitter::getParticlePosition(unsigned int idx)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
		return glm::vec3();
	}

	return m_pParticles[idx].position;
}
