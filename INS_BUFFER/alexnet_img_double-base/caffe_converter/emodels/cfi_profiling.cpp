#include <iostream>
#include <fstream>
#include "emodels.h"

using namespace std;

union doubleToInt{UINT64 intval; double dblval;};
long long totalProfileCount = 0;
long long layerCounter = 0;

int fiLayerNo = -1;
int fiInstIndex = -1;

void dumpProfileLog(long long staticIndex){
	ofstream pf;
	pf.open ("profiling.log", ios::app);
	pf << staticIndex << " " << totalProfileCount << "\n";
	pf.close();
}

void dumpLayerInfo(string infoString){
	//ofstream pf;
	//pf.open ("profiling.log", ios::app);
	//pf << "\nLayer: " << layerCounter << ": " << infoString << "\n";
	//pf.close();
	layerCounter++;
}

void profileCount(long long staticIndex){

	if ( fiLayerNo != -1){
		if ( fiLayerNo != layerCounter)
			continue;
	}
	if ( fiInstIndex = -1){
		if ( fiInstIndex != staticIndex)
			 continue;
	}

	totalProfileCount++;
	//dumpProfileLog(staticIndex);
}

void endDumpProfileLog(){
	ofstream pf;
	pf.open ("profiling.log", ios::trunc);
	pf << "Total " << totalProfileCount << "\n";
	pf.close();
}
