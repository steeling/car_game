ifeq ($(TARGET), osx)
	LIB=-framework GLUT -framework OpenGL
else
	LIB=-DGL_GLEXT_PROTOTYPES -lglut -lGL -lGLU
endif

all: a4

run: a4
	./a4

a4:  main.o tiny_obj_loader.o MatrixStack.o GLSL.o Camera.o Shape.o Image.o car.o Ramp.o odeBoxBox.o GuardRail.o
	g++ -o a4 -ansi -pedantic -Wno-deprecated main.o tiny_obj_loader.o MatrixStack.o GLSL.o Camera.o Shape.o GuardRail.o Image.o car.o Ramp.o odeBoxBox.o $(LIB)

main.o: main.cpp
	g++ -c -ansi -pedantic -Wno-deprecated main.cpp $(LIB)

Image.o: Image.cpp
	g++ -c -ansi -pedantic -Wno-deprecated Image.cpp

Shape.o: Shape.cpp
	g++ -c -ansi -pedantic -Wno-deprecated Shape.cpp $(LIB)

tiny_obj_loader.o: tiny_obj_loader.cc
	g++ -c -ansi -pedantic -Wno-deprecated tiny_obj_loader.cc

MatrixStack.o: MatrixStack.cpp
	g++ -c -ansi -pedantic -Wno-deprecated MatrixStack.cpp

GLSL.o: GLSL.cpp
	g++ -c -ansi -pedantic -Wno-deprecated GLSL.cpp -DGL_GLEXT_PROTOTYPES

Camera.o: Camera.cpp
	g++ -c -ansi -pedantic -Wno-deprecated Camera.cpp -DGL_GLEXT_PROTOTYPES

car.o: car.cpp
	g++ -c -ansi -pedantic -Wno-deprecated car.cpp -DGL_GLEXT_PROTOTYPES

Ramp.o: ramp.cpp
	g++ -c -ansi -pedantic -Wno-deprecated Ramp.cpp -DGL_GLEXT_PROTOTYPES

odeBoxBox.o: odeBoxBox.cpp
	g++ -c -ansi -pedantic -Wno-deprecated odeBoxBox.cpp -DGL_GLEXT_PROTOTYPES	

GuardRail.o: GuardRail.cpp
	g++ -c -ansi -pedantic -Wno-deprecated GuardRail.cpp -DGL_GLEXT_PROTOTYPES	


clean:
	rm -f *~ *.o a.out a4
