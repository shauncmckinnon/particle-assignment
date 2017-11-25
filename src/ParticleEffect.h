/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/
#pragma once

#include <vector>
#include <string>
#include "ParticleEmitter.h"

struct ParticleEffectSettings {
	std::string name = "default";
	std::vector<ParEmitSettings> parEmitSettings;
};

class ParticleEffect
{
public:
	ParticleEffect();
	~ParticleEffect();

	int getFileSize(const std::string &fileName);
	bool save();
	ParticleEffectSettings load(std::string filename);
	void setupEmitters();
	void resetEmitters();
	friend std::ostream& operator << (std::ostream& stream, ParticleEffectSettings settings);

	// data members
	int peID = 0;
	std::string name = "default";
	std::vector<ParEmitSettings> parEmitSettings;
	std::vector<ParticleEmitter> emitters;
};