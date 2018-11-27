//*************************************************************#
// TRACEROOT.cxx file for building TRACEROOT
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include "TRACEROOT.h"
#include "GRPCUtility.h"

using namespace std;

string IN_FILES_PATH; // IN path to get ROOT file to analyse
string OUT_FILES_PATH; // OUT path to put ROOT file
UInt_t runNumber; // RUN number 
UInt_t eventNumber; // EVENT number to analyse
UInt_t padNumber; // PAD number to get a trace
Bool_t statusOk = true;

int main(int argc, char** argv) {
	GRPCTraceUtility gRPCTraceUtility(argc, argv);
	if(!gRPCTraceUtility.GetStatus() || !statusOk)
		return -1;


	IN_FILES_PATH = gRPCTraceUtility.GetPathFiles("root");
	OUT_FILES_PATH = gRPCTraceUtility.GetPathFiles("root");
	runNumber = gRPCTraceUtility.GetRunNumber();
	eventNumber = gRPCTraceUtility.GetEventNumber();
	padNumber = gRPCTraceUtility.GetPadNumber();
	statusOk = gRPCTraceUtility.GetStatus();
	cout << "Run Number : " << runNumber << endl << 
		"Event number : " << eventNumber << endl << 
		"Pad Number : " << padNumber << endl << 
		"Status ok : " << statusOk << endl << 
		"In path files : " << IN_FILES_PATH << endl << 
		"Out path files : " << OUT_FILES_PATH << endl;

	TRACEROOT tRACEROOT(runNumber, eventNumber, padNumber);
	if(statusOk)
		tRACEROOT.GetData();

  	return 0;
}

TRACEROOT::TRACEROOT(UInt_t runNumber, UInt_t eventNumber, UInt_t padNumber) {
	mRunNumber = runNumber;
	mEventNumber = eventNumber;
	mPadNumber = padNumber;

	string filePath = IN_FILES_PATH + "GRPC_" + std::to_string(mRunNumber) + ".root";
	// Get root file and the tree
   	mFile = new TFile(filePath.c_str());
	if(!(mFile->IsOpen()) || mFile->IsZombie()) {
		statusOk = false;
		cout << "The file GRPC_" + std::to_string(mRunNumber) + ".root does not exist" << endl;
		exit(1);
	}
   	
	mTree = (TTree*) mFile->Get("RawHits");
	
	mTree->SetBranchAddress("event", &mEvent);
	mTree->SetBranchAddress("timeDigit", &mTimeDigit);
	mTree->SetBranchAddress("time", &mTime);
	mTree->SetBranchAddress("absTime", &mAbsTime);
	mTree->SetBranchAddress("dif", &mDif);
	mTree->SetBranchAddress("asic", &mAsic);
	mTree->SetBranchAddress("chan", &mChan);
	mTree->SetBranchAddress("x", &mX);
	mTree->SetBranchAddress("y", &mY);
	mTree->SetBranchAddress("z", &mZ);
	mTree->SetBranchAddress("threshold", &mThreshold);
	mTree->SetBranchAddress("frame", &mFrame);

	mEntries = mTree->GetEntries();// get the number of entries
	cout << mEntries << " entries found" << endl;

	// new ROOT file and tree
	filePath = OUT_FILES_PATH + "GRPC_" + std::to_string(mRunNumber) + "_" + std::to_string(mEventNumber) + "_" + std::to_string(mPadNumber) + ".root";
	_mFile = new TFile(filePath.c_str(), "RECREATE");
	if(!(_mFile->IsOpen()) || _mFile->IsZombie()) {
		statusOk = false;
		cout << "Can't create file GRPC_" + std::to_string(mRunNumber) + "_" + std::to_string(mEventNumber) + "_" + std::to_string(mPadNumber) + ".root" << endl;
		exit(1);
	}

	_mTree = new TTree("RawHits", "RawHits");
	
	_mTree->Branch("event", &_mEvent, "_mEvent/i");
	_mTree->Branch("timeDigit", &_mTimeDigit, "_mTimeDigit/i");
	_mTree->Branch("time", &_mTime, "_mTime/i");
	_mTree->Branch("absTime", &_mAbsTime, "_mAbsTime/i");
	_mTree->Branch("dif", &_mDif, "_mDif/i");
	_mTree->Branch("asic", &_mAsic, "_mAsic/i");
	_mTree->Branch("chan", &_mChan, "_mChan/i");
	_mTree->Branch("x", &_mX, "_mX/i");
	_mTree->Branch("y", &_mY, "_mY/i");
	_mTree->Branch("z", &_mZ, "_mZ/i");
	_mTree->Branch("threshold", &_mThreshold, "_mThreshold/i");
	_mTree->Branch("frame", &_mFrame, "_mFrame/i");
}
TRACEROOT::~TRACEROOT() {
	//TXTFile.close();
	if (_mFile) {
		_mTree->Write();
		delete _mTree;
		delete _mFile;
    	}
}
void TRACEROOT::ClearData() {
	X_POINT_1.clear();
	Y_POINT_1.clear();
	Z_POINT_1.clear();
	X_POINT_2.clear();
	Y_POINT_2.clear();
	Z_POINT_2.clear();
	X_POINT_3.clear();
	Y_POINT_3.clear();
	Z_POINT_3.clear();

	gRPCInfos.clear();
}
void TRACEROOT::AddData(GRPCInfos gRPCInfo) {
	if(gRPCInfo.grpcDif == DIF1) {
		X_POINT_1.push_back(mY);
		Y_POINT_1.push_back(mX);
		Z_POINT_1.push_back(mZ);
		//Z_POINT_1.push_back(1);
	}
	if(gRPCInfo.grpcDif == DIF2) {
		X_POINT_2.push_back(mY);
		Y_POINT_2.push_back(mX);
		Z_POINT_2.push_back(mZ);
		//Z_POINT_2.push_back(2);
	}
	if(gRPCInfo.grpcDif == DIF3) {
		X_POINT_3.push_back(mY);
		Y_POINT_3.push_back(mX);
		Z_POINT_3.push_back(mZ);
		//Z_POINT_3.push_back(3);
	}

	gRPCInfos.push_back(gRPCInfo);
}
/*
Bool_t GRPCTRACE::HasTrace() {
	for(UInt_t i = 0; i < X_POINT_1.size(); i++) {
		for(UInt_t k = 0; k < X_POINT_3.size(); k++) {
			if(abs(X_POINT_1.at(i) - X_POINT_3.at(k)) <= mPadNumber &&
                        	abs(Y_POINT_1.at(i) - Y_POINT_3.at(k)) <= mPadNumber) {								
			
				return true;				
			}
		}
	}
	return false;
}
*/
Bool_t TRACEROOT::HasTrace() {
	UInt_t mBigPadNumber = mPadNumber;
	for(UInt_t i = 0; i < X_POINT_1.size(); i++) {
		for(UInt_t j = 0; j < X_POINT_2.size(); j++) {
			if(abs(X_POINT_1.at(i) - X_POINT_2.at(j)) <= mPadNumber &&
                        	abs(Y_POINT_1.at(i) - Y_POINT_2.at(j)) <= mPadNumber) {
				for(UInt_t k = 0; k < X_POINT_3.size(); k++) {
					mBigPadNumber = mPadNumber * (Z_POINT_3.at(k) - Z_POINT_1.at(i)) / (Z_POINT_2.at(j) - Z_POINT_1.at(i));
                                        if(abs(X_POINT_1.at(i) - X_POINT_3.at(k)) <= mBigPadNumber &&
                                                abs(Y_POINT_1.at(i) - Y_POINT_3.at(k)) <= mBigPadNumber) {
			
						return true;
					}
					if(X_POINT_3.at(k) + mBigPadNumber > 32 || Y_POINT_3.at(k) + mBigPadNumber > 48) { // out of the chamber's domain
						return true;
					}
				}				
			
				return true;				
			}
		}
	}
	return false;
}


