#!/bin/csh

#-------------------                                                                                                                                 
# Arguments                                                                                                               
#-------------------                                                                                                                                  

set p = $1

@ tempSTART = ( $p * 50 ) 
@ tempSTOP = ( $tempSTART + 50 )
set START = "$tempSTART"
set STOP = "$tempSTOP"

while ( `echo $START | awk '{print length($0)}'` != 6 );
   set START = "0"$START
end

while ( `echo $STOP | awk '{print length($0)}'` != 6 );
   set STOP = "0"$STOP
end

# #-------------------                                                                                                                                
# # Variable Defs                                                                                                                                      
# #-------------------                                                                                                                                  

set EVENTS = 50

set INPUT="/sphenix/user/vassalli/XjPhi1/XjPhi1_pT5_"$1".dat"
#set OUTPUT="/sphenix/user/chase/embedded_IsoAnalysis/0-4fm/XjPhi1_Hijing_embed_0-4fm_output_"$p".root"
#set EMBED="/sphenix/sim/sim01/cd1_review/sHijing/fm_0-4/G4Hits_AuAu200_hijing_0-4fm_"$START"_"$STOP".root"
#set OUTPUT="/sphenix/user/chase/embedded_IsoAnalysis/4-8fm/XjPhi1_Hijing_embed_4-8fm_output_"$p".root"
set EMBED="/sphenix/sim/sim01/cd1_review/sHijing/fm_4-8/G4Hits_AuAu200_hijing_4-8fm_"$START"_"$STOP".root"
set OUTPUT="/sphenix/user/chase/embedded_IsoAnalysis/no_embedding/XjPhi1_Hijing_not_embedded_output_"$p".root"

# #-------------------                                                                                                                                
# # Export Libraries                                                                                                                                   
# #-------------------                                                                                                                                  

source /phenix/u/chase/.cshrc

# #-------------------                                                                                                                                
# # Run Executable  
# #-------------------                                                                                                                                  

echo root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT\",\"$EMBED\"\) 
time root -b -q -l Fun4All_G4_sPHENIX.C\($EVENTS,\"$INPUT\",\"$OUTPUT\",\"$EMBED\"\) 

exit 0
