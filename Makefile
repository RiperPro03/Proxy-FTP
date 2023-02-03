CC=gcc

proxy: proxy.o simpleSocketAPI.o
	${CC} $^ -o $@ 
proxy.o: proxy.c simpleSocketAPI.h
	${CC} -c $^ 
simpleSocketAPI.o: simpleSocketAPI.c
	${CC} -c $^ 
