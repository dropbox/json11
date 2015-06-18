test: json11.cpp json11.hpp test.cpp
	$(CXX) -O -std=c++11 json11.cpp test.cpp -o test -fno-rtti -fno-exceptions

clean:
	if [ -e test ]; then rm test; fi

.PHONY: clean
