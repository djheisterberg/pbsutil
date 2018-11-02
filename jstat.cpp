#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>

#include <pbs_ifl.h>

#include "systemrules.h"
#include "oakleyrules.h"
#include "owensrules.h"
#include "rubyrules.h"
#include "pitzerrules.h"
#include "job.h"

#define ONE_HOUR 3600

struct attropl* createSelAttrs() {

   time_t then = time(NULL) - ONE_HOUR;
   char *thenString = (char *) malloc (16);
   snprintf(thenString, 16, "%ld", then);

   struct attropl* cAttrs = (struct attropl *) malloc(sizeof (struct attropl));
   cAttrs->next = NULL;
   cAttrs->name = strdup(ATTR_ctime);
   cAttrs->resource = NULL;
   cAttrs->value = thenString;
   cAttrs->op = LT;

   struct attropl* qAttrs = (struct attropl *) malloc(sizeof(struct attropl));
   qAttrs->next = cAttrs;
   qAttrs->name = strdup(ATTR_state);
   qAttrs->resource = NULL;
   qAttrs->value = strdup("Q");
   qAttrs->op = EQ;

   return qAttrs;
}

void getIdAndSystem(Job& job, char* nm, char* defaultSystem) {
   char c;
   job.id = nm;
   while ((c = *nm)) {
      if (c == '.') {
         *nm = 0;
         nm++;
         break;
      }
      nm++;
   }
   if (c) {
      job.system = nm;
      while ((c = *nm)) {
         if ((c == '.') || (c == '-')) {
            *nm = 0;
            nm++;
            break;
         }
         nm++;
      }
   } else {
      job.system = defaultSystem;
   }
}

int main(int argc, char** argv) {

   time_t reallyOld = time(NULL) - 24 * ONE_HOUR;

   std::unordered_map<std::string, SystemRules*> systemRules;
   OakleyRules oakleyRules = OakleyRules();
   OwensRules owensRules = OwensRules();
   RubyRules rubyRules = RubyRules();
   PitzerRules pitzerRules = PitzerRules();
   systemRules[oakleyRules.getSystem()] = &oakleyRules;
   systemRules[owensRules.getSystem()] = &owensRules;
   systemRules[rubyRules.getSystem()] = &rubyRules;
   systemRules[pitzerRules.getSystem()] = &pitzerRules;

   char *pbsServer = pbs_default();
   if (!pbsServer) {
      printf("No pbsServer defined\n");
      return 1;
   }

   int cnx = pbs_connect(pbsServer);
   if (cnx <= 0) {
      printf("Error connecting to server %s: %d\n", pbsServer, cnx);
      return 1;
   }

   struct attropl* qAttrs = createSelAttrs();
   struct batch_status* batchStatus = pbs_selstat(cnx, qAttrs, NULL);
   if (!batchStatus) {
      printf("Error getting batch status\n");
      char* pbsErr = pbs_geterrmsg(cnx);
   }

   struct batch_status* batchStatus0 = batchStatus;
   char ctime[20];
   Job job = Job();
   int nJobs = 0;
   int nFlaggedJobs = 0;
   while (batchStatus) {
      nJobs++;
      job.clear();
      getIdAndSystem(job, batchStatus->name, "ruby");
      time_t ctime_t = 0;

      struct attrl* attrs = batchStatus->attribs;
      while (attrs) {
         if (!strcmp(attrs->name, ATTR_euser)) {
            job.user = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_A)) {
            job.account = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_N)) {
            job.name = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_queue)) {
            job.queue = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_ctime)) {
            ctime_t = (time_t) atol(attrs->value);
            strftime(ctime, 20, "%Y-%m-%d %H:%M:%S", localtime(&ctime_t));
            job.ctime = ctime;
         } else if (!strcmp(attrs->name, ATTR_l)) {
            if (!strcmp(attrs->resource, "nodes")) {
               job.nodesString = attrs->value;
            } else if (!strcmp(attrs->resource, "mem")) {
               job.memString = attrs->value;
            } else if (!strcmp(attrs->resource, "walltime")) {
               job.timeString = attrs->value;
            }
         }
         attrs = attrs->next;
      }

      char analysis = ANALYSIS_OK;
      std::string system = job.system;
      if (systemRules.count(system)) {
         SystemRules* sr = systemRules[system];
         analysis = sr->analyze(job);
      } else {
         analysis = '#';
      }
      if ((analysis == ANALYSIS_OK) && (ctime_t < reallyOld)) {
         analysis = '@';
      }
      if (analysis != ANALYSIS_OK) nFlaggedJobs++;

      char* nodes = job.nodesString;
      if (!nodes) nodes = "";
      char* mem = job.memString;
      if (!mem) mem = "";
      char* time = job.timeString;
      if (!time) time = "";

      printf("%c%c %s.%s user=%s account=%s jobName=%s queue=%s ctime=%s; nodes=%s mem=%s walltime=%s\n",
            analysis, analysis, job.id, job.system, job.user, job.account, job.name, job.queue, job.ctime,
            job.nodesString, mem, time);
      batchStatus = batchStatus->next;
   }
   if (batchStatus0) {
      pbs_statfree(batchStatus0);
      batchStatus0 = NULL;
   }

   pbs_disconnect(cnx);

   printf(".. %d flagged jobs of %d\n", nFlaggedJobs, nJobs);
   return 0;
}
