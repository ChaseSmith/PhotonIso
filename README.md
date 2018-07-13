# PhotonIso  
source: contains all files for TestClusterIso Class...
  TestClusterIso.cc: finds isolation energy of every cluster in EMCal, build this - it gets called in Fun4All_G4_sPHENIX.C
 
treesource: contains all files for TreeMaker Class...
  TreeMaker.C is a small module for testing ClusterIso class, build this - it gets called in Fun4All_G4_sPHENIX.C

testmacros: folder which contains all modules needed to run Fun4All...
  Use root to run Fun4All_G4_sPHENIX.C with input HEPMC file and output DST file to simulate collisions in the sPHENIX detector
  
RunCondor: folder which contains shell script and submit file to submit Fun4All on condor

analysis: folder which contains IsoPlotter.C...
  IsoPlotter.C is analysis code to plot isolation energy outputted by running Fun4All
