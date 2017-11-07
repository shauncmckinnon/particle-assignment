// Shaun McKinnon - 100642799 //
#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <vector>

// Params for each particle
// Note: this is a bare minimum particle structure
// Add any properties you want to control here
struct Particle
{
	// Physics properties
	// Position is where the particle currently is
	// Velocity is the rate of change of position
	// Acceleration is the rate of change of velocity
	// p' = p + v*dt
		// current position is the previous position plus the change in position multiplied by the amount of time passed since we last calculated position
	// v' = v + a*dt
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
	float mass;

	// Visual Properties
	float size;
	glm::vec4 colour;
	// Other properties... 
	// ie. sprite sheet

	float life;
};

struct ParEmitSettings {
	glm::vec3	 position	   = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	 velocity	   = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	 acceleration  = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	 force		   = glm::vec3(0.0f, 0.0f, 0.0f);
	float		 mass		   = 0.0f;
	float		 size		   = 0.0f;
	float		 lifetime	   = 0.0f;
	bool		 gravity	   = false;
	float		 rate		   = 0.0f;
	float		 duration	   = 0.0f;
	glm::vec4	 colorStart    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4	 colorEnd	   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3	 offset		   = glm::vec3(0.0f, 0.0f, 0.0f);
	bool		 seekToPoint   = false;
	bool		 fleeFromPoint = false;
	bool		 repel		   = false;
	bool		 attract	   = false;
	bool		 followPath    = false;
};

// Emitter is responsible for emitting (creating, dispatching) particles
class ParticleEmitter
{
public:
private:
	unsigned int m_pNumParticles;
	Particle* m_pParticles;

	void __GetParticlePos(glm::vec3 *pos);

public:
	ParticleEmitter();
	~ParticleEmitter();

	void initialize(unsigned int numParticles);
	void freeMemory();

	void update(float dt);
	void draw();

	void applyForceToParticle(unsigned int idx, glm::vec3 force);

	unsigned int getNumParticles() { return m_pNumParticles; }
	void setNumParticles(unsigned int numParticles) { m_pNumParticles = numParticles; }

	glm::vec3 getParticlePosition(unsigned int idx);

	///// Playback properties
	bool playing;

	//// Update flags
	bool interpolateColour;

	///// Initial properties for newly spawned particles

	glm::vec3 emitterPosition;
	// TODO: Add "box emitter" properties (float width, float height, bool enabled)
	float width, height;
	bool enabled;

	// Storing ranges for each property so each particle has some randomness
	glm::vec3 velocity0;
	glm::vec3 velocity1;

	// these properties are single floats, so we can pack the min and max into a vec2, just data!
	glm::vec2 lifeRange;
	glm::vec2 sizeRange;
	glm::vec2 massRange;

	glm::vec4 colour0;
	glm::vec4 colour1;

	// ... other properties
	// ... what would be a better way of doing this?
	// Make a keyframe controller for each property! this gives you max control!!
	// See the KeyframeController class
	// (this is what full out particle editors do, ie popcorn fx)
};