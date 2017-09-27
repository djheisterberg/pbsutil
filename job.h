#ifndef JOBINFO_JOB_H
#define JOBINFO_JOB_H

#include <string>
#include <unordered_map>

class Job {
public:
   char* id;
   char* system;
   char* user;
   char* account;
   char* name;
   char* ctime;
   char* queue;
   char* nodesString;
   char* memString;
   char* timeString;
   char* software;

   int getNNodes();
   int getPPN();
   int getGPUs();
   int getMemMB();
   int getTimeSec();
   std::unordered_map<std::string, std::string>& getNodeAttributes();

   void clear();

private:
   int nNodes = -1;
   int ppn = -1;
   int gpus = -1;
   int memMB = -1;
   int timeSec = -1;
   std::unordered_map<std::string, std::string> nodeAttributes = {};

   void initializeNodes();
   void initializeMem();
   void initializeTime();
};

#endif /* JOBINFO_JOB_H */
