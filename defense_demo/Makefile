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
	@echo "Compilation complete..."

# rule for making hostgprof
hostgprof: $(hostgprof_OBJ)
	$(CC) -o hostgprof $(CFLAGS) $(hostgprof_OBJ) -lm
	@echo "-----------DONE WITH hostgprof-----------"

# type "make clean" to remove all .o files plus the hostgprof binary
clean:
	rm -f *.o hostgprof* *.csv *.png *.out gprofout.txt

# makes, runs, and performs all subsequent gprof operations
run: all
	@echo "-----------RUNNING HOST PC APPLICATION-----------"
	hostgprof $(ARGS)
	@echo "-----------RUNNING GPROF-----------"
	gprof blinky_demo.elf gmon.out
	@echo "-----------GENERATING GPROF TEXT & IMAGE OUTPUTS-----------"
	gprof blinky_demo.elf gmon.out > gprofout.txt
	gprof blinky_demo.elf gmon.out | gprof2dot -e 0.0 -n 0.0 | dot -Tpng -o ./image.png
	@echo "-----------RUNNING VALIDATION SCRIPT-----------"
	python ./gmon_validation.py
	@echo "-----------DONE-----------"
