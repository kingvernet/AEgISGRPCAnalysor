//*************************************************************#
// SLCIOROOT.cxx file for building SLCIOROOT
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include "SLCIOROOT.h"

#include "lcio.h"
#include "EVENT/LCCollection.h"
#include "EVENT/RawCalorimeterHit.h"
#include "Mapping.h"
#include <iostream>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"

#include "GRPCUtility.h"


using namespace lcio;
using namespace std;

string IN_FILES_PATH; // IN path to get SLCIO file to analyse
string OUT_FILES_PATH; // OUT path to put ROOT file
string runNumber = ""; // RUN number 
Bool_t statusOk = true;

int main(int argc, char** argv) {
	GRPCTraceUtility gRPCTraceUtility;
	if(!gRPCTraceUtility.GetStatus() || !statusOk)
		return -1;

	IN_FILES_PATH = gRPCTraceUtility.GetPathFiles("slcio");
	OUT_FILES_PATH = gRPCTraceUtility.GetPathFiles("root");

	statusOk = gRPCTraceUtility.GetStatus();
	cout << "Status ok : " << statusOk << endl << 
		"In path files : " << IN_FILES_PATH << endl << 
		"Out path files : " << OUT_FILES_PATH << endl;



	string filePath;
	if(argc < 2) {
		cout << "How to use : " << endl;
		cout << "./SLCIOROOT runNumber" << endl;
		return -1;
	}
	for(int i = 1; i < argc; i++) {
		try {
			runNumber = argv[i];
					
			LCReader* lcReader = LCFactory::getInstance()->createLCReader();
			filePath = IN_FILES_PATH + "DHCAL_" + runNumber + "_I0_0.slcio";
			lcReader->open(filePath);

			SLCIOROOT sLCIOROOT;
  			lcReader->registerLCRunListener(&sLCIOROOT) ;
  			lcReader->registerLCEventListener(&sLCIOROOT);

  			lcReader->readStream();

  			lcReader->close();
		}
		catch(IOException& e){
			cout << "The file DHCAL_" + runNumber + "_I0_0.slcio does not exist !" << endl ;
		}
	}
	

  	return 0;
}

