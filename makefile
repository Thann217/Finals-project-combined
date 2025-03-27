DEPS = food_donation
CXXFLAGS = -std=c++11

compile: main.cpp Queue.cpp 
	g++ $(CXXFLAGS) main.cpp Queue.cpp -o $(DEPS)

run: $(DEPS)
	./$(DEPS)

clean:
	rm -f $(DEPS) *.dat

.PHONY: clean