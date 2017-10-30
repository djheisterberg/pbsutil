#include "jmanager.h"


int main(int argc, char** argv) {

   JManager myJobManager = JManager();
   myJobManager.setPbsServer();
   myJobManager.setPbsConn();
   struct attropl* qAttrs = myJobManager.createSelAttrs();
   myJobManager.getBatchStatus(qAttrs);
   myJobManager.parseBatchStatus();
   myJobManager.close();

   return 0;
}