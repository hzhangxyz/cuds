default: main.out

run: main.out
	./main.out

main.out: main.o string.o variable.o item.o list.o term.o rule.o utility.o ground.o match.o
	$(CXX) $^ -o $@ -g -O3

%.o: %.c++
	$(CXX) -c $< -o $@ -g -O3

clean:
	$(RM) -f *.o *.out
