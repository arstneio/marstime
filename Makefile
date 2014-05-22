.SUFFIXES: .c .o
CC = gcc
EXEC = marsTime
CCFLAGS = -g -Wall
OBJS = leapSecs.o marsTime.o main.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -lm -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}
    
clean:
	rm -f ${EXEC} ${OBJS}

leapSecs.o:leapSecs.c leapSecs.h main.h
marsTime.o:marsTime.c marsTime.h main.h
main.o:main.c marsTime.h
