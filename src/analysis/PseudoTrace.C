#include "include/AEgISGRPCUtility.h"

UInt_t currentEvent = 1;
UInt_t currentTimeDigit = 0;
UInt_t lastEvent = 1;
UInt_t lastTimeDigit = 0;
TTree* treeFile; // tree file "RawHits"

TCanvas *difsPseudoTraceCanvas;
TPad* padDif;
TPad* padInfo;
TPad* padDif1;
TPad* padDif2;
TPad* padDif3;

// number of entries in the root file
Long64_t ENTRIES;

TTimer* plotTimer;
UInt_t goWait = 0, SeenWait = 2000;
string foundLine;

Long64_t POS = 0;

// tree branches
UInt_t event;
UInt_t timeDigit;
UInt_t dif;
UInt_t x, y, z;

// dif numbers
UInt_t DIF1 = 68, DIF2 = 112, DIF3 = 104; // verify order

// number of found traces
Long64_t TRACESFOUND = 0;

// number of pads
UInt_t padPlot = 0;

// must stop
Bool_t mustStop = false;

void soundLineFound()
{
   //gSystem->Exec("sudo apt-get install sox libsox-fmt-all");
   gSystem->Exec("play audio/beep-07.mp3");
}


bool PlotLine() {
	TGraph2D * gr = new TGraph2D();

	std::vector<Double_t> X_POINT_1, Y_POINT_1, Z_POINT_1, X_POINT_2, Y_POINT_2, Z_POINT_2, X_POINT_3, Y_POINT_3, Z_POINT_3;
	Bool_t witness = false;
	UInt_t _pos = 0;
	Bool_t hasLine = false;

	for(Long64_t i = POS; i < ENTRIES; i++) {	
		treeFile->GetEntry(i);

		if(event == currentEvent && timeDigit == currentTimeDigit) {
			gr->SetPoint(_pos, y, x, z);
			if(dif == DIF1) {
				X_POINT_1.push_back(y);
				Y_POINT_1.push_back(x);
				Z_POINT_1.push_back(z);
			}
			if(dif == DIF2) {
				X_POINT_2.push_back(y);
				Y_POINT_2.push_back(x);
				Z_POINT_2.push_back(z);
			}
			if(dif == DIF3) {
				X_POINT_3.push_back(y);
				Y_POINT_3.push_back(x);
				Z_POINT_3.push_back(z);
			}

			_pos++;
			witness = true;			
		}
		else if(witness) {
			POS = i;
			
			break;
		}
	}

	// to fix axis
	TGraph2D * grInit = new TGraph2D();
	grInit->SetPoint(0, -1.0, -1.0, 0.9);
	grInit->SetPoint(1 , 50.0, 34.0, 3.1);	
	grInit->SetMarkerStyle(20);
	grInit->SetMarkerColor(kRed);
	grInit->SetMarkerSize(1.5);
	grInit->Draw("P");

   	if(_pos > 0) {
		gr->Draw("same p0");
	}
	
	
	TPolyLine3D* grLine;
	UInt_t mBigPadPlot = padPlot;
	for(UInt_t i = 0; i < X_POINT_1.size(); i++) {
		for(UInt_t j = 0; j < X_POINT_2.size(); j++) {
			if(abs(X_POINT_1.at(i) - X_POINT_2.at(j)) <= padPlot &&
                        	abs(Y_POINT_1.at(i) - Y_POINT_2.at(j)) <= padPlot) {
				for(UInt_t k = 0; k < X_POINT_3.size(); k++) {
					mBigPadPlot = padPlot * (Z_POINT_3.at(k) - Z_POINT_1.at(i)) / (Z_POINT_2.at(j) - Z_POINT_1.at(i));
                                        if(abs(X_POINT_1.at(i) - X_POINT_3.at(k)) <= mBigPadPlot &&
                                                abs(Y_POINT_1.at(i) - Y_POINT_3.at(k)) <= mBigPadPlot) {
						grLine = new TPolyLine3D(2);
						grLine->SetPoint(0 , X_POINT_1.at(i), Y_POINT_1.at(i), Z_POINT_1.at(i));
						grLine->SetPoint(1 , X_POINT_3.at(k), Y_POINT_3.at(k), Z_POINT_3.at(k));
						grLine->Draw("same a*");
						return true;
					}
					if(X_POINT_3.at(k) + mBigPadPlot > 32 || X_POINT_3.at(k) - mBigPadPlot < 0 || 
						Y_POINT_3.at(k) + mBigPadPlot > 48 || Y_POINT_3.at(k) - mBigPadPlot < 0) { // out of the chamber's domain
						grLine = new TPolyLine3D(2);
						grLine->SetPoint(0 , X_POINT_1.at(i), Y_POINT_1.at(i), Z_POINT_1.at(i));
						grLine->SetPoint(1 , X_POINT_3.at(k), Y_POINT_3.at(k), Z_POINT_3.at(k));
						//grLine->Draw("same a*");
						hasLine = true;
					}
				}				
				grLine = new TPolyLine3D(2);
				grLine->SetPoint(0 , X_POINT_1.at(i), Y_POINT_1.at(i), Z_POINT_1.at(i));
				grLine->SetPoint(1 , X_POINT_2.at(j), Y_POINT_2.at(j), Z_POINT_2.at(j));
				grLine->Draw("same a*");
				return true;				
			}
		}
	}
	if(hasLine) { // pseudo trace out of the chamber's domain
		grLine->Draw("same a*");
	}
	return hasLine;
}


