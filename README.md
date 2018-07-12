# PhotonIso
analysis: folder which contains IsoPlotter.C...
  IsoPlotter.C is analysis code to plot isolation energy outputted in treemaker, to test module
  
RunCondor: folder which contains shell script and submit file to submit Fun4All on condor

testmacros: folder which contains all modules needed to run Fun4All...
  Fun4All is configured to run the TestClusterIso and TreeMaker which tests TestClusterIso Class

source: contains all files for TestClusterIso Class...
  TestClusterIso.cc: source code for TestClusterIso class, finds isolation energy of every cluster in EMCal
 
treesource: contains all files for TreeMaker Class...
  TreeMaker.C is a small module for testing ClusterIso class
