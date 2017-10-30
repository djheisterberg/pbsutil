#include "jmanager.h"

#define ONE_HOUR 3600

void JManager::setSystemRules() {

   OakleyRules oakleyRules = OakleyRules();
   OwensRules owensRules = OwensRules();
   RubyRules rubyRules = RubyRules();
   systemRules[oakleyRules.getSystem()] = &oakleyRules;
   systemRules[owensRules.getSystem()] = &owensRules;
   systemRules[rubyRules.getSystem()] = &rubyRules;
}


// return 1 if set sussessfully, else return 0.
int JManager::setPbsServer() {

   int result;
   pbsServer = pbs_default();
   if (!pbsServer) {
      printf("No pbsServer defined\n");
      result = 0;
   }else {
      result = 1;
   }
   return result;
}

// return 1 if set sussessfully, else return 0.
int JManager::setPbsConn() {

   cnx = pbs_connect(pbsServer);
   int result;
   if (cnx <= 0) {
      printf("Error connecting to server %s: %d\n", pbsServer, cnx);
      result = 0;
   }else {
      result = 1;
   }
   return result;

}

struct attropl* JManager::createSelAttrs() {


   // this create a time point. Any time less than this point means existed for more than 1 hour.
   time_t then = time(NULL) - ONE_HOUR;
   char *thenString = (char *) malloc (16);
   snprintf(thenString, 16, "%ld", then);

   // this is the first criteria, which specify the create time of the job. 
   struct attropl* cAttrs = (struct attropl *) malloc(sizeof (struct attropl));
   cAttrs->next = NULL;
   cAttrs->name = strdup(ATTR_ctime); // name of the attribute we want to query 
   cAttrs->resource = NULL;
   cAttrs->value = thenString; // target value we want attribute to be 
   cAttrs->op = LT; // less than, logic

   // this is the second criteria, the state need to be "Q"
   struct attropl* qAttrs = (struct attropl *) malloc(sizeof(struct attropl));
   qAttrs->next = cAttrs;
   qAttrs->name = strdup(ATTR_state);
   qAttrs->resource = NULL;
   qAttrs->value = strdup("Q");
   qAttrs->op = EQ; // equal

   return qAttrs;

}


void JManager::getBatchStatus(struct attropl*& qAttrs) {
   batchStatus = pbs_selstat(cnx, qAttrs, NULL);
   if (!batchStatus) {
      printf("Error getting batch status\n");
      char* pbsErr = pbs_geterrmsg(cnx);
   }
   batchStatus0 = batchStatus;
}


void JManager::parseBatchStatus() {

   job = Job();
   nJobs = 0;
   nFlaggedJobs = 0;

   time_t reallyOld = time(NULL) - 24 * ONE_HOUR;
   char ctime[20];
  
   while (batchStatus) {
      nJobs++;
      job.clear();
      getIdAndSystem(batchStatus->name, "ruby");
      time_t ctime_t = 0;
      struct attrl* attrs = batchStatus->attribs;
      while (attrs) {
         if (!strcmp(attrs->name, ATTR_euser)) { // only if attrs->name == ATTR_, then assgin the values. 
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
      // if system is ruby, oakley or owens, then..
      if (systemRules.count(system)) {
         SystemRules* sr = systemRules[system]; // find the right system rule
         analysis = sr->analyze(job); // analyze the job
      } else {
         analysis = '#'; // can't find a match system for this job. 
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
}

void JManager::getIdAndSystem(char* nm, char* defaultSystem) {
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

   // if c has a value, then ....
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

void JManager::close() {

   if (batchStatus0) {
         pbs_statfree(batchStatus0);
         batchStatus0 = NULL;
   }
   pbs_disconnect(cnx);
   printf(".. %d flagged jobs of %d\n", nFlaggedJobs, nJobs);

}




















