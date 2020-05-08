CC = gcc
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .c files here (source files, excluding header files)
hostgprof_SRC = gmon.c profil.c main.c mcount.c

# List corresponding compiled object files here (.o files)
hostgprof_OBJ = gmon.o profil.o main.o mcount.o

#################################

# default rule

all: hostgprof
	@echo "my work is done here..."


# rule for making hostgprof

hostgprof: $(hostgprof_OBJ)
	$(CC) -o hostgprof $(CFLAGS) $(hostgprof_OBJ) -lm
	@echo "-----------DONE WITH hostgprof-----------"

# type "make clean" to remove all .o files plus the hostgprof binary

clean:
	rm -f *.o hostgprof


# type "make clobber" to remove all .o files (leaves hostgprof binary)

clobber:
	rm -f *.o

# for my environment, running my program in atom IDE
run:
	hostgprof $(ARGS)