void GRPCPlotDif(string cond) {
	Long64_t longX, longY;
	padInfo->cd();
	bool hasLine = PlotLine();
	
	padDif->cd();
	padDif1->cd();
	longX = treeFile->Draw("z:x>>hxy1(32, -1.0, 34.0, 30, 0.9, 3.1)", cond.c_str(), "colz");

	padDif2->cd();
	longY = treeFile->Draw("z:y>>hxy2(48, -1.0, 50.0, 30, 0.9, 3.1)", cond.c_str(), "colz");
	
	plotTimer->SetTime(goWait);

	
	if(longX > 0 || longY > 0) {
		difsPseudoTraceCanvas->Modified();
   		difsPseudoTraceCanvas->Update();

		if(hasLine) {
			soundLineFound();
			TRACESFOUND++;
			cout << endl << "===============================================" << endl;
			cout << TRACESFOUND << " trace(s) found." << endl;
			cout << endl << "**************************" << endl;
			cout << "Event : " << currentEvent << endl;
			cout << "TimeDigit : " << currentTimeDigit << endl;
			cout << endl << "**************************" << endl;
			cout << "===============================================" << endl;
			plotTimer->SetTime(SeenWait);
		}
	}
}


void DoPlotPseudoTrace() {
	//just in case the canvas has been deleted
	if (!gROOT->GetListOfCanvases()->FindObject("difsPseudoTraceCanvas")) {
		plotTimer->TurnOff();
		plotTimer->Delete();
		return;
	}
	
	treeFile->GetEntry(POS);
	currentEvent = event;
	currentTimeDigit = timeDigit;

	string cond = "event == " + std::to_string(currentEvent) + " && timeDigit == " + std::to_string(currentTimeDigit);
	
	GRPCPlotDif(cond);

	if(currentEvent == lastEvent && currentTimeDigit == lastTimeDigit) {
		cout << endl;
		cout << "==============================================================" << endl;
            	cout << "**** End of reading ****" << endl;
            	cout << TRACESFOUND << " trace(s) found" << endl;
            	cout << "==============================================================" << endl;
		plotTimer->TurnOff();
            	plotTimer->Delete();  
	}

	return;
}

void InitData(UInt_t runNumber) {	
	auto title = "AEgIS GRPC Pseudo-trace detecting for an event and a timeDigit   ->   Run Number = " + std::to_string(runNumber);
	difsPseudoTraceCanvas = new TCanvas("difsPseudoTraceCanvas", title.c_str(), 1300, 700);
	
	difsPseudoTraceCanvas->SetFillColor(42);
   	difsPseudoTraceCanvas->SetGrid();
   	gStyle->SetOptStat(0);

	// Inside this canvas, we create two pads
   	padDif = new TPad("padDif", "Dif plot", 0.01, 0.01, 0.4, 0.99);
   	padInfo = new TPad("padInfo","Info...", 0.42, 0.99, 0.99, 0.1);
   	padDif->SetFillColor(11);
   	padInfo->SetFillColor(11);
   	padDif->Draw();
   	padInfo->Draw();

	// A pad may contain other pads and graphics objects.
   	// We set the current pad to padDif.
   	// Note that the current pad is always highlighted.
   	padDif->cd();
   	padDif1 = new TPad("padDif1", "First subpad of padDif", 0.01, 0.99, 0.99, 0.505, 17, 3);
   	padDif2 = new TPad("padDif2", "Second subpad of padDif", 0.01, 0.495, 0.99, 0.01, 17, 3);
   	padDif1->Draw();
   	padDif2->Draw();

	treeFile->SetBranchAddress("event", &event);
	treeFile->SetBranchAddress("timeDigit", &timeDigit);
	treeFile->SetBranchAddress("dif", &dif);
	treeFile->SetBranchAddress("x", &x);
	treeFile->SetBranchAddress("y", &y);
	treeFile->SetBranchAddress("z", &z);

	ENTRIES = treeFile->GetEntries();// get the number of entries
	cout << ENTRIES << " entries found" << endl;

	// initialisation
	POS = 0;
	treeFile->GetEntry(POS);
	currentEvent = event;
	currentTimeDigit = timeDigit;
	// end of reading
	treeFile->GetEntry(ENTRIES - 1);
	lastEvent = event;
	lastTimeDigit = timeDigit;

	cout << "Begin at event : " << currentEvent << " timeDigit : " << currentTimeDigit << endl;
}

void PseudoTrace(UInt_t runNumber = 0, UInt_t eventNumber = 0, UInt_t padNumber = 0) {
        if(runNumber <= 0 ) {
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		cout << "How to use ?" << endl;
		cout << "You should pass : " << endl;
		cout << "1) A positive integer number not null for the run Number" << endl;
		cout << "2) Two optional positive integers as eventNumber,  padNumber" << endl;
		cout << "========================================================================" << endl;
		cout << "Example .x PlotDifsInTime.C(741179, 1000, 4)" << endl;
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		return;
	}
	currentEvent = 1;
	currentTimeDigit = 0;
	padPlot = padNumber;

	string filePath = GetRootPathFiles() + "GRPC_" + std::to_string(runNumber) + "_" + std::to_string(eventNumber) + "_" + std::to_string(padNumber) + ".root";
	// Get root file and the tree
   	TFile* rootFile = new TFile(filePath.c_str());
	if(rootFile->IsZombie()) {
		cout << "The file number " << "GRPC_" + std::to_string(runNumber) + "_" + std::to_string(eventNumber) + "_" + std::to_string(padNumber) + ".root doesn't exist" << endl;
		return;
	}
   	treeFile = (TTree*) rootFile->Get("RawHits");

	InitData(runNumber);

	plotTimer = new TTimer(goWait);
   	plotTimer->SetCommand("DoPlotPseudoTrace()");
   	plotTimer->TurnOn();
}

