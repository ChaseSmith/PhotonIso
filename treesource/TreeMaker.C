#include "TreeMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
#include <iostream>

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <jetbackground/TowerBackground.h>


double* CenterOfEnergy(double *eta, double *phi, double *energy, int NTowers, double etot)
{
  double *CoE = new double[2];
  double avgeta = 0;
  double avgphi = 0;
  for(int i = 0; i < NTowers; i++)
  {
    avgeta += eta[i] * energy[i];
    avgphi += phi[i] * energy[i];
  }
  CoE[0] = avgeta/etot;
  CoE[1] = avgphi/etot;
  return CoE;
}

double* CenterOfEnergy_BazilevskyStyle(double *eta, double *phi, double *energy, int NTowers, double etot)
{
  double *CoE = new double[2];
  double avgeta = 0;
  double avgphi = 0;
  for(int i = 0; i < NTowers; i++)
  {
    avgeta += eta[i] * energy[i];
    avgphi += phi[i] * energy[i];
  }
  CoE[0] = avgeta/etot;
  CoE[1] = avgphi/etot;
  return CoE;
}



TreeMaker::TreeMaker(const std::string &name, int embed_id) : SubsysReco("IsoTree")
{

  _foutname = name;
  _embed_id = embed_id;

}

int TreeMaker::Init(PHCompositeNode *topNode)
{

  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a gentle baobab tree");
  //truth particle information
  _tree->Branch("particle_n", &_b_particle_n);
  _tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  _tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  _tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  _tree->Branch("particle_pid", _b_particle_pid,"particle_pid[particle_n]/I");
  _tree->Branch("particle_et", _b_particle_et,"particle_et[particle_n]/F");

  //reco cluster information
  _tree->Branch("cluster_n", &_b_cluster_n);
  _tree->Branch("cluster_et", _b_cluster_et,"cluster_et[cluster_n]/D");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/D");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/D");
  _tree->Branch("cluster_prob",_b_cluster_prob,"cluster_prob[cluster_n]/D");
  _tree->Branch("et_iso_calotower_sub_R01",_b_et_iso_calotower_sub_R01, "et_iso_calotower_sub_R01[cluster_n]/D");
  _tree->Branch("et_iso_calotower_R01",_b_et_iso_calotower_R01, "et_iso_calotower_R01[cluster_n]/D");
  _tree->Branch("et_iso_calotower_sub_R02",_b_et_iso_calotower_sub_R02, "et_iso_calotower_sub_R02[cluster_n]/D");
  _tree->Branch("et_iso_calotower_R02",_b_et_iso_calotower_R02, "et_iso_calotower_R02[cluster_n]/D");
  _tree->Branch("et_iso_calotower_sub_R03",_b_et_iso_calotower_sub_R03, "et_iso_calotower_sub_R03[cluster_n]/D");
  _tree->Branch("et_iso_calotower_R03",_b_et_iso_calotower_R03, "et_iso_calotower_R03[cluster_n]/D");
  _tree->Branch("et_iso_calotower_sub_R04",_b_et_iso_calotower_sub_R04, "et_iso_calotower_sub_R04[cluster_n]/D");
  _tree->Branch("et_iso_calotower_R04",_b_et_iso_calotower_R04, "et_iso_calotower_R04[cluster_n]/D");

 return 0;
}

