#include "PhotonIsolationMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
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

//#include <g4jets/JetMap.h>
//#include <g4jets/Jet.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <jetbackground/TowerBackground.h>

PhotonIsolationMaker::PhotonIsolationMaker(const std::string &name) : SubsysReco("TRIGGERTEST")
{

  _foutname = name;

}

int PhotonIsolationMaker::Init(PHCompositeNode *topNode)
{

  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a succulent orange tree");

  _tree->Branch("particle_n", &_b_particle_n,"particle_n/I");
  _tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  _tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  _tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  _tree->Branch("particle_pid", _b_particle_pid,"particle_pid[particle_n]/I");
  _tree->Branch("particle_calo_iso_0", _b_particle_calo_iso_0,"particle_calo_iso_0[particle_n]/F");
  _tree->Branch("particle_calo_iso_1", _b_particle_calo_iso_1,"particle_calo_iso_1[particle_n]/F");
  _tree->Branch("particle_calo_iso_2", _b_particle_calo_iso_2,"particle_calo_iso_2[particle_n]/F");

  _tree->Branch("cluster_n", &_b_cluster_n,"cluster_n/I");
  _tree->Branch("cluster_pt", _b_cluster_pt,"cluster_pt[cluster_n]/F");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/F");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/F");


  return 0;

}

int PhotonIsolationMaker::process_event(PHCompositeNode *topNode)
{

  std::cout << "DVP : at process_event, tree size is: " << _tree->GetEntries() << std::endl;

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  //RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
  //RawTowerContainer *towersEM4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
  std::cout << "PhotonIsolationMaker::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << std::endl;
  //std::cout << "PhotonIsolationMaker::process_event: " << towersEM3->size() << " TOWER_CALIB_CEMC_RETOWER towers" << std::endl;
  //std::cout << "PhotonIsolationMaker::process_event: " << towersEM4->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << std::endl;

  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  //RawTowerContainer *towersIH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
  std::cout << "PhotonIsolationMaker::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << std::endl;
  //std::cout << "PhotonIsolationMaker::process_event: " << towersIH4->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << std::endl;

  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  //RawTowerContainer *towersOH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
  std::cout << "PhotonIsolationMaker::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;
  //std::cout << "PhotonIsolationMaker::process_event: " << towersOH4->size() << " TOWER_CALIB_HCALOUT_SUB1 towers" << std::endl;

  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  _b_particle_n = 0;
  
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?

    //if ( truthinfo->isEmbeded( g4particle->get_track_id() ) != _embed_id ) continue;
    
    TLorentzVector t; t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
    
    float truth_pt = t.Pt();
    //if (truth_pt < 1) continue;
    float truth_eta = t.Eta();
    if (fabs(truth_eta) > 1.1) continue;
    float truth_phi = t.Phi();
    int truth_pid = g4particle->get_pid();
    
    //if (truth_pid == 22 || truth_pid == 2112 || truth_pid == -2112 || truth_pid == 130) continue;
    //if (truth_pid == 2112 || truth_pid == -2112 || truth_pid == 130) continue;
    // save high-pT photons
    //if (truth_pid == 22 && truth_pt < 20) continue;
    //if (truth_pid == 12 || truth_pid == -12 || truth_pid == 13 || truth_pid == -13 || truth_pid == 14 || truth_pid == -14) continue;
    
    _b_particle_pt[ _b_particle_n ] = truth_pt;
    _b_particle_eta[ _b_particle_n ] = truth_eta;
    _b_particle_phi[ _b_particle_n ] = truth_phi;
    _b_particle_pid[ _b_particle_n ] = truth_pid;
    
    _b_particle_calo_iso_0[ _b_particle_n ] = -99;
    _b_particle_calo_iso_1[ _b_particle_n ] = -99;
    _b_particle_calo_iso_2[ _b_particle_n ] = -99;
    if ( truth_pid == 22 && truth_pt > 10 && fabs( truth_eta ) > 0.7 ){return 0;}
    else if ( truth_pid == 22 && truth_pt > 10  ) {
      _b_particle_calo_iso_0[ _b_particle_n ] = 0;
      _b_particle_calo_iso_1[ _b_particle_n ] = 0;
      _b_particle_calo_iso_2[ _b_particle_n ] = 0;

      {
	RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
	for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
	  RawTower *tower = rtiter->second;
	  RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());

	  float this_phi = tower_geom->get_phi();
	  float this_eta = tower_geom->get_eta();
	  float this_ET = tower->get_energy() / cosh( this_eta );

	  if ( deltaR( truth_eta, this_eta, truth_phi, this_phi ) < 0.3 )
	    _b_particle_calo_iso_0[ _b_particle_n ] += this_ET;
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

	  if ( deltaR( truth_eta, this_eta, truth_phi, this_phi ) < 0.3 )
	    _b_particle_calo_iso_1[ _b_particle_n ] += this_ET;

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

	  if ( deltaR( truth_eta, this_eta, truth_phi, this_phi ) < 0.3 )
	    _b_particle_calo_iso_2[ _b_particle_n ] += this_ET;

	}
      }

      // 

      std::cout << " --> truth photon at #" << _b_particle_n << ", pt / eta / phi = " << truth_pt << " / " << truth_eta << " / " << truth_phi << ", PID " << truth_pid << ", embed = " <<  truthinfo->isEmbeded( g4particle->get_track_id() ) << std::endl;
      std::cout << " --> --> calo iso in layers = " << _b_particle_calo_iso_0[ _b_particle_n ] << " / " << _b_particle_calo_iso_1[ _b_particle_n ] << " / " << _b_particle_calo_iso_2[ _b_particle_n ] << std::endl;

    }
    
    _b_particle_n++;
    
  }

  _b_cluster_n = 0;

  {
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  
  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;
  
  //std::cout << " I see " << clusters->size() << " clusters " << std::endl;
  
  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
    RawCluster *cluster = rtiter->second;
    
    CLHEP::Hep3Vector vertex( 0, 0, 0 );
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    float cluster_energy = E_vec_cluster.mag();
    float cluster_eta = E_vec_cluster.pseudoRapidity();

    float pt = cluster_energy / cosh( cluster_eta );
    
    if (pt < 5) continue;
    
    _b_cluster_pt[ _b_cluster_n ] = pt;
    _b_cluster_eta[ _b_cluster_n ] =  cluster_eta;
    _b_cluster_phi[ _b_cluster_n ] =  cluster->get_phi();
    
    std::cout << " cluster (CEMC) # " << _b_cluster_n << " pt/eta/phi = " << pt << " / " << cluster_eta << " / " << cluster->get_phi() << std::endl;
    
    _b_cluster_n++;
  }
  }

  _tree->Fill();

  return 0;
}



int PhotonIsolationMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

