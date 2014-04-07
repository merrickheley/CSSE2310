PROGRAM = thresher
C_FILES := thresher.c thresherSupport.c ansiC.c misc.c java.c latex.c
OBJS := $(C_FILES:.c=.o)
CFLAGS = -Wall -pedantic -std=gnu99

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(PROGRAM)
	@echo "Built $(PROGRAM)!"

%.o: %.c
	gcc $(CFLAGS) -c $<
	
clean:
	@rm -f *.o *.gch
	@echo "Cleaned!"
