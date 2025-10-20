default:

.PHONY: default

%.o : %.c
	$(CC) -I../c -Wall -g -std=c23 -c $< -o $@

%.o : %.cpp
	$(CXX) -I../c -Wall -g -std=c++23 -c $< -o $@

%.o : %.cc
	$(CXX) -I../c -Wall -g -std=c++23 -c $< -o $@

ironseed-c.o : ../c/ironseed.c
	$(CC) -I../c -Wall -g -std=c23 -c $< -o $@

test-c: test-c.o ironseed-c.o
	$(CXX) $^ -o $@

.PHONY: test

test: test-c
	./test-c
