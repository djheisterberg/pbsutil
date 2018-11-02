#ifndef JOBINFO_PITZER_RULES_H
#define JOBINFO_PITZER_RULES_H

#include <string>

#include "systemrules.h"

class PitzerRules: public SystemRules {
private:
   std::string system  = "pitzer";

public:
   std::string getSystem();
   bool isBigMem(Job& job);
   bool isVis(Job& job);
   int getMaxPPN();
   int getMemLimitMB();
};

#endif /* JOBINFO_PITZER_RULES_H */
