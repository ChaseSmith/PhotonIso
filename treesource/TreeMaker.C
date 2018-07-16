#include "TreeMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include <iostream>

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawTowerGeomContainer.h>

#include <g4main/PHG4Particle.h>

#include <g4main/PHG4VtxPoint.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>



TreeMaker::TreeMaker(const std::string &name, int embed_id) : SubsysReco("IsoTree")
{

  _foutname = name;
  _embed_id = embed_id;

}

int TreeMaker::Init(PHCompositeNode *topNode)
{

  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a quite imposing pine tree");

  _tree->Branch("cluster_eta",&_b_cluster_eta, "cluster_eta/D");
  _tree->Branch("et_iso_calotower_sub_R01",&_b_et_iso_calotower_sub_R01, "et_iso_calotower_sub_R01/D");
  _tree->Branch("et_iso_calotower_R01",&_b_et_iso_calotower_R01, "et_iso_calotower_R01/D");
  _tree->Branch("et_iso_calotower_sub_R02",&_b_et_iso_calotower_sub_R02, "et_iso_calotower_sub_R02/D");
  _tree->Branch("et_iso_calotower_R02",&_b_et_iso_calotower_R02, "et_iso_calotower_R02/D");
  _tree->Branch("et_iso_calotower_sub_R03",&_b_et_iso_calotower_sub_R03, "et_iso_calotower_sub_R03/D");
  _tree->Branch("et_iso_calotower_R03",&_b_et_iso_calotower_R03, "et_iso_calotower_R03/D");
  _tree->Branch("et_iso_calotower_sub_R04",&_b_et_iso_calotower_sub_R04, "et_iso_calotower_sub_R04/D");
  _tree->Branch("et_iso_calotower_R04",&_b_et_iso_calotower_R04, "et_iso_calotower_R04/D");

 return 0;
}

int TreeMaker::process_event(PHCompositeNode *topNode)
{  
  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  std::cout << "ClusterIso::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << '\n';
  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  std::cout << "ClusterIso::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << '\n';
  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  std::cout << "ClusterIso::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << '\n';

  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
    
  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;

  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
  vx=vy=vz=0;
  if (vertexmap&&!vertexmap->empty())
  {
     GlobalVertex* vertex = (vertexmap->begin()->second);
     vx = vertex->get_x();
     vy = vertex->get_y();
     vz = vertex->get_z();
     std::cout<<"Event Vertex Calculated in ClusterIso x:"<<vx<<" y:"<<vy<<" z:"<<vz<<'\n';
  }

  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) 
  {
    RawCluster *cluster = rtiter->second;
    CLHEP::Hep3Vector vertex( vx, vy, vz); //set new correct vertex for eta calculation
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    //double cluster_energy = E_vec_cluster.mag();
    double cluster_eta = E_vec_cluster.pseudoRapidity(); 
    //double cluster_phi = E_vec_cluster.phi();
    //double et = cluster_energy / cosh( cluster_eta );
    _b_cluster_eta = cluster_eta;

    if(cluster->get_energy() / cosh( cluster_eta ) > 1) 
    {
      //arguments are (cone radiusx10, subtract event = true, use calotowers for isolation = true)
      _b_et_iso_calotower_sub_R01 = cluster->get_et_iso(1,1,1);
      //std::cout<<"_b_et_iso_calotower_sub_R01: "<<_b_et_iso_calotower_sub_R01<<std::endl;
      _b_et_iso_calotower_R01 = cluster->get_et_iso(1,0,1);
      //std::cout<<"_b_et_iso_calotower_R01: "<<_b_et_iso_calotower_R01<<std::endl;
      _b_et_iso_calotower_sub_R02 = cluster->get_et_iso(2,1,1);
      _b_et_iso_calotower_R02 = cluster->get_et_iso(2,0,1);
      _b_et_iso_calotower_sub_R03 = cluster->get_et_iso(3,1,1);
      _b_et_iso_calotower_R03 = cluster->get_et_iso(3,0,1);
      _b_et_iso_calotower_sub_R04 = cluster->get_et_iso(4,1,1);
      _b_et_iso_calotower_R04 = cluster->get_et_iso(4,0,1);
      _tree->Fill();
    }
  }
  return 0; 
}

int TreeMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}




