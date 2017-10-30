#ifndef JMANAGER_H
#define JMANAGER_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unordered_map>

#include <pbs_ifl.h>

#include "systemrules.h"
#include "oakleyrules.h"
#include "owensrules.h"
#include "rubyrules.h"
#include "job.h"


class JManager {

public:

	std::unordered_map<std::string, SystemRules*> systemRules;
	char *pbsServer;
	int cnx;
	struct batch_status* batchStatus;
	struct batch_status* batchStatus0;
	char ctime[20];
	Job job;
	int nJobs;
	int nFlaggedJobs;

	void setSystemRules();
	int setPbsServer();
	int setPbsConn();
	struct attropl* createSelAttrs();
	void getBatchStatus(struct attropl*& qAttrs);
	void parseBatchStatus();
	void getIdAndSystem(char* nm, char* defaultSystem);
    void close();

};

#endif /* JMANAGER_H */