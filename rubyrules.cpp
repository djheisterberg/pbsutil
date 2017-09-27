#include "rubyrules.h"

std::string RubyRules::getSystem() {
   return system;
}

bool RubyRules::isWholeNode(Job& job) {
   return true;
}

bool RubyRules::isBigMem(Job& job) {
   return (job.getPPN() == 32);
}

bool RubyRules::isVis(Job& job) {
   return false;
}

int RubyRules::getMaxPPN() {
   return 20;
}

int RubyRules::getMemLimitMB() {
   return 64 * 1024;
}
