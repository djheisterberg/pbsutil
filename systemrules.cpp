#include "systemrules.h"

bool SystemRules::isWholeNode(Job& job) {
   return (job.getPPN() == getMaxPPN());
}

char analyzeBigMem(Job& job) {
   if ((job.getMemMB() > 0) || (job.getNNodes() > 1)) {
      return ANALYSIS_QUESTIONABLE;
   }
   return ANALYSIS_OK;
}

char analyzeVis(Job& job) {
   if (job.getNNodes() > 1) {
      return ANALYSIS_QUESTIONABLE;
   }
   return ANALYSIS_OK;
}
   
char analyzeWholeNode(SystemRules* rules, Job& job) {
   int ppn = job.getPPN();
   if (((ppn > 0) && (ppn != rules->getMaxPPN())) || (job.getMemMB() > 0)) {
      return ANALYSIS_QUESTIONABLE;
   }
   return ANALYSIS_OK;
}

char analyzePartialNode(Job& job) {
   if (job.getNNodes() > 1) {
      return ANALYSIS_QUESTIONABLE;
   }
   return ANALYSIS_OK;
}

char SystemRules::analyze(Job& job) {

   if (isBigMem(job)) {
      return analyzeBigMem(job);
   } else if (isVis(job)) {
      return analyzeVis(job);
   } else {
      if (job.getMemMB() > getMemLimitMB()) {
         return ANALYSIS_BAD;
      }
      if (isWholeNode(job)) {
         return analyzeWholeNode(this, job);
      } else {
         return analyzePartialNode(job);
      }
   }

   return ANALYSIS_OK;
}
