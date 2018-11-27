//*************************************************************#
// TRACEROOT.h file for building TRACEROOT
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include <fstream>

#include "TFile.h"
#include "TTree.h"

UInt_t DIF1 = 68, DIF2 = 112, DIF3 = 104; // verify order


struct GRPCInfos {
	UInt_t grpcEvent;
	UInt_t grpcTime;
	UInt_t grpcAbsTime;
	UInt_t grpcDif;
	UInt_t grpcAsic;
	UInt_t grpcChannel;
	
	UInt_t grpcXPos;
	UInt_t grpcYPos;
	UInt_t grpcZPos;
	
	UInt_t grpcThreshold;
	UInt_t grpcFrame;
};
struct Order
{
   bool operator() (const GRPCInfos & lhs, const GRPCInfos & rhs) const
   {
      return lhs.grpcTime < rhs.grpcTime;
   }
};


class TRACEROOT {
    public:
        TRACEROOT();
	TRACEROOT(UInt_t runNumber, UInt_t eventNumber, UInt_t padNumber);
        ~TRACEROOT();
	void ClearData();  // initialize the vectors
	void AddData(GRPCInfos gRPCInfo); // add data to vectors
	void GetData(); // get data trace
	Bool_t HasTrace(); // verify if data has trace
	void SaveData(UInt_t timeDigit); // save data trace

        void AddRows(std::vector<GRPCInfos> gRPCInfos);
        void AddRow(UInt_t timeDigit, GRPCInfos gRPCInfo);
	void AddRow(GRPCInfos gRPCInfo);
	void AddRow(UInt_t timeDigit);
    private:
        std::ofstream mTxtFile;

        TFile* mFile;
        TTree* mTree;
	TFile* _mFile;
	TTree* _mTree;
	UInt_t mRunNumber;
	UInt_t mEventNumber;
	UInt_t mPadNumber;
	
	// tree branches
	UInt_t mEvent, mTimeDigit, _mEvent, _mTimeDigit;
	UInt_t mTime, mAbsTime, _mTime, _mAbsTime;
	UInt_t mDif, mAsic, mChan, _mDif, _mAsic, _mChan;
	UInt_t mX, mY, mZ, _mX, _mY, _mZ;
	UInt_t mThreshold, mFrame, _mThreshold, _mFrame;

	// number of entries
	Long64_t mEntries;

	// vector to hold data
	std::vector<Double_t> X_POINT_1;
	std::vector<Double_t> Y_POINT_1;
	std::vector<Double_t> Z_POINT_1;
	std::vector<Double_t> X_POINT_2;
	std::vector<Double_t> Y_POINT_2;
	std::vector<Double_t> Z_POINT_2;
	std::vector<Double_t> X_POINT_3;
	std::vector<Double_t> Y_POINT_3;
	std::vector<Double_t> Z_POINT_3;
	// vector ROOT data
	std::vector<GRPCInfos> gRPCInfos;
};
