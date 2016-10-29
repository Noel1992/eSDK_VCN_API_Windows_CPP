CASEOBJS=gets_test.o memcpytest.o scanftestutf8.o sprintftest.o strcattest.o strcpytest.o strtoktest.o testmain.o testutil.o memset_s_test.o memmove_test.o str_perf.o
CC = gcc
FLAGS = -I ../include -g -D_DEBUG


maintest: $(CASEOBJS)
	$(CC)  $(FLAGS) -L../lib -lsecurec -o maintest $(CASEOBJS) 

gets_test.o: gets_test.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
memcpytest.o: memcpytest.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
memset_s_test.o: memset_s_test.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
memmove_test.o: memmove_test.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
scanftestutf8.o: scanftestutf8.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
sprintftest.o: sprintftest.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
strcattest.o: strcattest.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
strcpytest.o: strcpytest.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
strtoktest.o: strtoktest.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
str_perf.o: str_perf.c
	$(CC)  $(FLAGS) -c $<
testmain.o: testmain.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
testutil.o: testutil.c unittest.h testutil.h
	$(CC)  $(FLAGS) -c $<
cle:
	rm *.o maintest*
	