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

  _tree->Branch("cluster_n", &_b_cluster_n,"cluster_n/I");
  _tree->Branch("cluster_pt", _b_cluster_pt,"cluster_pt[cluster_n]/F");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/F");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/F");
  _tree->Branch("cluster_prob", _b_cluster_prob,"cluster_prob[cluster_n]/F");
  _tree->Branch("cluster_ecore",_b_cluster_ecore,"cluster_ecore[cluster_n]/F");
  _tree->Branch("cluster_e",    _b_cluster_e,"cluster_e[cluster_n]/F");


  return 0;

}

int ClusterIso::process_event(PHCompositeNode *topNode, float pTCut)
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

  _b_particle_n = 0;
  
  _b_particle_calo_iso_0[ _b_particle_n ] = -99;
  _b_particle_calo_iso_1[ _b_particle_n ] = -99;
  _b_particle_calo_iso_2[ _b_particle_n ] = -99;
  //here there are 3 loops through the towers in the EM &I/O HCAl
   /*{
      RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
      for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
    	  RawTower *tower = rtiter->second; 
    	  RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());

      	float this_phi = tower_geom->get_phi();
      	float this_eta = tower_geom->get_eta();//check if global vertex is calculated 
      	float this_ET = tower->get_energy() / cosh( this_eta );
      }
    }
      {
      	RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
      	for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
      	  RawTower *tower = rtiter->second;
      	  RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());

      	float this_phi = tower_geom->get_phi();
      	float this_eta = tower_geom->get_eta();
      	float this_ET = tower->get_energy() / cosh( this_eta );
      	}
      }
      {
      	RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
      	for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
      	  RawTower *tower = rtiter->second;
      	  RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());

      	float this_phi = tower_geom->get_phi();
      	float this_eta = tower_geom->get_eta();
      	float this_ET = tower->get_energy() / cosh( this_eta );
      	}
      }
  
  _b_particle_n++;*/
  _b_cluster_n = 0;

  {
    RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
    
    RawClusterContainer::ConstRange begin_end = clusters->getClusters();
    RawClusterContainer::ConstIterator rtiter;
    
    std::cout << " I see " << clusters->size() << " clusters " << '\n';
    
    for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
      RawCluster *cluster = rtiter->second;
      
      CLHEP::Hep3Vector vertex( 0, 0, 0 ); //Note these need to be changed to get the right vertex
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      float cluster_energy = E_vec_cluster.mag();
      float cluster_eta = E_vec_cluster.pseudoRapidity();
      float cluster_phi = 
      float pt = cluster_energy / cosh( cluster_eta );
      
      if (pt < pTCut) continue; 
      {
      RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
      for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
        RawTower *tower = rtiter->second; 
        RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());

        float this_phi = tower_geom->get_phi();
        float this_eta = tower_geom->get_eta();//check if global vertex is calculated 
        float this_ET = tower->get_energy() / cosh( this_eta );
      }
    }
      {
        RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second;
          RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());

        float this_phi = tower_geom->get_phi();
        float this_eta = tower_geom->get_eta();
        float this_ET = tower->get_energy() / cosh( this_eta );
        }
      }
      {
        RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second;
          RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());

        float this_phi = tower_geom->get_phi();
        float this_eta = tower_geom->get_eta();
        float this_ET = tower->get_energy() / cosh( this_eta );
        }
      }
      
      _b_cluster_pt[ _b_cluster_n ] = pt;
      _b_cluster_eta[ _b_cluster_n ] =  cluster_eta;
      _b_cluster_phi[ _b_cluster_n ] =  cluster->get_phi();
      
      std::cout << " cluster (CEMC) # " << _b_cluster_n << " pt/eta/phi = " << pt << " / " << cluster_eta << " / " << cluster->get_phi() << '\n';
      
      _b_cluster_n++;
    }
  }

  _tree->Fill();

  return 0;
}



int ClusterIso::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

