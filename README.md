# PhotonIso
ClusterIso is a class for calculating the isolation energy of a cluster during online Fun4All analysis.  It can be run after the clusterizer or if a global vertex is used, after the vertex is calculated. 
A .C and .h are included for the class.  It can be initialized with a pT min and the size of the isolation cone.  The linkdef is attached for ease of compling.  
The Treemaker class is an anaylsis/output module for Fun4All.  It is complied with the ClusterIso and is intended to provide a way to test the ClusterIso. It only makes a root tree of the eT. 
PhotonIsolationMaker is currently deprecated however in further production will be a class to recontructed isolated photons from Au+Au G4 
The config autogen and make files are working on RCF.
A Manager is being made to call the classes in a Fun4All system of this. 
The .csh and submit files can be used to make condor tests.  The sPHENIX simulation is run my Fun4All_G4_sPHENIX.C which is editted to use the TreeMaker and ClusterIso. 
