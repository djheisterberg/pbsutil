#ifndef JOBINFO_RUBY_RULES_H
#define JOBINFO_RUBY_RULES_H

#include <string>

#include "systemrules.h"

class RubyRules: public SystemRules {
private:
   std::string system  = "ruby";

public:
   std::string getSystem();
   bool isWholeNode(Job& job);
   bool isBigMem(Job& job);
   bool isVis(Job& job);
   int getMaxPPN();
   int getMemLimitMB();
};

#endif /* JOBINFO_RUBY_RULES_H */
