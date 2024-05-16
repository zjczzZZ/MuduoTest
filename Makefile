make:
	g++ -g -Wall *.cpp  -o server -lmuduo_net -lmuduo_base -lpthread -lhiredis  -lmysqlclient -std=c++17
	
clean:
	rm server