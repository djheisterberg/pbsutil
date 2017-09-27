#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <pbs_ifl.h>

#define ONE_HOUR 3600

struct attropl* createSelAttrs() {

   time_t then = time(NULL) - ONE_HOUR;
   char *thenString = (char *) malloc (16);
   snprintf(thenString, 16, "%ld", then);

   struct attropl* sAttrs = (struct attropl *) malloc(sizeof (struct attropl));
   sAttrs->next = NULL;
   sAttrs->name = strdup(ATTR_start_time);
   sAttrs->resource = NULL;
   sAttrs->value = thenString;
   sAttrs->op = GE;

   struct attropl* qAttrs = (struct attropl *) malloc(sizeof(struct attropl));
   qAttrs->name = strdup(ATTR_state);
   qAttrs->next = sAttrs;
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

   struct batch_status* batchStatus0 = batchStatus;
   int nJobs = 0;
   int maxStarts = 0;
   int minStarts = 1000000;
   double starts = 0.0;
   double starts2 = 0.0;
   while (batchStatus) {
      nJobs++;

      struct attrl* attrs = batchStatus->attribs;
      while (attrs) {
         if (!strcmp(attrs->name, ATTR_start_count)) {
            int sc = atoi(attrs->value);
            if (sc > maxStarts) maxStarts = sc;
            if (sc < minStarts) minStarts = sc;
            starts += sc;
            starts2 += (sc * sc);
            if ((sc > 1) && (argc > 1)) {
               printf("%s %d\n", batchStatus->name, sc);
            }
            break;
         }
         attrs = attrs->next;
      }

      batchStatus = batchStatus->next;
   }

   if (batchStatus0) {
      pbs_statfree(batchStatus0);
      batchStatus0 = NULL;
   }

   pbs_disconnect(cnx);

   double avg = starts / nJobs;
   double std = starts2 / nJobs;
   std = sqrt(std - avg * avg);
   printf("%d jobs: min=%d, max=%d, avg=%f, std=%f\n", nJobs, minStarts, maxStarts, avg, std);
   return 0;
}
