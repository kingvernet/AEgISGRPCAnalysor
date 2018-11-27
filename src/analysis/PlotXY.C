#include "include/AEgISGRPCUtility.h"

TFile* rootFile;
TTree* treeFile;

void DoPlotXY(UInt_t runNumber = 0, UInt_t dif = 0) {
	TCanvas *difCanvas = new TCanvas("difCanvas", "Distribution of x vs y", 1300, 700);
	difCanvas->SetFillColor(42);
   	difCanvas->SetGrid();
   	gStyle->SetOptStat(0);

	difCanvas->Clear();

	difCanvas->cd();
	string cond = "dif == " + std::to_string(dif);
	
	auto histoTitle = "Distribution x vs y for run " + std::to_string(runNumber) + " *** DIF : " + std::to_string(dif) + " ***";
	TH2F* hxy = new TH2F("hxy", histoTitle.c_str(), 48, -1.0, 50.0, 32, -1.0, 34.0);
	
	treeFile->Draw("x:y>>hxy", cond.c_str(), "HIST");
	hxy->SetDirectory(gROOT);
	hxy->Draw("colz");

	difCanvas->Modified();
   	difCanvas->Update();

	if(rootFile->IsOpen()) {
		rootFile->Close();
		rootFile->Delete();
	}
}
void PlotXY(UInt_t runNumber = 0, UInt_t dif = 0, Int_t eventNumber = -1, UInt_t padNumber = 0) {
	if(runNumber <= 0 || dif <= 0) {
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		cout << "How to use ?" << endl;
		cout << "1) You should pass a positive integer parameter not null for the run Number" << endl;
		cout << "2) You should pass a positive integer parameter not null for dif" << endl;
		cout << "3) You can pass two optional positive integer parameters for eventNumber and padNumber" << endl;
		cout << "========================================================================" << endl;
		cout << "Example .x PlotXY.C(741179, 68, -1) or PlotXY.C(741179, 68, 1000, 4)" << endl;
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		return;
	}

	string filePath;
	if(eventNumber >= 0) {
		filePath = GetRootPathFiles() + "GRPC_" + std::to_string(runNumber) + "_" + std::to_string(eventNumber) + "_" + std::to_string(padNumber) + ".root";
	}
	else {
		filePath = GetRootPathFiles() + "GRPC_" + std::to_string(runNumber) + ".root";
	}
	// Get root file and the tree
   	rootFile = new TFile(filePath.c_str());
	if(rootFile->IsZombie()) {
		
		cout << "The file " << filePath + " doesn't exist" << endl;
		return;
	}
   	treeFile = (TTree*) rootFile->Get("RawHits");

	DoPlotXY(runNumber, dif);
}
