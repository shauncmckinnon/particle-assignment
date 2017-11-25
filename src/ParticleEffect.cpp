/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/
#include "ParticleEffect.h"
#define RAW reinterpret_cast<char*>

ParticleEffect::ParticleEffect() {

}

ParticleEffect::~ParticleEffect() {

}

int ParticleEffect::getFileSize(const std::string &fileName) {
	std::ifstream file(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

	if (!file.is_open())
	{
		return -1;
	}

	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.close();

	return fileSize;
}

bool ParticleEffect::save() {
	// create file name
	std::string filename = name + ".dat";

	// get the file
	std::ofstream savefile(filename, std::ios::out | std::ios::binary);

	// check if the file didn't load
	if (!savefile) {
		std::cout << "Error in loading file." << std::endl;
		return false;
	}

	// write (or overwrite) the file
	savefile.seekp(0);

	// get the name size and write it and the data
	int nameSize = name.length() + 1;
	savefile.write(RAW(&nameSize), sizeof(int));
	savefile.write(name.c_str(), name.length() + 1);

	// get the vector size and write it
	int vectorSize = parEmitSettings.size();
	savefile.write(RAW(&vectorSize), sizeof(int));

	// iterate over the vector and write the values to the file
	int currentSeek = name.length() + 1;

	for (int i = 0; i < parEmitSettings.size(); i++)
	{
		savefile.write(RAW(&parEmitSettings[i]), sizeof(ParEmitSettings));
	}

	if (savefile.bad()) {
		std::cout << "Particle Effect Settings could not be saved.";
		savefile.close();
		return false;
	}

	savefile.close();
	return true;
}

ParticleEffectSettings ParticleEffect::load(std::string filename) {
	// create the particle effect settings object
	ParticleEffectSettings peSettings;
	std::cout << "File being loaded: " << filename << std::endl;

	// get the file
	std::ifstream loadfile(filename, std::ios::in | std::ios::binary);

	// check if the file didn't load
	if (!loadfile) {
		std::cout << "Error in loading file." << std::endl;
	}

	// values
	int nameSize = 0;
	int vectorSize = 0;

	// read the file
	loadfile.read(RAW(&nameSize), sizeof(int));
	char* nameIn = new char[nameSize];
	loadfile.read(nameIn, nameSize);
	name = nameIn;

	// load in the vector
	peSettings.parEmitSettings.clear();
	parEmitSettings.clear();
	loadfile.read(RAW(&vectorSize), sizeof(int));
	for (int i = 0; i < vectorSize; i++) {
		ParEmitSettings temp;
		loadfile.read(RAW(&temp), sizeof(ParEmitSettings));
		parEmitSettings.push_back(temp);
		peSettings.parEmitSettings.push_back(temp);
	}

	// file reads like shit
	if (loadfile.bad()) {
		std::cout << "There was an error reading the file." << std::endl;
	}

	peSettings.name = name;
	setupEmitters();

	loadfile.close();
	return peSettings;
}

void ParticleEffect::setupEmitters() {
	emitters.clear();
	for (int i = 0; i < parEmitSettings.size(); i++)
	{
		ParticleEmitter parEmit;
		emitters.push_back(parEmit);
	}

	for (int i = 0; i < parEmitSettings.size(); i++)
	{
		//std::cout << "ParEff.ParEmiSet.size: " << particleEffect.parEmitSettings.size() << ", Emitter.size: " << emitter.size() << std::endl;

		// Physics properties
		emitters[i].emitterPosition = parEmitSettings[i].position;

		// Create the particles
		emitters[i].initialize(parEmitSettings[i].numOfPar);

		// write parEmitSettings
		emitters[i].parEmitSettings = parEmitSettings[i];
	}
}

void ParticleEffect::resetEmitters()
{
	for (int i = 0; i < parEmitSettings.size(); i++)
	{
		// Physics properties
		emitters[i].reset();
		emitters[i].update(0.01f);
	}
}

std::ostream& operator << (std::ostream& stream, ParticleEffectSettings settings) {
	stream << "Name: " << settings.name << ", " << "Emitter Count: " << settings.parEmitSettings.size();
	return stream;
}