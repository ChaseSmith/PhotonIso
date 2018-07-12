#!/bin/csh 
                                                                                                                                            

#-------------------                                                                                                                                 
# Arguments                                                                                                               
# #-------------------                                                                                                                                  
#
@ p = $1
#
# #-------------------                                                                                                                                
# # Variable Defs                                                                                                                                      
# #-------------------                                                                                                                                  
#
set OUT_FILE="/sphenix/user/chase/embedded_IsoAnalysis"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/chase/sphenix/PhotonIso/*"
#
# #-------------------                                                                                                                                
# # Export Libraries                                                                                                                                   
# #-------------------                                                                                                                                  
#
source /phenix/u/chase/.cshrc
#
# #-------------------                                                                                                                                 
# # Set Scratch Area                                                                                                                                   
# #-------------------                                                                                                                                  
#
mkdir $SCRATCH_AREA/chase_PhotonIso
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/chase_PhotonIso/
#
# #-------------------                                                                                                                                
# # Run Executable  
# #-------------------                                                                                                                                  
#
cd $SCRATCH_AREA/chase_PhotonIso
root -b -q Fun4All_G4_sPHENIX.C\(\"/sphenix/user/vassalli/XjPhi1/XjPhi1_pT5_${1}.dat\",\"Hijing.root\"\) 
cp out${1}ClusterTree.root $OUT_FILE
#
#
rm -r $SCRATCH_AREA/chase_PhotonIso
#
#
exit 0
