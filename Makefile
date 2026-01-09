# Project: CAD_hw02
# Makefile created by Embarcadero Dev-C++ 6.3

CPP      = g++.exe
CC       = gcc.exe
WINDRES  = windres.exe
OBJ      = main.o
LINKOBJ  = main.o
LIBS     = -L"D:/Dev-Cpp/TDM-GCC-64/lib" -L"D:/Dev-Cpp/TDM-GCC-64/x86_64-w64-mingw32/lib" -static-libgcc
INCS     = -I"D:/Dev-Cpp/TDM-GCC-64/include" -I"D:/Dev-Cpp/TDM-GCC-64/x86_64-w64-mingw32/include" -I"D:/Dev-Cpp/TDM-GCC-64/lib/gcc/x86_64-w64-mingw32/9.2.0/include"
CXXINCS  = -I"D:/Dev-Cpp/TDM-GCC-64/include" -I"D:/Dev-Cpp/TDM-GCC-64/x86_64-w64-mingw32/include" -I"D:/Dev-Cpp/TDM-GCC-64/lib/gcc/x86_64-w64-mingw32/9.2.0/include" -I"D:/Dev-Cpp/TDM-GCC-64/lib/gcc/x86_64-w64-mingw32/9.2.0/include/c++"
BIN      = CAD_hw02.exe
CXXFLAGS = $(CXXINCS) 
CFLAGS   = $(INCS) 
DEL      = D:\Dev-Cpp\devcpp.exe INTERNAL_DEL

.PHONY: all all-before all-after clean clean-custom

all: main.o
	@g++ -std=c++11 main.o -o exe

run:
	@./exe $(input) $(output)

clean: clean-custom
	@rm *.o
	@rm exe

main.o: main.cpp
	@g++ -std=c++11 -c main.cpp