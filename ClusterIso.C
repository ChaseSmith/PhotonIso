#include "ClusterIso.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include <iostream>

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>

#include <calobase/RawTowerGeom.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawTowerGeomContainer.h>

#include <g4main/PHG4Particle.h>


ClusterIso::ClusterIso(const std::string &name) : SubsysReco("TRIGGERTEST")
{

  _foutname = name;

}

int ClusterIso::Init(PHCompositeNode *topNode)
{

  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a succulent orange tree");

  _tree->Branch("cluster_iso", &_b_cluster_isoEt,"cluster_iso/F");

  return 0;
}

bool towerInCluster(RawCluster* cluster, RawTower* tower){
  return cluster->get_towermap().cend() == cluster->get_towermap().find(tower->get_key());
}

int ClusterIso::process_event(PHCompositeNode *topNode, float pTCut, float coneSize)
{

  std::cout << "DVP : at process_event, tree size is: " << _tree->GetEntries() << std::endl;

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  //RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
  //RawTowerContainer *towersEM4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
  std::cout << "ClusterIso::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << std::endl;
  //std::cout << "ClusterIso::process_event: " << towersEM3->size() << " TOWER_CALIB_CEMC_RETOWER towers" << std::endl;
  //std::cout << "ClusterIso::process_event: " << towersEM4->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << std::endl;

  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  //RawTowerContainer *towersIH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
  std::cout << "ClusterIso::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << std::endl;
  //std::cout << "ClusterIso::process_event: " << towersIH4->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << std::endl;

  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  //RawTowerContainer *towersOH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
  std::cout << "ClusterIso::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;
  //std::cout << "ClusterIso::process_event: " << towersOH4->size() << " TOWER_CALIB_HCALOUT_SUB1 towers" << std::endl;

  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  {
    RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
    
    RawClusterContainer::ConstRange begin_end = clusters->getClusters();
    RawClusterContainer::ConstIterator rtiter;
    
    std::cout << " ClusterIso sees " << clusters->size() << " clusters " << '\n';
    _b_cluster_n=0;

    for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
      RawCluster *cluster = rtiter->second;
      
      CLHEP::Hep3Vector vertex( 0, 0, 0 ); //Note these need to be changed to get the right vertex
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      float cluster_energy = E_vec_cluster.mag();
      float cluster_eta = E_vec_cluster.pseudoRapidity(); //may need to chagne the eta after it is set.  Needs to be in same reference frame as the towers 
      float cluster_phi = E_vec_cluster.phi();
      float pt = cluster_energy / cosh( cluster_eta );
      float isoEt=0;
      if (pt < pTCut) continue; 
      //for each cluster go through all of the towers that are not in that cluster 
      //if the towers are within the iso cone add their energy to the sum 
      {
        RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second; 
          RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
          if(clusterInTower(cluster,tower)) continue;
          float this_phi = tower_geom->get_phi();
          float this_eta = tower_geom->get_eta();//check if global vertex is calculated 
          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      {
        RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second; 
          RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
          float this_phi = tower_geom->get_phi();
          float this_eta = tower_geom->get_eta();//check if global vertex is calculated 
          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      {
        RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second; 
          RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
          float this_phi = tower_geom->get_phi();
          float this_eta = tower_geom->get_eta();//check if global vertex is calculated 
          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      
      std::cout << " cluster (CEMC) # " << _b_cluster_n << '\n';
      
      _b_cluster_n++;
      _b_cluster_isoEt=isoEt
      _tree->Fill(); //may need to turn this into an array and fill on the outside 
    }
  }

  return 0;
}



int ClusterIso::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

