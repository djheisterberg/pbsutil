#include "owensrules.h"

std::string OwensRules::getSystem() {
   return system;
}

bool OwensRules::isBigMem(Job& job) {
   return (job.getPPN() == 48);
}

bool OwensRules::isVis(Job& job) {
   return false;
}

int OwensRules::getMaxPPN() {
   return 28;
}

int OwensRules::getMemLimitMB() {
   return 126 * 1024;
}
