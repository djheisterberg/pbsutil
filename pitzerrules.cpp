#include "pitzerrules.h"

std::string PitzerRules::getSystem() {
   return system;
}

bool PitzerRules::isBigMem(Job& job) {
   return (job.getPPN() == 80);
}

bool PitzerRules::isVis(Job& job) {
   return false;
}

int PitzerRules::getMaxPPN() {
   return 40;
}

int PitzerRules::getMemLimitMB() {
   return 192 * 1024;
}
