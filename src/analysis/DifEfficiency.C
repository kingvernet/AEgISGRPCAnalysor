#include "include/AEgISGRPCUtility.h"

TFile* rootFile; // root file
TTree* treeFile; // tree file "RawHits"

// tree branches                                           =================================> (x, y)
UInt_t AEgIS_event;                               //      ||
UInt_t AEgIS_time;                                //      ||   // ================ DIF 1
UInt_t AEgIS_dif;                                 // z    ||   // ================ DIF 2
UInt_t AEgIS_x, AEgIS_y, AEgIS_z;                 //      ||   // ================ DIF 2
                                                  //      VV

UInt_t DIF1 = 68, DIF2 = 112, DIF3 = 104;

UInt_t FindHit(UInt_t dif, 
	vector<Int_t> X_POINT_1, vector<Int_t> Y_POINT_1, vector<Int_t> Z_POINT_1, 
	vector<Int_t> X_POINT_2, vector<Int_t> Y_POINT_2, vector<Int_t> Z_POINT_2, 
	vector<Int_t> X_POINT_3, vector<Int_t> Y_POINT_3, vector<Int_t> Z_POINT_3, Int_t pad) {
	Int_t x, y;
	Int_t pad1 = pad, pad2 = pad;
	if(dif == DIF1) {
		pad1 = pad2 = pad;
	}
	if(dif == DIF2) {
		pad1 = 2 * pad;
		pad2 = pad;
	}
	if(dif == DIF3) {
		pad1 = pad2 = pad;
	}

	if(X_POINT_1.size() * X_POINT_2.size() == 0) {
		return 2;
	}
	for(UInt_t i = 0; i < X_POINT_1.size(); i++) {
		for(UInt_t j = 0; j < X_POINT_2.size(); j++) {
			if(abs(X_POINT_1[i] - X_POINT_2[j]) <= pad1 && abs(Y_POINT_1[i] - Y_POINT_2[j]) <= pad1) {
				for(UInt_t k = 0; k < X_POINT_3.size(); k++) {
					x = X_POINT_1[i] + (X_POINT_2[j] - X_POINT_1[i]) * (Z_POINT_3[k] - Z_POINT_1[i]) / (Z_POINT_2[j] - Z_POINT_1[i]);
					y = Y_POINT_1[i] + (Y_POINT_2[j] - Y_POINT_1[i]) * (Z_POINT_3[k] - Z_POINT_1[i]) / (Z_POINT_2[j] - Z_POINT_1[i]);
					if(abs(x - X_POINT_3[k]) <= pad2 && abs(y - Y_POINT_3[k]) <= pad2) {
						return 1; // hit found
					}
					else if(x < 0 || x > 32 || y < 0 || y > 48) {
						return 2; // outside the chamber domain thus do not count
					}
				}
			}
		}
	}

	return 0;
}
void GetEfficiencies(Int_t pad) {	
	treeFile->SetBranchAddress("event", &AEgIS_event);
	treeFile->SetBranchAddress("time", &AEgIS_time);
	treeFile->SetBranchAddress("dif", &AEgIS_dif);
	treeFile->SetBranchAddress("x", &AEgIS_x);
	treeFile->SetBranchAddress("y", &AEgIS_y);
	treeFile->SetBranchAddress("z", &AEgIS_z);

	std::vector<Int_t> X_POINT_1, Y_POINT_1, Z_POINT_1, X_POINT_2, Y_POINT_2, Z_POINT_2, X_POINT_3, Y_POINT_3, Z_POINT_3;

	Long64_t hitsDIF1 = 0, hitsDIF2 = 0, hitsDIF3 = 0, hitsTotal1 = 0, hitsTotal2 = 0, hitsTotal3 = 0;
	Double_t EFF1 = 0.00, EFF2 = 0.00, EFF3 = 0.00;
	Long64_t runEntries = treeFile->GetEntries();	
	UInt_t currentEvent = 1;

	for(Long64_t i = 0; i < runEntries; i++) {
		treeFile->GetEntry(i);

		if(AEgIS_event == currentEvent) {
			if(AEgIS_dif == DIF1) {
				X_POINT_1.push_back(AEgIS_y);
				Y_POINT_1.push_back(AEgIS_x);
				Z_POINT_1.push_back(AEgIS_z);
			}
			if(AEgIS_dif == DIF2) {
				X_POINT_2.push_back(AEgIS_y);
				Y_POINT_2.push_back(AEgIS_x);
				Z_POINT_2.push_back(AEgIS_z);
			}
			if(AEgIS_dif == DIF3) {
				X_POINT_3.push_back(AEgIS_y);
				Y_POINT_3.push_back(AEgIS_x);
				Z_POINT_3.push_back(AEgIS_z);
			}
		}
		else {
			// incrementation
			if(X_POINT_1.size() > 0 || X_POINT_2.size() > 0 || X_POINT_3.size() > 0) {
				UInt_t qty_1 = FindHit(DIF1, 
						X_POINT_2, Y_POINT_2, Z_POINT_2, 
						X_POINT_3, Y_POINT_3, Z_POINT_3, 
						X_POINT_1, Y_POINT_1, Z_POINT_1, 
						pad);
				UInt_t qty_2 = FindHit(DIF2, 
						X_POINT_1, Y_POINT_1, Z_POINT_1, 
						X_POINT_3, Y_POINT_3, Z_POINT_3, 
						X_POINT_2, Y_POINT_2, Z_POINT_2, 
						pad);
				UInt_t qty_3 = FindHit(DIF3, 
						X_POINT_1, Y_POINT_1, Z_POINT_1, 
						X_POINT_2, Y_POINT_2, Z_POINT_2, 
						X_POINT_3, Y_POINT_3, Z_POINT_3, 
						pad);
				if(qty_1 < 2) {
					hitsTotal1++;
					hitsDIF1 += qty_1;
				}
				if(qty_2 < 2) {
					hitsTotal2++;
					hitsDIF2 += qty_2;
				}
				if(qty_3 < 2) {
					hitsTotal3++;
					hitsDIF3 += qty_3;
				}
			}
			currentEvent = AEgIS_event;
			X_POINT_1.clear();
			Y_POINT_1.clear();
			Z_POINT_1.clear();

			X_POINT_2.clear();
			Y_POINT_2.clear();
			Z_POINT_2.clear();

			X_POINT_3.clear();
			Y_POINT_3.clear();
			Z_POINT_3.clear();			
		}
	}
	if(hitsTotal1 != 0)
		EFF1 = 100.00 * hitsDIF1 / hitsTotal1;
	if(hitsTotal2 != 0)
		EFF2 = 100.00 * hitsDIF2 / hitsTotal2;
	if(hitsTotal3 != 0)
		EFF3 = 100.00 * hitsDIF3 / hitsTotal3;

	cout << "======================================================" << endl;
	cout << "Dif " << DIF1 << endl;
	cout << hitsDIF1 << " on a total of " << hitsTotal1 << " good event(s)" << endl;
	cout << "Efficiency : " << EFF1 << endl;

	cout << "======================================================" << endl;
	cout << "Dif " << DIF2 << endl;
	cout << hitsDIF2 << " on a total of " << hitsTotal2 << " good event(s)" << endl;
	cout << "Efficiency : " << EFF2 << endl;

	cout << "======================================================" << endl;
	cout << "Dif " << DIF3 << endl;
	cout << hitsDIF3 << " on a total of " << hitsTotal3 << " good event(s)" << endl;
	cout << "Efficiency : " << EFF3 << endl;
	cout << "======================================================" << endl;
}


