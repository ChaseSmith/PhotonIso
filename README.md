# PhotonIso
analysis: folder which contains IsoPlotter.C
  IsoPlotter.C is analysis code to plot isolation energy outputted in treemaker, to test module
  
RunCondor: folder which contains shell script and submit file to submit Fun4All on condor

testmacros: folder which contains all modules needed to run Fun4All
  Fun4All is configured to run the ClusterIso and TreeMaker which tests ClusterIso Class

source: contains all files for ClusterIso Class
  ClusterIso.cc: source code for ClusterIso class, finds isolation energy of every cluster in EMCal
  
