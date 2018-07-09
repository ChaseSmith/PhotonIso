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
set OUT_FILE="/sphenix/user/vassalli/ClusterTest"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/PhotonIso/*"
#
# #-------------------                                                                                                                                
# # Export Libraries                                                                                                                                   
# #-------------------                                                                                                                                  
#
source /phenix/u/vassalli/.cshrc
#
# #-------------------                                                                                                                                 
# # Set Scratch Area                                                                                                                                   
# #-------------------                                                                                                                                  
#
mkdir $SCRATCH_AREA/fran_PhotonIso
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/fran_PhotonIso/
#
# #-------------------                                                                                                                                
# # Run Executable  
# #-------------------                                                                                                                                  
#
cd $SCRATCH_AREA/fran_PhotonIso
root -b -q Fun4All_G4_sPHENIX.C\(\"/sphenix/user/vassalli/XjPhi1/XjPhi1_pT5_${1}.dat\",\"out${1}ClusterTree.root\"\) 
cp out${1}ClusterTree.root $OUT_FILE
#
#
rm -r $SCRATCH_AREA/fran_PhotonIso
#
#
exit 0
