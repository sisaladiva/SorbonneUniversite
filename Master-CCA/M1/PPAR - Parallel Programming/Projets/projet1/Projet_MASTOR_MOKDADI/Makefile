CC = mpicc
CFLAGS = -O3 -Wall -lm
HOSTFILE = $OAR_NODEFILE
NP = 4

all: tranches sequentiel 3D checkpointnonbloc

tranches: tranches.c
	$(CC) tranches.c -o tranches $(CFLAGS)

sequentiel: sequentiel.c
	$(CC) sequentiel.c -o sequentiel $(CFLAGS)

3D: 3D.c
	$(CC) 3D.c -o 3D $(CFLAGS)
	
checkpointnonbloc: checkpointnonbloc.c
	$(CC) checkpointnonbloc.c -o checkpointnonbloc $(CFLAGS)


clean:
	rm -f tranches sequentiel 3D checkpointnonbloc
