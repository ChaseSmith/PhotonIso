# PhotonIso  
## This repository is responsible for testing the sPHENIX ClusterIso Package and Photon ID packages

**source**: contains all files for TestClusterIso Class... 
- TestClusterIso.cc: finds isolation energy of every cluster in EMCal, build this - it gets called in Fun4All_G4_sPHENIX.C
 
**treesource**: contains all files for TreeMaker Class... 
- TreeMaker.C is a small module for testing ClusterIso as well as Photon ID Packages

**testClusterIso**: folder which contains all modules needed to run Fun4All... 
- Use root to run Fun4All_G4_sPHENIX.C with input HEPMC file and output DST file to simulate collisions in the sPHENIX detector 
- Used to test ClusterIso package
  
**testOldProb**: folder which contains all modules needed to run Fun4All... 
- Use root to run Fun4All_G4_sPHENIX.C with input HEPMC file and output DST file to simulate collisions in the sPHENIX detector 
- Used to test the old recycled phenix prob algorithm

**testProb**: folder which contains all modules needed to run Fun4All... 
- Use root to run Fun4All_G4_sPHENIX.C with input HEPMC file and output DST file to simulate collisions in the sPHENIX detector
- Used to produce single particle events for prob algorithm with different configuration
