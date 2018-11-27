#*************************************************************#
# CMakeLists file for building SLCIOROOT / TRACEROOT
#          Authors : K. VERNET, P. NEDELEC
# ******** September 2018 - December 2018 ********
#*************************************************************#


     How to compile & install SLCIOROOT / TRACEROOT
========================================================
In the root AEgISGRPCAnalysor folder do as following

- mkdir build
- cd build
- cmake .. && make -j8
- make install


     How to run them and execute macro to analyse data
============================================================
1) Run SLCIOROOT / TRACEROOT
In the bin folder execute as following
- ./SLCIOROOT runNumber1 runNumber2 ... runNumberN
- ./TRACEROOT -r runNumber -e eventNumber -p padNumber

2) Execute a macro for analysis
- See README.txt in bin/analysis folder after installation


	      Configuration environnement
===========================================================
Go to bin/xml after installation
Modify file config.xml to choose your 
- SLCIO files path
- ROOT files path


Enjoy !!!
