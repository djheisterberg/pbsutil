#include "job.h"

int Job::getNNodes() {
   initializeNodes();
   return nNodes;
}

int Job::getPPN() {
   initializeNodes();
   return ppn;
}

int Job::getGPUs() {
   initializeNodes();
   return gpus;
}

int Job::getMemMB() {
   initializeMem();
   return memMB;
}

int Job::getTimeSec() {
   initializeTime();
   return timeSec;
}

std::unordered_map<std::string, std::string>& Job::getNodeAttributes() {
   initializeNodes();
   return nodeAttributes;
}

void Job::clear() {
   id = NULL;
   system = NULL;
   user = NULL;
   account = NULL;
   name = NULL;
   ctime = NULL;
   queue = NULL;
   nodesString = NULL;
   memString = NULL;
   timeString = NULL;
   software = NULL;

   nNodes = -1;
   memMB = -1;
   timeSec = -1;
   nodeAttributes.clear();
}


int parseNodes(char *ns, std::unordered_map<std::string, std::string>& na) {
   int nNodes = 0;

   if (ns) { // if ns is not NULL 
      int is = 0;
      char c;
      while (c = ns[is++]) {
         if ((c < '0') || (c > '9')) break;
         nNodes = 10 * nNodes + (c - '0');
      }
      while (c == ':') {
         int k0 = is;
         while (c = ns[is++]) {
            if ((c == '=') || (c == ':')) break;
         }
         int k1 = is - 1;
         int v0 = is;
         int v1 = is;
         if (c == '=') {
            while (c = ns[is++]) {
               if (c == ':') break;
            }
         }
         v1 = is - 1;
         int kLen = k1 - k0;
         if (kLen > 0) {
            std::string key (&ns[k0], kLen);
            int vLen = v1 - v0;
            std::string value;
            if (vLen > 0) {
               value = std::string(&ns[v0], vLen);
            }
            na[key] = value;
         }
      }
   }
   return nNodes;
}


// assign values to ppn and gpus from nodeAttributes, assign values to nNodes using parseNodes. 
void Job::initializeNodes() {
   if (nNodes < 0) {
      nNodes = parseNodes(nodesString, nodeAttributes);

      ppn = 0;
      std::string _ppn = nodeAttributes["ppn"];
      nodeAttributes.erase("ppn");
      if (!_ppn.empty()) {
         ppn = std::stoi(_ppn);
      }

      gpus = 0;
      std::string _gpus = nodeAttributes["gpus"];
      nodeAttributes.erase("gpus");
      if (!_gpus.empty()) {
         gpus = std::stoi(_gpus);
      }
   }
}

int parseMem(char *ms) {
   unsigned long memMB = 0;
   if (ms) {
      char c;
      while (c = *ms++) {
         if ((c < '0') || (c > '9')) break;
         memMB = 10 * memMB + (c - '0');
      }
      if ((c == 'G') || (c == 'g')) {
         memMB *= 1024;
         c = *ms++;
      } else if ((c == 'M') || (c == 'm')) {
         ; // NOP
         c = *ms++;
      } else if ((c == 'K') || (c == 'k')) {
         memMB >>= 10;
         c = *ms++;
      } else {
         memMB >>= 20;
      }
      if ((c == 'W') || (c == 'w')) {
         memMB *= 8;
      }
   }
   return (int) memMB;
}

void Job::initializeMem() {
   if (memMB < 0) {
      memMB = parseMem(memString);
   }
}

int parseTime(char* ts) {
   int timeSec = 0;
   int tSec = 0;
   if (ts) {
      char c;
      while (c = *ts++) {
         if (c == ':') {
            timeSec = 60 * (timeSec + tSec);
            tSec = 0;
         } else if ((c >= '0') && (c <= '9')) {
            tSec = 10 * tSec + (c - '0');
         }
      }
      timeSec += tSec;
   }
   return timeSec;
}

void Job::initializeTime() {
   if (timeSec < 0) {
      timeSec = parseTime(timeString);
   }
}
