iosched: iosched.cpp
	g++ -std=c++11 -g -O iosched.cpp -o iosched

clean:
	rm -rf iosched *~
