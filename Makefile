CC = gcc
SDL = -lSDL2main -lSDL2 -lm 
TARGET = simple-tic-tac-toe
SRCDIR = src/
OBJDIR = build/obj/
OBJECTS = build/obj/main.o

$(TARGET): $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o $(TARGET) ${SDL}
	
$(OBJDIR)%.o: $(SRCDIR)%.c
	mkdir -p $(OBJDIR)
	$(CC) -c $^ -o $@
