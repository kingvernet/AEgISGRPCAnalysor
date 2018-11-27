//*************************************************************#
// gRPCUtility.cxx file
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include "GRPCUtility.h"
#include "lcio.h"
#include <iostream>
#include "tinyxml.h"


using namespace lcio;
using namespace std;

GRPCTraceUtility::GRPCTraceUtility() {
	statusOk = true;
	mDoc = new TiXmlDocument;
}
GRPCTraceUtility::GRPCTraceUtility(int argc, char** argv) {
	runNumber = 0;
	eventNumber = 0;
	padNumber = 0;
	statusOk = true;
	mDoc = new TiXmlDocument;

	if(argc < 2) {
		statusOk = false;
		cout << "How to use : " << endl;
		cout << "./GRPCTRACE -r runNumber -e eventNumber -p padNumber" << endl;		
		return;
	}

	for(int i = 1; i < argc - 1; i += 2) {
		if(strcmp(argv[i], "-r") == 0) {
			try {
				runNumber = atoi(argv[i + 1]);
			}
			catch(IOException& e) {
				HowToUse();
			}
		}
		else if(strcmp(argv[i], "-e") == 0) {
			try {
				eventNumber = atoi(argv[i + 1]);
			}
			catch(IOException& e) {				
				HowToUse();
			}
		}
		else if(strcmp(argv[i], "-p") == 0) {
			try {
				padNumber = atoi(argv[i + 1]);
			}
			catch(IOException& e) {
				HowToUse();
			}
		}
		else {
			SynthaxUnknown();
		}
	}
}
GRPCTraceUtility::~GRPCTraceUtility() {}

string GRPCTraceUtility::GetPathFiles(string nodeName) {
	string xmlPathFile = "xml/config.xml";
	string childNodeName = "pathFiles";
	
	
	//TiXmlDocument* mDoc = new TiXmlDocument;
	bool loadOkay = mDoc->LoadFile(xmlPathFile.c_str());

	if(!loadOkay) {
		std::cout << "Error in file [" << xmlPathFile 
			<< " at row : " << mDoc->ErrorRow() << " at col : " << mDoc->ErrorCol() << "] : "
         		<< mDoc->ErrorDesc() << endl;
		return NULL;
  	}
  	TiXmlElement* root = mDoc->RootElement();
  	if(root == NULL || strcmp(root->Value(), "config") != 0) {
    		std::cout << "No root tag <config>...</config> found in " << xmlPathFile << endl;
		return NULL;
  	}

  	TiXmlNode* node = NULL;
	
	node = root->FirstChild(nodeName.c_str());
	if(node != NULL) {
		node = node->FirstChild(childNodeName.c_str())->FirstChild();
		return node->Value();
	}
	else {
		std::cout << "No node <" << nodeName << ">...</" << nodeName << "> found in " << xmlPathFile << endl;
		return NULL;
	}
	
	return NULL;
}
