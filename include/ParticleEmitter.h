/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/
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
	int			numOfPar		= 100;
	glm::vec3	position		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3   minParPos		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3   maxParPos		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	minVelocity		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	maxVelocity		= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	acceleration	= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	force			= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3	offset			= glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4	colorStart		= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4	colorEnd		= glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec2	mass			= glm::vec2(0.1f, 1.0f);
	glm::vec2	size			= glm::vec2(0.1f, 30.0f);
	glm::vec2	lifetime		= glm::vec2(1.0f, 50.0f);
	glm::vec2	rate			= glm::vec2(0.0f, 1.0f);
	float	    duration		= 300.0f;

	glm::vec3	targetPosition	= glm::vec3(500, 700, 0);
	float		gravityForce	= 0.0f;
	float		fleeForce		= 0.0f;
	float		seekForce		= 0.0f;
	bool		gravity			= false;
	bool		seekToPoint		= false;
	bool		fleeFromPoint	= false;
	bool		repel			= false;
	bool		attract			= false;
	bool		followPath		= false;
	
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
	void reset();
	void freeMemory();

	void update(float dt);
	void draw();

	void applyForceToParticle(unsigned int idx, glm::vec3 force);
	void activateMagnet(unsigned int, glm::vec3 pos, float val);

	unsigned int getNumParticles() { return m_pNumParticles; }
	void setNumParticles(unsigned int numParticles) { m_pNumParticles = numParticles; }

	glm::vec3 getParticlePosition(unsigned int idx);

	///// Playback properties
	bool playing;

	//// Update flags
	bool interpolateColour;

	///// Initial properties for newly spawned particles
	glm::vec3 emitterPosition;
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

	int emissionRate = 0;

	ParEmitSettings parEmitSettings;
};