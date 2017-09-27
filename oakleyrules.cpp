#include "oakleyrules.h"

std::string OakleyRules::getSystem() {
   return system;
}

bool OakleyRules::isBigMem(Job& job) {
   return ((job.getPPN() == 32) || job.getNodeAttributes().count(bigmem));
}

bool OakleyRules::isVis(Job& job) {
   return (job.getNodeAttributes().count(vis) > 0);
}

int OakleyRules::getMaxPPN() {
   return 12;
}

int OakleyRules::getMemLimitMB() {
   return 48 * 1024;
}
