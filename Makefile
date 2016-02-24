all: bin_dir libgcadapter_shared libgcadapter_static libgcadapter_test

bin_dir:
	-mkdir bin

libgcadapter_shared: libgcadapter.o
	gcc -shared -o bin/libgcadapter.so src/libgcadapter.o -Wl,--as-needed -lusb-1.0
	
libgcadapter_static: libgcadapter.o
	ar rcs bin/libgcadapter.a src/libgcadapter.o
	
libgcadapter_test: libgcadapter_static libgcadapter_test.o
	gcc -o bin/libgcadapter_test examples/libgcadapter_test.o -L./bin -l:libgcadapter.a -lusb-1.0 -lcurses
	
libgcadapter.o:
	gcc -c -o src/libgcadapter.o src/libgcadapter.c -Iinclude -Wall -Werror -Wl,--as-needed -lusb-1.0 -fpic
	
libgcadapter_test.o:
	gcc -c -o examples/libgcadapter_test.o examples/libgcadapter_test.c -L./bin -l:libgcadapter.a -lusb-1.0 -lcurses -Iinclude -Wall -Werror
	
clean: 
	-rm examples/libgcadapter_test.o
	-rm src/libgcadapter.o
	-rm bin/libgcadapter.a
	-rm bin/libgcadapter.so
	-rm bin/libgcadapter_test
	-rm -rf bin
