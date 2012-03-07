#include "Particles.h"
double GetParticleMass(std::string AParticleType)
{
  if((AParticleType!="kaon") && (AParticleType!="pion")) throw "Unknown Particle Type";
  if(AParticleType=="kaon") return 493.677e6;
  if(AParticleType=="pion") return 139.57018e6;
  throw "Particle Type not specified in Particles.c";
}
