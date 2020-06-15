all : test1.out test2.out test3.out test4.out
test1.out : myfs.cpp test1.cpp myfs.h.gch
	g++ test1.cpp myfs.cpp -lpthread -w -o test1.out

test2.out : myfs.cpp test2.cpp myfs.h.gch
	g++ test2.cpp myfs.cpp -lpthread -w -o test2.out

test3.out : myfs.cpp test3.cpp myfs.h.gch
	g++ test3.cpp myfs.cpp -lpthread -w -o test3.out

test4.out : myfs.cpp test4.cpp myfs.h.gch
	g++ test4.cpp myfs.cpp -lpthread -w -o test4.out

myfs.h.gch : myfs.h
	g++ myfs.h

clean: 
	rm -f  ./test1.out ./test2.out ./test3.out ./test4.out ./myfs.h.gch
