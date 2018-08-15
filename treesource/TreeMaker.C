#include "TreeMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
#include <iostream>
#include <vector>

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <jetbackground/TowerBackground.h>


////////////////////////////////////////////////////////////////////////////////////
//
//
//  These functions are for finding Sasha Bazilevsky's chi values for myself
//
//
///////////////////////////////////////////////////////////////////////////////////


ChaseTower findMaxTower(std::vector<ChaseTower> towers)
{
  ChaseTower MaxTower;
  MaxTower.setKey(towers.at(0).getKey());
  MaxTower.setEnergy(towers.at(0).getEnergy());
  MaxTower.setPhi(towers.at(0).getPhi());
  MaxTower.setEta(towers.at(0).getEta());

  for(unsigned int i = 1; i < towers.size(); i++)
  {
    if(towers.at(i).getEnergy() > MaxTower.getEnergy())
    {
      MaxTower.setKey(towers.at(i).getKey());
      MaxTower.setEnergy(towers.at(i).getEnergy());
      MaxTower.setPhi(towers.at(i).getPhi());
      MaxTower.setEta(towers.at(i).getEta());
    }
  }
  return MaxTower;
}

//double* CenterOfEnergy(double *eta, double *phi, double *energy, int NTowers, double etot)
//{
//  double *CoE = new double[2];
//  double avgeta = 0;
//  double avgphi = 0;
//  for(int i = 0; i < NTowers; i++)
//  {
//    avgeta += eta[i] * energy[i];
//    avgphi += phi[i] * energy[i];
//  }
//  CoE[0] = avgeta/etot;
//  CoE[1] = avgphi/etot;
//  return CoE;
//}

double* CenterOfEnergy_BazilevskyStyle(std::vector<ChaseTower> towers, double etot)
{
  double *CoE = new double[2];
  double avgeta = 0;
  double avgphi = 0;
  for(unsigned int i = 0; i < towers.size(); i++)
  {
    avgeta += towers.at(i).getEta() * towers.at(i).getEnergy();
    avgphi += towers.at(i).getPhi() * towers.at(i).getEnergy();
  }
  CoE[0] = avgeta/etot;
  CoE[1] = avgphi/etot;
  return CoE;
}

//void ChiValues_BazilevskyStyle(std::vector<ChaseTower> towers, double etot, double *eta4, double *phi4, double *energy4)
//{
//
//}


////////////////////////////////////////////////////////////////////////////////////
//
//
//  This is the actual meat of the afterburner module
//
//
///////////////////////////////////////////////////////////////////////////////////



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
  //_tree->Branch("et_iso_calotower_sub_R01",_b_et_iso_calotower_sub_R01, "et_iso_calotower_sub_R01[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R01",_b_et_iso_calotower_R01, "et_iso_calotower_R01[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R02",_b_et_iso_calotower_sub_R02, "et_iso_calotower_sub_R02[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R02",_b_et_iso_calotower_R02, "et_iso_calotower_R02[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R03",_b_et_iso_calotower_sub_R03, "et_iso_calotower_sub_R03[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R03",_b_et_iso_calotower_R03, "et_iso_calotower_R03[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_sub_R04",_b_et_iso_calotower_sub_R04, "et_iso_calotower_sub_R04[cluster_n]/D");
  //_tree->Branch("et_iso_calotower_R04",_b_et_iso_calotower_R04, "et_iso_calotower_R04[cluster_n]/D");

  _tree->Branch("NTowers",_b_NTowers,"NTowers[cluster_n]/D");
  _tree->Branch("etot",_b_etot,"etot[cluster_n]/D");

  _tree->Branch("tower_n",&_b_tower_n);
  _tree->Branch("tower_eta",_b_tower_eta,"tower_eta[cluster_n]/D");
  _tree->Branch("tower_phi",_b_tower_phi,"tower_phi[cluster_n]/D");

 return 0;
}



