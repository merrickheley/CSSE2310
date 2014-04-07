PROGRAM_1 = scores
C_FILES_1 := scores.c  misc.c
OBJS_1 = $(C_FILES_1:.c=.o)

PROGRAM_2 = trivial
C_FILES_2 := trivial.c trivialSupport.c misc.c
OBJS_2 := $(C_FILES_2:.c=.o)

PROGRAM_3 = serv
C_FILES_3 := serv.c servSupport.c misc.c
OBJS_3 := $(C_FILES_3:.c=.o)

ALL_FILES := $(C_FILES_1) $(C_FILES_2) $(C_FILES_3) $(wildcard $(patsubst %.c,%.h,$(C_FILES_1))) $(wildcard $(patsubst %.c,%.h,$(C_FILES_2))) $(wildcard $(patsubst %.c,%.h,$(C_FILES_3)))

# REMEMBER TO FIX THIS AND REMOVE -g
CFLAGS = -Wall -pedantic -std=gnu99 -pthread $(flag)

all: $(PROGRAM_1) $(PROGRAM_2) $(PROGRAM_3)

$(PROGRAM_1): $(OBJS_1)
	gcc $(CFLAGS) $(OBJS_1) -o $(PROGRAM_1)
	@echo "Built $(PROGRAM_1)!"

$(PROGRAM_2): $(OBJS_2)
	gcc $(CFLAGS) $(OBJS_2) -o $(PROGRAM_2)
	@echo "Built $(PROGRAM_2)!"
	
$(PROGRAM_3): $(OBJS_3)
	gcc $(CFLAGS) $(OBJS_3) -o $(PROGRAM_3)
	@echo "Built $(PROGRAM_3)!"

%.o: %.c
	gcc $(CFLAGS) -c $<
	
	
oclean:
	@rm -f *.o *.gch
	@echo "Cleaned!"
	
fclean:
	@rm -f *.o *.gch $(PROGRAM_1) $(PROGRAM_2) $(PROGRAM_3)
	@echo "Cleaned!"
	
style:
	~s4206155/public/style $(sort $(ALL_FILES))
