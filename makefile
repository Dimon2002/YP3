PROG = main

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

$(PROG): $(OBJ)
	@gcc $(OBJ) -o $(PROG)

$(PREF_OBJ)%.o : $(PREF_SRC)%.c
	@gcc -std=c99 -c -O $< -o $@

c :
	@rm $(PROGNAME) $(PREF_OBJ)*.o

s : $(PROG)
	@./$(PROG) 2> error.txt