void TRACEROOT::SaveData(UInt_t timeDigit) {
	for(UInt_t i = 0; i < gRPCInfos.size(); i++) {
		_mTimeDigit = timeDigit;
		_mEvent = gRPCInfos[i].grpcEvent;
		_mTime = gRPCInfos[i].grpcTime;
		_mAbsTime = gRPCInfos[i].grpcAbsTime;
		_mDif = gRPCInfos[i].grpcDif;
		_mAsic = gRPCInfos[i].grpcAsic;
		_mChan = gRPCInfos[i].grpcChannel;
		_mX = gRPCInfos[i].grpcXPos;
		_mY = gRPCInfos[i].grpcYPos;
		_mZ = gRPCInfos[i].grpcZPos;
		_mThreshold = gRPCInfos[i].grpcThreshold;
		_mFrame = gRPCInfos[i].grpcFrame;

		// Fill root file
		_mTree->Fill();

		std::cout<<"TimeDigit : " << _mTimeDigit << "  Event : " << _mEvent << "  Time : " << _mTime  << "Abs Time : " << _mAbsTime << "  DIF : " << _mDif << "  ASIC : " << _mAsic << "  Chan : " << _mChan << "  X : " << _mX << "  Y : " << _mY << "  Z : " << _mZ << " Threshold : " << _mThreshold << "  Frame : " << _mFrame << std::endl;
	}
}

void TRACEROOT::GetData() {
	UInt_t currentTimeDigit = 0;
	UInt_t curentEvent = 1;
	GRPCInfos gRPCInfo;

	for(Long64_t i = 0; i < mEntries; i++) {	
		mTree->GetEntry(i);
		if(mEventNumber > 0 && mEvent > mEventNumber + 1) {
			cout << "================================" << endl;
			cout << "End of reading" << endl;
			cout << "================================" << endl;
			return;
		}

		if(currentTimeDigit != mTimeDigit || curentEvent != mEvent) {
			// verify if there are traces and save date to ROOT file if there are
			if(HasTrace()) {
				SaveData(currentTimeDigit);
			}
			ClearData();
		}
		gRPCInfo = {mEvent, mTime, mAbsTime, mDif, mAsic, mChan, mX, mY, mZ, mThreshold, mFrame};
		AddData(gRPCInfo); // add the first data
		currentTimeDigit = mTimeDigit; // hold the current timeDigit
		curentEvent = mEvent; // hold the current event
	}
}
