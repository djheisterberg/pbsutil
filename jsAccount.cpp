#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#include <pbs_ifl.h>

struct attropl* createSelAttrs(char *account) {

   struct attropl* aAttrs = (struct attropl *) malloc(sizeof(struct attropl));
   aAttrs->next = NULL;
   aAttrs->name = ATTR_A;
   aAttrs->resource = NULL;
   aAttrs->value = account;
   aAttrs->op = EQ;

   return aAttrs;
}

char *trimToChar(char *ss, char x) {
   char *s = ss;
   char c;
   while ((c = *s) && (c != x)) {
      s++;
   }
   *s = 0;
   return ss;
}

char* formatTime(char *ts, char *fTime) {
   time_t time = (time_t) atol(ts);
   strftime(fTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
   return fTime;
}

int countAccountJobs(int cnx, char *account, int *nRunning, int *nQueued, int *nBlocked, bool longFormat) {

   int jobs = 0;
   *nRunning = 0;
   *nQueued = 0;
   *nBlocked = 0;

   struct attropl* aAttrs = createSelAttrs(account);
   struct batch_status* batchStatus = pbs_selstat(cnx, aAttrs, NULL);
   if (!batchStatus) {
      printf("No batch status for account %s\n", account);
   }

   struct batch_status* batchStatus0 = batchStatus;
   while (batchStatus) {
      jobs++;
      bool running = false;
      bool waiting = false;

      char *user;
      char *submitTime;
      char *startTime;
      char *wallReq;
      char *wallUsed;

      struct attrl* attrs = batchStatus->attribs;
      while (attrs) {
         if (!strcmp(attrs->name, ATTR_state)) {
            if (!strcmp(attrs->value, "R")) {
               *nRunning += 1;
               running = true;
            } else if (!strcmp(attrs->value, "Q")) {
               *nQueued += 1;
               waiting = true;
            } else if (!strcmp(attrs->value, "B")) {
               *nBlocked += 1;
               waiting = true;
            }
         } else if (!strcmp(attrs->name, ATTR_l)) {
            if (!strcmp(attrs->resource, "walltime")) {
               wallReq = attrs->value;
            }
         } else if (!strcmp(attrs->name, ATTR_used)) {
            if (!strcmp(attrs->resource, "walltime")) {
               wallUsed = attrs->value;
            }
         } else if (!strcmp(attrs->name, ATTR_ctime)) {
            submitTime = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_start_time)) {
            startTime = attrs->value;
         } else if (!strcmp(attrs->name, ATTR_owner)) {
            user = attrs->value;
         }
         attrs = attrs->next;
      }

      if (longFormat) {
         char fTime[20];
         if (running) {
            printf("%s %s started: %s, time req: %s, time used: %s\n",
               trimToChar(batchStatus->name, '.'), trimToChar(user, '@'), formatTime(startTime, fTime), wallReq, wallUsed);
         } else if (waiting) {
            printf("%s %s submitted: %s, time req: %s\n",
               trimToChar(batchStatus->name, '.'), trimToChar(user, '@'), formatTime(submitTime,fTime), wallReq);
         }
      }

      batchStatus = batchStatus->next;
   }

   if (batchStatus0) {
      pbs_statfree(batchStatus0);
      batchStatus0 = NULL;
   }

   return jobs;
}

int main(int argc, char** argv) {

   bool longFormat = false;
   int opt = 0;
   while ((opt = getopt(argc, argv, "l")) > -1) {
      if (opt == 'l') {
         longFormat = true;
      }
   }

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

   int nRunning, nQueued, nBlocked = 0;
   for (int i = optind; i < argc; i++) {
      char *account = argv[i];
      if (countAccountJobs(cnx, account, &nRunning, &nQueued, &nBlocked, longFormat)) {
         printf("%s: %d running, %d queued, %d blocked\n", account, nRunning, nQueued, nBlocked);
      }
   }

   pbs_disconnect(cnx);
   return 0;
}
