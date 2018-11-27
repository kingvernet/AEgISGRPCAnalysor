#include "include/AEgISGRPCUtility.h"

//#include <iostream>
//#include <string>

//using namespace std;


string OUT_FILES_PATH = "../data/"; // OUT path to put txt file

TFile* rootFile;
TTree* rootTree;
// branches
UInt_t tEvent, tTimeDigit, tTime, tAbsTime, tDif, tAsic, tChan, tX, tY, tZ, tThreshold, tFrame;
// number of entries
Long64_t tEntries;
// timeLow, timeHigh
Long64_t timeLow, timeHigh;
Long64_t traceTimeLow, traceTimeHigh;
// Ntime
Long64_t Ntime;


Bool_t SetRunInterval(UInt_t runNumber) {
	string filePath = GetRootPathFiles() + "GRPC_" + std::to_string(runNumber) + ".root";
	// Get root file and the tree
   	rootFile = new TFile(filePath.c_str());
	if(!rootFile->IsOpen() || rootFile->IsZombie()) {
		cout << "The file GRPC_" << runNumber << ".root doesn't exist" << endl;
		return false;
	}
   	
	rootTree = (TTree*) rootFile->Get("RawHits");
	
	rootTree->SetBranchAddress("event", &tEvent);
	rootTree->SetBranchAddress("timeDigit", &tTimeDigit);
	rootTree->SetBranchAddress("time", &tTime);
	rootTree->SetBranchAddress("absTime", &tAbsTime);
	
	tEntries = rootTree->GetEntries();// get the number of entries
	cout << tEntries << " entries found in total" << endl;

	// get timeLow, timeHigh
	rootTree->GetEntry(0);
	traceTimeLow = timeLow = tAbsTime + tTime * 0.000000020;
	rootTree->GetEntry(tEntries - 1);
	traceTimeHigh = timeHigh = tAbsTime + 1 + tTime * 0.000000020;

	Ntime = (timeHigh - timeLow);

	cout << "****************************************" << endl;
	cout << "First event absolute time for the run : " << GetTimeBySec(timeLow) << endl;
	cout << "Last event absolute time for the run  : " << GetTimeBySec(timeHigh - 1) << endl;
	cout << "****************************************" << endl;

	return true;
}

Bool_t InitData(UInt_t runNumber, UInt_t eventNumber, UInt_t padNumber) {
	string filePath = GetRootPathFiles() + "GRPC_" + std::to_string(runNumber) + "_" + std::to_string(eventNumber) + "_" + std::to_string(padNumber) + ".root";
	// Get root file and the tree
   	rootFile = new TFile(filePath.c_str());
	if(!rootFile->IsOpen() || rootFile->IsZombie()) {
		cout << "The file number GRPC_" << runNumber << "_" << eventNumber << "_" << padNumber << ".root doesn't exist" << endl;
		return false;
	}
   	
	rootTree = (TTree*) rootFile->Get("RawHits");
	
	rootTree->SetBranchAddress("event", &tEvent);
	rootTree->SetBranchAddress("timeDigit", &tTimeDigit);
	rootTree->SetBranchAddress("time", &tTime);
	rootTree->SetBranchAddress("absTime", &tAbsTime);
	/*	
	rootTree->SetBranchAddress("dif", &tDif);
	rootTree->SetBranchAddress("asic", &tAsic);
	rootTree->SetBranchAddress("chan", &tChan);
	rootTree->SetBranchAddress("x", &tX);
	rootTree->SetBranchAddress("y", &tY);
	rootTree->SetBranchAddress("z", &tZ);
	rootTree->SetBranchAddress("threshold", &tThreshold);
	rootTree->SetBranchAddress("frame", &tFrame);
	*/
	
	tEntries = rootTree->GetEntries();// get the number of entries
	cout << tEntries << " entries found" << endl;

	// get timeLow, timeHigh
	rootTree->GetEntry(0);
	traceTimeLow = tAbsTime + tTime * 0.000000020;
	rootTree->GetEntry(tEntries - 1);
	traceTimeHigh = tAbsTime + 1 + tTime * 0.000000020;

	cout << "****************************************" << endl;
	cout << "First pseudo trace absolute time : " << GetTimeBySec(traceTimeLow) << endl;
	cout << "Last pseudo trace absolute time  : " << GetTimeBySec(traceTimeHigh - 1) << endl;
	cout << "****************************************" << endl;

	return true;
}
void PlotDNDX(UInt_t runNumber, UInt_t withAbsTime, Bool_t forPseudoTrace) {
	TCanvas* canvasNX;
	TH1F* histoNX;
	string canvasTitle, histoTitle;

	if(forPseudoTrace) {
		canvasTitle = "Number of pseudo traces in absolute time => Run Number : " + std::to_string(runNumber);
		histoTitle = "Distribution of pseudo traces vs absolute time => Run Number : " + std::to_string(runNumber);
	}
	else {
		canvasTitle = "Distribution of absolute time => Run Number : " + std::to_string(runNumber);
		histoTitle = "Distribution of absolute time => Run Number : " + std::to_string(runNumber);
	}
	UInt_t timeClose = 0;
	if(withAbsTime != 0) {
		if(forPseudoTrace) {
			canvasTitle = "Number of pseudo traces in time => Run Number : " + std::to_string(runNumber);
			histoTitle = "Distribution of pseudo traces vs time => Run Number : " + std::to_string(runNumber);
		}
		else {
			canvasTitle = "Distribution of time => Run Number : " + std::to_string(runNumber);
			histoTitle = "Distribution of time => Run Number : " + std::to_string(runNumber);
		}
		timeClose = timeLow;
	}
	canvasNX = new TCanvas("canvasNX", canvasTitle.c_str(), 800, 500);
	histoNX = new TH1F("histoNX", histoTitle.c_str(), Ntime, timeLow - timeClose, timeHigh - timeClose); // histo dN/dt
	UInt_t currentTimeDigit = 0;
	UInt_t curentEvent = 1;
	

	for(Long64_t i = 0; i < tEntries; i++) {	
		rootTree->GetEntry(i);
		currentTimeDigit = tTimeDigit; // hold the current timeDigit
		curentEvent = tEvent; // hold the current event

		histoNX->Fill(tAbsTime + tTime * 0.000000020 - timeClose); // plot at difference timeClose

		Long64_t j = i + 1;
		while(j < tEntries) {
			rootTree->GetEntry(j);
			if(tTimeDigit != currentTimeDigit) {
				break;
			}
			j++;
		}
		i = j; // get current position
	}
	cout << "================================" << endl;
	cout << "End of reading" << endl;
	cout << "================================" << endl;

	canvasNX->cd();
	histoNX->Draw("HIST");

	return;
}
void PlotAbsTime(UInt_t runNumber, UInt_t eventNumber, UInt_t padNumber, UInt_t withAbsTime = 0) {
	if(SetRunInterval(runNumber)) {
		if(InitData(runNumber, eventNumber, padNumber)) {
			PlotDNDX(runNumber, withAbsTime, true);
		}
	}
}
void PlotAbsTime(UInt_t runNumber, UInt_t withAbsTime = 0) {
	if(SetRunInterval(runNumber)) {
		PlotDNDX(runNumber, withAbsTime, false);
	}
}
