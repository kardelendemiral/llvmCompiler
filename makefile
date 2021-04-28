mylang2ir:	main.o
			g++ -std=c++11 main.o -o mylang2ir
			

main.o:		main.cpp
			g++ -std=c++11 -c main.cpp
			
