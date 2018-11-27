//*************************************************************#
// GRPCUtility.h file
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include <iostream>
#include <string>
#include "tinyxml.h"

class GRPCTraceUtility {
public:
	GRPCTraceUtility();
	GRPCTraceUtility(int, char**);
	~GRPCTraceUtility();

	int GetRunNumber() const {return runNumber;}
	int GetEventNumber() const {return eventNumber;}
	int GetPadNumber() const {return padNumber;}
	bool GetStatus() const {return statusOk;}
	void SetStatus(bool statOk) {statusOk = statOk;}
	std::string GetPathFiles(std::string nodeName);
private:
	void HowToUse() {
		statusOk = false;
 		std::cout << "How to use : " << std::endl;
		std::cout << "./GRPCTRACE -r runNumber -e eventNumber -p padNumber" << std::endl;
	}
	void SynthaxUnknown() {
		statusOk = false;
		std::cout << "***** Syntax unknown *****" << std::endl;
	}
	int runNumber; // run number 
	int eventNumber; // event to number to analyse
	int padNumber; // pad number to get a trace
	bool statusOk; // to verify the char** parameters

	TiXmlDocument* mDoc;
};
