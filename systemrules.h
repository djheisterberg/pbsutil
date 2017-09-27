#ifndef JOBINFO_SYSTEM_RULES_H
#define JOBINFO_SYSTEM_RULES_H

#define ANALYSIS_OK ' '
#define ANALYSIS_QUESTIONABLE '?'
#define ANALYSIS_BAD '!'

#include <string>

#include "job.h"

class SystemRules {
public:
   virtual std::string getSystem() = 0;
   virtual bool isWholeNode(Job& job);
   virtual bool isBigMem(Job& job) = 0;
   virtual bool isVis(Job& job) = 0;
   virtual int getMaxPPN() = 0;
   virtual int getMemLimitMB() = 0;

   virtual char analyze(Job& job);
};

#endif /* JOBINFO_SYSTEM_RULES_H */
