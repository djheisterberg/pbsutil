#ifndef JOBINFO_OAKLEY_RULES_H
#define JOBINFO_OAKLEY_RULES_H

#include <string>

#include "systemrules.h"

class OakleyRules: public SystemRules {
private:
   std::string system  = "oak";
   std::string bigmem = "bigmem";
   std::string vis = "vis";

public:
   std::string getSystem();
   bool isBigMem(Job& job);
   bool isVis(Job& job);
   int getMaxPPN();
   int getMemLimitMB();
};

#endif /* JOBINFO_OAKLEY_RULES_H */
