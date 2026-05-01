CC := g++
INCPATH := ./include
LIBPATH := ./lib
LIBS := -lraylib -lopengl32 -lgdi32 -lwinmm
EXEC := xadrez.exe

main: ./src/main.cpp
	${CC} $< -I${INCPATH} -L${LIBPATH} ${LIBS} -o ./bin/${EXEC}

start:
	./bin/${EXEC}