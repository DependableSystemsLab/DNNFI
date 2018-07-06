#include <iostream>
#include <fstream>
#include "emodels.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

union doubleToInt{UINT64 intval; double dblval;};
union floatToInt{UINT32 intval; float fltval;};
int readConfigFlag = 0;
long long totalInstCount = 0;
long long injectionSampleCount = 0;
long long currentInstCount = 0;
long long layerNo = 0;
string layerType;
int profile_in_fi_flag = 1;
long long totalInstProfiled = 0;
int currentFiFlag = 1;


///////////////////////////////////////
//// Config for static fi
int fiLayerNo = -1;
int fiInstIndex = -1;
///////////////////////////////////////


void resetFi(){
	readConfigFlag = 0;
	totalInstCount = 0;
	injectionSampleCount = 0;
	currentInstCount = 0;
	layerNo = 0;
}

void dumpLayerInfo(string lt){
	layerNo++;
	layerType = lt;
}

void generateInjectionSample(){
	srand(time(NULL));
	long long max = totalInstCount;
	long long min = 1;
	long long randNum = rand() % (max-min + 1) + min;
	injectionSampleCount = randNum;
}

void readInjectionConfigFile(){
/*
	fstream f("injection.config", fstream::in );
	string s;
	getline( f, s, '\0');
	//cout << s << endl;
	f.close();
	totalInstCount = atoll(s.c_str());
*/
	readConfigFlag = 1;
	std::cout << "\n**** Reading total inst profiled " << totalInstProfiled << " ****\n";
	std::cout << "\n**** FI config:  layerNo: " << fiLayerNo << ", instIndex: " << fiInstIndex << "****\n";
	totalInstCount = totalInstProfiled;
}


double injectFault(double target, int staticIndex){

	if(readConfigFlag == 0 && profile_in_fi_flag == 0){
		readInjectionConfigFile();
		generateInjectionSample();
		cout << "\n*** In FI Run (double) *** \n Total Inst Count Read: " << totalInstCount << " - Sampled Inst Count: " << injectionSampleCount << "\n***\n" << endl;
	}

        if ( fiLayerNo != -1){
                if ( fiLayerNo != layerNo)
                        return target;
        }
        if ( fiInstIndex != -1){
                if ( fiInstIndex != staticIndex)
                         return target;
        }

	currentInstCount++;

	if (profile_in_fi_flag == 1){
		totalInstProfiled = currentInstCount;
		currentFiFlag = 1;
		return target;
	}


	if(currentInstCount == injectionSampleCount){
		std::cout << "\n**** Cycle hit, injecting fault ... ****\n";

		srand(time(NULL));
		union doubleToInt target2, target3;
		target2.dblval = target;
		target3.intval = RANDBIT1_inject64(target2.intval);
		double finalResult = target3.dblval;
		//cout << target << endl;
		//cout << finalResult << endl;
		//cout << "***FI: prevValue: " << target << " fiValue: " << finalResult << "\n";

		// Write to fi_log file
		/*
		ofstream fif;
		fif.open ("fi_log");
		fif << "prevValue: " << target << " fiValue: " << finalResult << "\n";
		fif << "Sampled Inst: " << injectionSampleCount;
		fif.close();	
		*/
		FILE* fi_f = fopen("fi_log", "a");
		fprintf(fi_f, "preValue: %lu fiValue: %lu\n", target2.intval, target3.intval);
		fprintf(fi_f, "preValue: %.16f fiValue: %.16f\n", target, finalResult);
		fprintf(fi_f, "Sampled Inst: %lld\n", injectionSampleCount);
		fprintf(fi_f, "Static Index: %d\n", staticIndex);
		fprintf(fi_f, "Layer NO.: %lld\n", layerNo);
		fprintf(fi_f, "Layer Type: %s\n", layerType.c_str());
		fclose(fi_f);

		currentFiFlag = 0;
	
		return finalResult;
	}

	return target;
}


float injectFault(float target, int staticIndex){


	if(readConfigFlag == 0 && profile_in_fi_flag == 0){
		readInjectionConfigFile();
		generateInjectionSample();
		cout << "\n*** In FI Run (float) *** \n Total Inst Count Read: " << totalInstCount << " - Sampled Inst Count: " << injectionSampleCount << "\n***\n" << endl;
	}

        if ( fiLayerNo != -1){
                if ( fiLayerNo != layerNo)
                        return target;
        }
        if ( fiInstIndex != -1){
                if ( fiInstIndex != staticIndex)
                         return target;
        }

	currentInstCount++;

	if (profile_in_fi_flag == 1){
		totalInstProfiled = currentInstCount;
		currentFiFlag = 1;
		return target;
	}
	if(currentInstCount == injectionSampleCount){

		std::cout << "\n**** Cycle hit, injecting fault ... ****\n";
		srand(time(NULL));
		union floatToInt target2, target3;
		target2.fltval = target;
		target3.intval = RANDBIT1_inject32(target2.intval);
		float finalResult = target3.fltval;
		//cout << target << endl;
		//cout << finalResult << endl;
		//cout << "***FI: prevValue: " << target << " fiValue: " << finalResult << "\n";

		// Write to fi_log file
		/*
		ofstream fif;
		fif.open ("fi_log");
		fif << "prevValue: " << target << " fiValue: " << finalResult << "\n";
		fif << "Sampled Inst: " << injectionSampleCount;
		fif.close();	
		*/
		FILE* fi_f = fopen("fi_log", "a");
		fprintf(fi_f, "preValue: %u fiValue: %u\n", target2.intval, target3.intval);
		fprintf(fi_f, "preValue: %.16f fiValue: %.16f\n", target, finalResult);
		fprintf(fi_f, "Sampled Inst: %lld\n", injectionSampleCount);
		fprintf(fi_f, "Static Index: %d\n", staticIndex);
		fprintf(fi_f, "Layer NO.: %lld\n", layerNo);
		fprintf(fi_f, "Layer Type: %s\n", layerType.c_str());
		fclose(fi_f);

		currentFiFlag = 0;
	
		return finalResult;
	}

	return target;
}