int TreeMaker::process_event(PHCompositeNode *topNode)
{
  /////////////////////////////////////find truth particle information /////////////////////////////////////////////
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

  //RawTowerContainer *Emtow = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  //std::cout<<"Size of RawTowerContainer for EMCal: "<<Emtow->size()<<std::endl;
  
  
  //////////////////////////////////////Find cluster information/////////////////////////////////////////////////////
  _b_cluster_n = 0;
  _b_tower_n = 0;

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");

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

    std::cout<<"Cluster Eta: "<<cluster_eta<<std::endl;
    std::cout<<"Cluster Phi: "<<cluster_phi<<std::endl;

    //arguments are (cone radiusx10, subtract event = true, use calotowers for isolation = true)
    //_b_et_iso_calotower_sub_R01[ _b_cluster_n ] = cluster->get_et_iso(1,1,1);
    //_b_et_iso_calotower_R01[ _b_cluster_n ] = cluster->get_et_iso(1,0,1);
    //_b_et_iso_calotower_sub_R02[ _b_cluster_n ] = cluster->get_et_iso(2,1,1);
    //_b_et_iso_calotower_R02[ _b_cluster_n ] = cluster->get_et_iso(2,0,1);
    //_b_et_iso_calotower_sub_R03[ _b_cluster_n ] = cluster->get_et_iso(3,1,1);
    //_b_et_iso_calotower_R03[ _b_cluster_n ] = cluster->get_et_iso(3,0,1);
    //_b_et_iso_calotower_sub_R04[ _b_cluster_n ] = cluster->get_et_iso(4,1,1);
    //_b_et_iso_calotower_R04[ _b_cluster_n ] = cluster->get_et_iso(4,0,1);

    //now we get tower information for ID purposes, find "Center of Energy", get 4 central towers
    _b_NTowers[_b_cluster_n] = cluster->getNTowers();
    std::cout<<"Number of Towers in Cluster: "<<_b_NTowers[_b_cluster_n]<<std::endl;

    std::vector <ChaseTower> clusterTowers;

    RawCluster::TowerConstRange clusterrange = cluster->get_towers();
    for (RawCluster::TowerConstIterator rtiter = clusterrange.first; rtiter != clusterrange.second; ++rtiter) 
    {
      RawTower *tower = towersEM3old->getTower(rtiter->first);
      RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
      ChaseTower temp;
      temp.setEta(tower_geom->get_eta());
      temp.setPhi(tower_geom->get_phi());
      temp.setPhi(tower->get_energy());
      temp.setKey(tower->get_key());
      clusterTowers.push_back(temp);
    }

    ////////////////////now that we have all towers from cluster, find max tower//////////////////////////
    ChaseTower MaxTower = findMaxTower(clusterTowers);
    std::cout<<"Max Tower Eta: "<<MaxTower.getEta()<<std::endl;
    std::cout<<"Max Tower Phi: "<<MaxTower.getPhi()<<std::endl;

    ////////////////////Find 49 towers around max tower, Sasha style/////////////////////////////////////
    std::vector<ChaseTower> Sasha49Towers;
    _b_etot[_b_cluster_n] = 0;

    RawTowerContainer::ConstRange towerrange = towersEM3old->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = towerrange.first; rtiter != towerrange.second; ++rtiter) 
    {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
      double this_phi = tower_geom->get_phi();
      double this_eta = tower_geom->get_eta();
      double this_energy = tower->get_energy();
      double dif_eta = this_eta - MaxTower.getEta();
      double dif_phi = this_phi - MaxTower.getPhi();
      _b_tower_phi[_b_tower_n] = tower_geom->get_phi();
      _b_tower_eta[_b_tower_n] = tower_geom->get_eta();
      std::cout<<"tower number"<<_b_tower_n<<std::endl;
      _b_tower_n++;

      //if(dif_phi > TMath::Pi()){dif_phi -= 2*TMath::Pi();} //make sure dif_phi is between -pi and pi
      //else if(dif_phi < -1*TMath::Pi()){dif_phi += 2*TMath::Pi();}

      if(fabs(dif_eta) < 0.08 and fabs(dif_phi) < 0.08)
      {
        std::cout<<"ANOTHER TOWER PASSED THE CUT "<<std::endl;
        std::cout<<"tower eta: "<<this_eta<<std::endl;
        std::cout<<"tower phi: "<<this_phi<<std::endl;
        ChaseTower temp;
        temp.setEta(this_phi);
        temp.setPhi(this_eta);
        temp.setPhi(this_energy);
        temp.setKey(tower->get_key());
        Sasha49Towers.push_back(temp);
        _b_etot[_b_cluster_n] += tower->get_energy();
      }
    }
    std::cout<<"size of the 49 tower vector (better be 49): "<<Sasha49Towers.size()<<std::endl;
    /////////////Find Center of energy for cluster, get tower info of 4 towers around CoE////////////////
    double CoE[2] = CenterOfEnergy_BazilevskyStyle(Sasha49Towers, _b_etot[_b_cluster_n] );
    std::cout<<"Center of Energy eta: "<<CoE[0]<<std::endl;
    std::cout<<"Center of Energy phi: "<<CoE[1]<<std::endl;


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

