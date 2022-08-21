all: output

output: main.o serialport.o readUtils.o writeUtils.o
	g++ -o output main.cpp serialport.cpp readUtils.cpp writeUtils.cpp

serialport.o: serialport.cpp serialport.h
	g++ -c serialport.cpp

readUtils.o: readUtils.cpp readUtils.h
	g++ -c readUtils.cpp

writeUtils.o: writeUtils.cpp writeUtils.h
	g++ -c writeUtils.cpp

main.o: main.cpp typeabbreviations.h
	g++ -c main.cpp

clean:
	rm *.o output