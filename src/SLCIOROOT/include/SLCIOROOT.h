//*************************************************************#
// CMakeLists file for building SLCIOROOT
// Authors : K. VERNET, P. NEDELEC
//*************************************************************#
#include "lcio.h"
#include "IO/LCRunListener.h"
#include "IO/LCEventListener.h"
#include <fstream>

#include "TFile.h"
#include "TTree.h"

UInt_t DIF1 = 68, DIF2 = 112, DIF3 = 104; // verify order

using namespace lcio;

struct GRPCInfos {
	int grpcEvent;
	int grpcTime;
	int grpcAbsTime;
	unsigned int grpcDif;
	unsigned int grpcAsic;
	unsigned int grpcChannel;
	
	unsigned int grpcXPos;
	unsigned int grpcYPos;
	unsigned int grpcZPos;
	
	unsigned int grpcThreshold;
	unsigned int grpcFrame;
};
struct Order
{
   bool operator() (const GRPCInfos & lhs, const GRPCInfos & rhs) const
   {
      return lhs.grpcTime < rhs.grpcTime;
   }
};


class SLCIOROOT : public LCRunListener, public LCEventListener {
    public:
        SLCIOROOT();
        ~SLCIOROOT();
        void processRunHeader(LCRunHeader* run);
        void processEvent(LCEvent* evt);
        void modifyEvent(LCEvent * evt);
        void modifyRunHeader(LCRunHeader* run);
        void init(char** runNumbers);

        void AddRows(std::vector<GRPCInfos> gRPCInfos);
        void AddRow(unsigned int timeDigit, GRPCInfos gRPCInfo);

        unsigned long long getAbsoluteBCID(int* cb, uint32_t idx = 0);
        unsigned int getCellDif_id(int cell_id);
        unsigned int getCellAsic_id(int cell_id);
        unsigned int getCellChan_id(int cell_id);
        unsigned int* getPadIndex(unsigned int dif_id, unsigned int asic_id, unsigned int chan_id, unsigned int index[]);
        int getThreshold(int threshold);
    private:
        std::ofstream mTxtFile;
        int mEvent;
	int mTime;
	int mAbsTime;
        int mTimeDigit;
	unsigned int mDif;
	unsigned int mAsic;
	unsigned int mChan;
	
	unsigned int mXPos;
	unsigned int mYPos;
	unsigned int mZPos;
	
	unsigned int mThreshold;
	unsigned int mFrame;

        TFile* mFile;
        TTree* mTree;
};
