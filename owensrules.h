#ifndef JOBINFO_OWENS_RULES_H
#define JOBINFO_OWENS_RULES_H

#include <string>

#include "systemrules.h"

class OwensRules: public SystemRules {
private:
   std::string system  = "owens";

public:
   std::string getSystem();
   bool isBigMem(Job& job);
   bool isVis(Job& job);
   int getMaxPPN();
   int getMemLimitMB();
};

#endif /* JOBINFO_OWENS_RULES_H */
