#!/bin/csh

#-------------------                                                                                                                                 
# Arguments                                                                                                               
#-------------------                                                                                                                                  

set p = $1

# #-------------------                                                                                                                                
# # Variable Defs                                                                                                                                      
# #-------------------                                                                                                                                  

set EVENTS = 100

set INPUT="/sphenix/user/chase/XjPhi_separated/XjPhi_pT5_"$1".dat" #for direct only

set OUTPUT1="/sphenix/user/vassalli/singlesamples/pi5/pi0_DST_"$1".root"
set OUTPUT2="/sphenix/user/vassalli/singlesamples/piMinus5/piminus_DST_"$1".root"
set OUTPUT3="/sphenix/user/vassalli/singlesamples/e5/e_DST_"$1".root"
#set OUTPUT4="/sphenix/user/chase/clusterProbTest/photon_output_"$1".root"

set PI0="pi0"
set PIMINUS="pi-"
set E="e-"
set PHOTON="gamma"

# #-------------------                                                                                                                                
# # Export Libraries                                                                                                                                   
# #-------------------                                                                                                                                  

source /phenix/u/chase/.cshrc

# #-------------------                                                                                                                                
# # Run Executable  
# #-------------------                                                                                                                                  

echo root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT1\",\"$PI0\"\) 
time root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT1\",\"$PI0\"\) 

echo root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT2\",\"$PIMINUS\"\) 
time root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT2\",\"$PIMINUS\"\) 

echo root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT3\",\"$E\"\) 
time root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT3\",\"$E\"\) 

#echo root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT4\",\"$PHOTON\"\) 
#time root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT4\",\"$PHOTON\"\) 


exit 0
