INC	:= 
LIB	:= 
CXXFLAGS	:= -O2 -DUNIT_TESTING

all:	test_emodels

test_emodels:	test_emodels.cpp emodels.h Makefile
	g++ test_emodels.cpp -o test_emodels $(INC) $(CXXFLAGS) $(LIB)

#lib_emodels:	emodels.h Makefile
#	g++ -c -o emodels.o emodels.h $(INC) $(CXXFLAGS) $(LIB)
#	g++ -shared emodels.o -o emodels.so

clean:
	rm -f test_emodels emodels.o emodels.so
