#!/bin/bash

#-------------------                                                                                                                                 
# Arguments                                                                                                               
#-------------------                                                                                                                                  

p = $1

START = (((($p - 1)) * 50))
STOP = (($p * 50))

while [ ${#x} -ne 6 ];
do
   x="0"$x
done

# #-------------------                                                                                                                                
# # Variable Defs                                                                                                                                      
# #-------------------                                                                                                                                  

SOURCE="/direct/phenix+u/chase/sphenix/PhotonIso/testmacros/Fun4All_G4_sPHENIX.C"

DESTINATION="/sphenix/user/chase/embedded_IsoAnalysis"

BACKGROUND_EVENT="/sphenix/sim/sim01/cd1_review/sHijing/fm_0-4/G4Hits_AuAu200_hijing_0-4fm_${START}_${STOP}.root"
#BACKGROUND_EVENT="/sphenix/sim/sim01/cd1_review/sHijing/fm_4-8/G4Hits_AuAu200_hijing_4-8fm_${START}_${STOP}.root"

SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"   
#SCRATCH_AREA="/phenix/scratch/chase" 

# #-------------------                                                                                                                                
# # Export Libraries                                                                                                                                   
# #-------------------                                                                                                                                  

source /phenix/u/chase/.cshrc

# #-------------------                                                                                                                                 
# # Set Scratch Area                                                                                                                                   
# #-------------------                                                                                                                                  

mkdir $SCRATCH_AREA/chase_PhotonIso
cp  $SOURCE $SCRATCH_AREA/chase_PhotonIso/

# #-------------------                                                                                                                                
# # Run Executable  
# #-------------------                                                                                                                                  

cd $SCRATCH_AREA/chase_PhotonIso
root -b -q Fun4All_G4_sPHENIX.C\(\"/sphenix/user/vassalli/XjPhi1/XjPhi1_pT5_${p}.dat\",\"BEST_TEST_EVER_DST_${p}.root\",1,\"$BACKGROUND_EVENT\"\) 
cp BEST_TEST_EVER_DST_${p}.root $DESTINATION


rm -r $SCRATCH_AREA/chase_PhotonIso


exit 0
