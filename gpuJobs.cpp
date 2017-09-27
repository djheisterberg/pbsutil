#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <pbs_ifl.h>

struct attropl* createSelAttrs() {

   struct attropl* qAttrs = (struct attropl *) malloc(sizeof(struct attropl));
   qAttrs->name = strdup(ATTR_state);
   qAttrs->next = NULL;
   qAttrs->resource = NULL;
   qAttrs->value = strdup("R");
   qAttrs->op = EQ;

   return qAttrs;
}

int main(int argc, char** argv) {

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
      pbs_disconnect(cnx);
      return 1;
   }

   int nJobs = 0;
   int nGPUs = 0;
   struct batch_status* batchStatus0 = batchStatus;
   while (batchStatus) {

      char* user;
      char* gpuHost = NULL;
      char* wallReqSec;
      char* wallSec;

      struct attrl* attrs = batchStatus->attribs;
      while (attrs) {
         if (!strcmp(attrs->name, ATTR_euser)) {
            user = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_exec_gpus)) {
            gpuHost = attrs->value;
            nJobs++;
            nGPUs++;
            for (char* g = gpuHost; *g; g++) {
               if (*g == '+') {
                  nGPUs++;
               }
            }
         } else if (!strcmp(attrs->name, ATTR_l)) {
            if (!strcmp(attrs->resource, "walltime")) {
               wallReqSec = attrs->value;
            }
         } else if (!strcmp(attrs->name, ATTR_used)) {
            if (!strcmp(attrs->resource, "walltime")) {
               wallSec = attrs->value;
            }
         }

         attrs = attrs->next;
      }

      if (gpuHost && (argc == 1)) {
         printf("%s %s %s %s %s\n", batchStatus->name, user, gpuHost, wallReqSec, wallSec);
      }

      batchStatus = batchStatus->next;
   }

   if (batchStatus0) {
      pbs_statfree(batchStatus0);
      batchStatus0 = NULL;
   }

   pbs_disconnect(cnx);

   if (nJobs) {
      printf("%d gpu jobs, %d gpus\n", nJobs, nGPUs);
   }
   return 0;
}
