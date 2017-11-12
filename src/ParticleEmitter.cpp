// Shaun McKinnon - 100642799 //
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
		for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
		{
			if (particle->life <= 0) // if particle has no life remaining
			{
				// Respawn particle
				// Note: we are not freeing memory, we are "Recycling" the particles
				particle->acceleration = glm::vec3(0.0f);
				float randomTval = glm::linearRand(0.0f, 1.0f);
				particle->colour = Math::lerp(colour0, colour1, randomTval);
				particle->life = Math::lerp(lifeRange.x, lifeRange.y, randomTval);
				particle->mass = Math::lerp(massRange.x, massRange.y, randomTval);

				particle->position = emitterPosition + glm::vec3(50.0f, 0.0f, 0.0f) + Math::lerp(0.0f, 5.0f, randomTval);

				particle->size = Math::lerp(sizeRange.x, sizeRange.y, randomTval);
				particle->velocity = Math::lerp(velocity0, velocity1, randomTval);
			}
			
			// Update physics

			// Update acceleration (basic Newtonian physics)
			particle->acceleration = particle->force / particle->mass;
			particle->velocity = particle->velocity + (particle->acceleration * dt);
			particle->position = particle->position + particle->velocity * dt;

			// We've applied the force, let's remove it so it does not get applied next frame
			particle->force = glm::vec3(0.0f);

			// Decrease particle life
			particle->life -= dt;

			// Update visual properties
			if (interpolateColour)
			{
				// calculate t value
				float tVal = Math::invLerp(particle->life, lifeRange.x, lifeRange.y);
				particle->colour = Math::lerp(colour0, colour1, tVal);
			}
		}
	}
}

void ParticleEmitter::draw()
{
	// Draw the emitter position
	// Note: not necessary
	TTK::Graphics::DrawTeapot(emitterPosition, 50.0f, glm::vec4(1.0f));
	
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

glm::vec3 ParticleEmitter::getParticlePosition(unsigned int idx)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
		return glm::vec3();
	}

	return m_pParticles[idx].position;
}
