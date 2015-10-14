RW_NAME = rw
TRANSFER1_NAME = transfer1
TRANSFER2_NAME = transfer2
SRC = buffer_synchronization.c
RW_SRC = $(SRC) rw.c
TRANSFER1_SRC = $(SRC) transfer1.c
TRANSFER2_SRC = $(SRC) transfer2.c
OBJ = $(RW_SRC:.c=.o)
CFLAGS += -Wall -Wextra -pedantic-errors
LIBS = -lpthread
RM = rm -f

#
#

all:
	gcc $(CFLAGS) $(RW_SRC) -o $(RW_NAME) $(LIBS)
	gcc $(CFLAGS) $(TRANSFER1_SRC) -o $(TRANSFER1_NAME) $(LIBS)
	gcc $(CFLAGS) $(TRANSFER2_SRC) -o $(TRANSFER2_NAME) $(LIBS)
debug: 
	gcc $(CFLAGS) -DDEBUG $(RW_SRC) -o $(RW_NAME) $(LIBS)
	gcc $(CFLAGS) -DDEBUG $(TRANSFER1_SRC) -o $(TRANSFER1_NAME) $(LIBS)
	gcc $(CFLAGS) -DDEBUG $(TRANSFER2_SRC) -o $(TRANSFER2_NAME) $(LIBS)
clean:
	-$(RM) *.o
	-$(RM) *~
	-$(RM) \#*
fclean: clean
	-$(RM) $(RW_NAME) $(TRANSFER1_NAME) $(TRANSFER2_NAME)
re: fclean all