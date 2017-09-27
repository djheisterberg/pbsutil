EXES = jstat startCounts gpuJobs jsAccount softwareJobs

CXX = icc
CFLAGS = -std=c++11 -I$(PBS_INSTALL)/include -O3
LD = icc
LIBS = -L$(PBS_INSTALL)/lib -ltorque

%.o:	%.cpp
	$(CXX) -c $(CFLAGS) $<

jstat:	jstat.o systemrules.o oakleyrules.o rubyrules.o owensrules.o
	$(LD) $(LIBS) -o $@ $^

startCounts:	startCounts.o
	$(LD) $(LIBS) -o $@ $^

gpuJobs:	gpuJobs.o
	$(LD) $(LIBS) -o $@ $^

jsAccount:	jsAccount.o
	$(LD) $(LIBS) -o $@ $^

softwareJobs:	softwareJobs.o
	$(LD) $(LIBS) -o $@ $^

.PHONY:	clean
clean:
	rm -f $(EXES) *.o
