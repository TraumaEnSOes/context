CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -g -ggdb -O0
CPPFLAGS=
LDLIBS=-lboost_context

context: context.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

PHONY: clean
clean:
	rm -f context