SLCIOROOT::SLCIOROOT(){
	try {
		mTxtFile.open(OUT_FILES_PATH + "GRPC_" + runNumber + ".txt");
		mTxtFile<<"=========== AEGIS GRPC [ K. VERNET & P. NEDELEC ] ============="<<std::endl;
		mTxtFile<<"=========================================================" <<std::endl;
  		mTxtFile<<"DIF_Id     ASIC_Id     CHAN_Id     X     Y     Z     TIME   ABS TIME    FRAME     EVENT" <<std::endl;
  		mTxtFile<<"===============================================================" <<std::endl;

  		std::string fileName = OUT_FILES_PATH + "GRPC_" + runNumber + ".root";
    		mFile = new TFile(fileName.c_str(), "RECREATE");
		mFile->IsOpen();

  		mTree = new TTree("RawHits", "RawHits");
	
		mTree->Branch("timeDigit", &mTimeDigit, "mTimeDigit/i"); // Time Id	
		mTree->Branch("event", &mEvent, "mEvent/i"); // Event
		mTree->Branch("time", &mTime, "mTime/i"); // Time
		mTree->Branch("absTime", &mAbsTime, "mAbsTime/i"); // Absolute Time

  		mTree->Branch("dif", &mDif, "mDif/i"); // DIF
  		mTree->Branch("asic", &mAsic, "mAsic/i"); // ASIC
  		mTree->Branch("chan", &mChan, "mChan/i");  // Channel

  		mTree->Branch("x", &mXPos, "mXPos/i"); // x Position
  		mTree->Branch("y", &mYPos, "mYPos/i"); // y Position
  		mTree->Branch("z", &mZPos, "mZPos/i"); // z Position
  	
  		mTree->Branch("threshold", &mThreshold, "mThreshold/i"); // Level
  		mTree->Branch("frame", &mFrame, "mFrame/i"); // Frame		
	}
	catch(IOException& e){
		cout << "Process initialisation failled." << endl ;
	}
}
SLCIOROOT::~SLCIOROOT(){
	mTxtFile.close();
	if(mFile) {
		mTree->Write();
		delete mTree;
		delete mFile;
    	}
}
void SLCIOROOT::processRunHeader(LCRunHeader* run) {
	cout << "  Run : " << run->getRunNumber() << " - " << run->getDetectorName() << " : " << run->getDescription() << endl;	
}   
void SLCIOROOT::processEvent(LCEvent* evt) {
	if (evt != nullptr) {
		try {
			std::vector<GRPCInfos> gRPCInfos;
			//int runNumber = evt->getRunNumber();
			int evtNumber = evt->getEventNumber();
			string dectName = evt->getDetectorName();
			int absTimeStamp = evt->getTimeStamp(); // Definition: ns since January 1, 1970, 00:00:00 GMT 
			//int evtWeight = evt->getWeight();


			LCCollection* col = evt->getCollection("DHCALRawHits");
			std::string colTypeName = col->getTypeName();
			//int colFlag = col->getFlag();
			int nHits =  col->getNumberOfElements();
			for( int ihit = 0 ; ihit < nHits ; ihit++ ) {
				auto *rawHit = dynamic_cast<RawCalorimeterHit*>(col->getElementAt(ihit)) ;
				if (rawHit != nullptr) {
					if (rawHit->getTimeStamp() < 0)
                        			continue;
				
					int CellID0 = rawHit->getCellID0();
					unsigned int difId = getCellDif_id(CellID0);
                			unsigned int asicId = getCellAsic_id(CellID0);
                			unsigned int chanId = getCellChan_id(CellID0);

					unsigned int pos[3] = {0, 0, 0};
                			getPadIndex(difId, asicId, chanId, pos);

					int timeStamp = rawHit->getTimeStamp();

					unsigned int colThreshold = static_cast<unsigned int>(getThreshold(rawHit->getAmplitude()));
					unsigned int colFrame = static_cast<unsigned int>(ihit);

					GRPCInfos gRPCInfo = {evtNumber, timeStamp, absTimeStamp, difId, asicId, chanId, pos[0], pos[1], pos[2], colThreshold, colFrame};
         				gRPCInfos.push_back(gRPCInfo);
				}
			}
			// Tri du vector grâce à la fonction std::sort
   			std::sort(gRPCInfos.begin(), gRPCInfos.end(), Order());
			// Add rows to root file
			AddRows(gRPCInfos);
		}
		catch (DataNotAvailableException &e) {
	      		std::cout << "Trigger skiped !!!!..."<<std::endl;
	   	}
	}
}
void SLCIOROOT::modifyEvent(LCEvent * evt) {
	//cout << "Modifying event..." << endl;
}
void SLCIOROOT::modifyRunHeader(LCRunHeader* run){
	//cout << "Modifying runHeader..." << endl;
}

