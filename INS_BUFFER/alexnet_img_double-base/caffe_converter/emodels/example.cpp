#include <iostream>
#include "emodels.h"

using namespace std;

union doubleToInt{UINT64 intval; double dblval;};

int main(){
	srand(time(NULL));

	double target = 0.111;
	
	union doubleToInt target2, target3;
	target2.dblval = target;

	target3.intval = RANDBIT1_inject64(target2.intval);

	double finalResult = target3.dblval;

	cout << target << endl;
	cout << finalResult << endl;

	return 0;
}