int TreeMaker::process_event(PHCompositeNode *topNode)
{  
    //find truth particle information 
  _b_particle_n = 0;
  
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
 
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  vx = NAN;
  vy = NAN;
  vz = NAN;
  if (vertexmap)
  {
    if (!vertexmap->empty())
    {
      GlobalVertex* vertex = (vertexmap->begin()->second);

      vx = vertex->get_x();
      vy = vertex->get_y();
      vz = vertex->get_z();
    }
  }
 
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
    if ( truthinfo->isEmbeded( g4particle->get_track_id() ) != _embed_id ) continue;
    TLorentzVector t; t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );
    
    float truth_pt = t.Pt();
    float truth_et = t.Et();
    //if (truth_pt < 1) continue;
    float truth_eta = t.Eta();
    if (fabs(truth_eta) > 1.1) continue;
    float truth_phi = t.Phi();
    int truth_pid = g4particle->get_pid();
    
    _b_particle_pt[ _b_particle_n ] = truth_pt;
    _b_particle_eta[ _b_particle_n ] = truth_eta;
    _b_particle_phi[ _b_particle_n ] = truth_phi;
    _b_particle_pid[ _b_particle_n ] = truth_pid;
    _b_particle_et[ _b_particle_n ] = truth_et;
    
    _b_particle_n++;
    
  }
  

  //Find cluster information
  _b_cluster_n = 0;

  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
    
  //find correct vertex
  vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
  vx=vy=vz=0;
  if (vertexmap&&!vertexmap->empty())
  {
     GlobalVertex* vertex = (vertexmap->begin()->second);
     vx = vertex->get_x();
     vy = vertex->get_y();
     vz = vertex->get_z();
     std::cout<<"Event Vertex Calculated in ClusterIso x:"<<vx<<" y:"<<vy<<" z:"<<vz<<'\n';
  }

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;

  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) 
  {
    RawCluster *cluster = rtiter->second;
    CLHEP::Hep3Vector vertex( vx, vy, vz); //set new correct vertex for eta calculation
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
    double cluster_energy = E_vec_cluster.mag();
    double cluster_eta = E_vec_cluster.pseudoRapidity(); 
    double cluster_phi = E_vec_cluster.phi();
    double et = cluster_energy / cosh( cluster_eta );
    double prob = cluster->get_prob();

    if (et < 1) continue;
    _b_cluster_eta[ _b_cluster_n ] = cluster_eta;
    _b_cluster_phi[ _b_cluster_n ] = cluster_phi;
    _b_cluster_et[ _b_cluster_n ] = et;
    _b_cluster_prob[ _b_cluster_n ] = prob;
    //arguments are (cone radiusx10, subtract event = true, use calotowers for isolation = true)
    _b_et_iso_calotower_sub_R01[ _b_cluster_n ] = cluster->get_et_iso(1,1,1);
    _b_et_iso_calotower_R01[ _b_cluster_n ] = cluster->get_et_iso(1,0,1);
    _b_et_iso_calotower_sub_R02[ _b_cluster_n ] = cluster->get_et_iso(2,1,1);
    _b_et_iso_calotower_R02[ _b_cluster_n ] = cluster->get_et_iso(2,0,1);
    _b_et_iso_calotower_sub_R03[ _b_cluster_n ] = cluster->get_et_iso(3,1,1);
    _b_et_iso_calotower_R03[ _b_cluster_n ] = cluster->get_et_iso(3,0,1);
    _b_et_iso_calotower_sub_R04[ _b_cluster_n ] = cluster->get_et_iso(4,1,1);
    _b_et_iso_calotower_R04[ _b_cluster_n ] = cluster->get_et_iso(4,0,1);

    //now we get tower information for ID purposes, find "Center of Energy", get 4 central towers
    _b_NTowers = cluster->getNTowers();
    std::cout<<"Number of Towers in Cluster: "<<_b_NTowers<<std::endl;
    double tower_phi[_b_NTowers];
    double tower_eta[_b_NTowers];
    double tower_energy[_b_NTowers];
    _b_etot[_b_cluster_n] = 0;
    int counter = 0;
    /*
    {
      RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
      for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) 
      {
        RawTower *tower = rtiter->second;
        RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
        double this_phi = tower_geom->get_phi();
        double this_eta= tower_geom->get_eta();
        if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize)
        {
          isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
        }
      }
    }
    */

    RawTowerContainer::TowerConstRange begign_end = cluster->get_towers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) 
    {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
      std::cout<<"Tower Key: "<<tower->get_key()<<std::endl;

      tower_phi[counter] = tower_geom->get_phi();
      std::cout<<"Tower Phi: "<<tower_geom->get_phi()<<std::endl;

      tower_eta[counter] = tower_geom->get_eta();
      std::cout<<"Tower Eta: "<<tower_geom->get_eta()<<std::endl;

      tower_energy[counter] = tower->get_energy();
      std::cout<<"Tower Energy: "<<tower_geom->get_energy()<<std::endl;

      _b_etot[_b_cluster_n] += tower->get_energy();
      counter++;
    }
    std::cout<<"Total Energy in Cluster: "<<_b_etot<<std::endl;

    _b_cluster_n++;
  }
    
  _tree->Fill();


  return 0; 
}

int TreeMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

