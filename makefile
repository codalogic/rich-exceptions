all:
	g++ -o rich-exception \
		rich-exception-example.cpp rich-exception-linkage-check.cpp

run: all
	./rich-exception