void DifEfficiency(UInt_t runNumber = 0, Int_t eventNumber = -1, UInt_t padNumber = 0) {
        if(runNumber <= 0) {
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		cout << "How to use ?" << endl;
		cout << "You should pass : " << endl;
		cout << "1) A positive integer number not null for the run Number" << endl;
                cout << "2) Two optional positive integers as eventNumber and padNumber" << endl;
		cout << "========================================================================" << endl;
		cout << "Example .x DifEfficiencyTrig.C(741179, 1000, 4)" << endl;
		cout << "========================================================================" << endl;
		cout << "========================================================================" << endl;
		return;
	}
	
	auto title = "AEgIS GRPC Efficiency => Run Number = " + std::to_string(runNumber);
	TCanvas* difsCanvas;
	TPad* padDif1;
	TPad* padDif2;
	TPad* padDif3;

 	difsCanvas = new TCanvas("difsCanvas", title.c_str(), 600, 700);
	padDif1 = new TPad("padDif1", "", 0.01, 0.99, 0.99, 0.67, 17, 3);
   	padDif2 = new TPad("padDif2", "", 0.01, 0.66, 0.99, 0.32, 17, 3);
	padDif3 = new TPad("padDif3", "", 0.01, 0.31, 0.99, 0.01, 17, 3);
	padDif1->Draw();
	padDif2->Draw();
	padDif3->Draw();
	
	difsCanvas->SetFillColor(42);
   	difsCanvas->SetGrid();
   	gStyle->SetOptStat(0);

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
		cout << "The file " << filePath << " doesn't exist" << endl;
		return;
	}

   	treeFile = (TTree*) rootFile->Get("RawHits");

	string cond = "dif == " + std::to_string(DIF1);
	padDif1->cd(); // select padDif1
	treeFile->Draw("x:y>>hyxdif1(48, -1.0, 50.0, 32, -1.0, 34.0)", cond.c_str(), "colz");
	
	cond = "dif == " + std::to_string(DIF2);
	padDif2->cd(); // select padDif2
	treeFile->Draw("x:y>>hyxdif2(48, -1.0, 50.0, 32, -1.0, 34.0)", cond.c_str(), "colz");
	
	cond = "dif == " + std::to_string(DIF3);
	padDif3->cd(); // select padDif3

	treeFile->Draw("x:y>>hyxdif3(48, -1.0, 50.0, 32, -1.0, 34.0)", cond.c_str(), "colz");
	
	GetEfficiencies(padNumber);
}