void SLCIOROOT::AddRows(std::vector<GRPCInfos> gRPCInfos) {
	unsigned int timeNo = 0;
	if(gRPCInfos.size() > 0) {
		GRPCInfos gRPCInfo = {gRPCInfos[0].grpcEvent, gRPCInfos[0].grpcTime, gRPCInfos[0].grpcAbsTime, gRPCInfos[0].grpcDif, 
			gRPCInfos[0].grpcAsic, gRPCInfos[0].grpcChannel, gRPCInfos[0].grpcXPos, gRPCInfos[0].grpcYPos, 
			gRPCInfos[0].grpcZPos, gRPCInfos[0].grpcThreshold, gRPCInfos[0].grpcFrame};

		AddRow(timeNo, gRPCInfo);


		for(UInt_t i = 1; i < gRPCInfos.size(); i++) {			
			if(gRPCInfos[i].grpcTime != gRPCInfo.grpcTime) {
				gRPCInfo = {gRPCInfos[i].grpcEvent, gRPCInfos[i].grpcTime, gRPCInfos[i].grpcAbsTime, gRPCInfos[i].grpcDif, 
					gRPCInfos[i].grpcAsic, gRPCInfos[i].grpcChannel, gRPCInfos[i].grpcXPos, gRPCInfos[i].grpcYPos, 
					gRPCInfos[i].grpcZPos, gRPCInfos[i].grpcThreshold, gRPCInfos[i].grpcFrame};

				timeNo++;				
			}
			AddRow(timeNo, gRPCInfos[i]);
		}
	}
}
void SLCIOROOT::AddRow(unsigned int timeDigit, GRPCInfos gRPCInfo) {
	mTimeDigit = timeDigit;
	mEvent = gRPCInfo.grpcEvent;
	mTime = gRPCInfo.grpcTime;
	mAbsTime = gRPCInfo.grpcAbsTime;
	mDif = gRPCInfo.grpcDif;
	mAsic = gRPCInfo.grpcAsic;
	mChan = gRPCInfo.grpcChannel;
	mXPos = gRPCInfo.grpcXPos;
	mYPos = gRPCInfo.grpcYPos;
	//if(mDif == DIF1) mZPos = 1;
	//if(mDif == DIF2) mZPos = 2;
	//if(mDif == DIF3) mZPos = 3;
	mZPos = gRPCInfo.grpcZPos;

	mThreshold = gRPCInfo.grpcThreshold;
	mFrame = gRPCInfo.grpcFrame;

	// Fill root file
	mTree->Fill();

	std::cout<<"TimeDigit : " << mTimeDigit << "  Event : " << mEvent << "  Time : " << mTime << "  Abs Time : " << mAbsTime << "  DIF : " << mDif << "  ASIC : " << mAsic << "  Chan : " << mChan << "  X : " << mXPos << "  Y : " << mYPos << "  Z : " << mZPos << " Threshold : " << mThreshold << "  Frame : " << mFrame << std::endl;

	mTxtFile<<"TimeDigit : " << mTimeDigit << "  Event : " << mEvent << "  Time : " << mTime << "  Abs Time : " << mAbsTime << "  DIF : " << mDif << "  ASIC : " << mAsic << "  Chan : " << mChan << "  X : " << mXPos << "  Y : " << mYPos << "  Z : " << mZPos << " Threshold : " << mThreshold << "  Frame : " << mFrame << std::endl;

}

unsigned int SLCIOROOT::getCellDif_id(int cell_id) {
    return static_cast<unsigned int>(cell_id & 0xFF);
}
unsigned int SLCIOROOT::getCellAsic_id(int cell_id) {
    return static_cast<unsigned int>((cell_id & 0xFF00) >> 8);
}
unsigned int SLCIOROOT::getCellChan_id(int cell_id) {
    return static_cast<unsigned int>((cell_id & 0x3F0000) >> 16);
}
unsigned int* SLCIOROOT::getPadIndex(unsigned int dif_id, unsigned int asic_id, unsigned int chan_id, unsigned int index[]) {
	unsigned int x, y;
    	x = 32 - (mapping::MapJLargeHR2[chan_id] + mapping::AsicShiftJ[asic_id]);
    	y = 1 + mapping::MapILargeHR2[chan_id] + mapping::AsicShiftI[asic_id];

    	index[0] = x;
    	index[1] = y;
    	if(dif_id == DIF1)
    		index[2] = 1;
    	if(dif_id == DIF2)
    		index[2] = 2;
    	if(dif_id == DIF3)
    		index[2] = 3;

    	return index;
}
int SLCIOROOT::getThreshold(int threshold) {
	if(threshold == 1)
        	return 2;
    	else if(threshold == 2)
        	return 1;
    	else if(threshold == 3)
        	return 3;
	else
		return -1;
}
