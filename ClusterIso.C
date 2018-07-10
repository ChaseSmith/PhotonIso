
/*!
 * \file ClusterIso.cc
 * \brief 
 * \author Francesco Vassalli <@colorado.edu> 
 * \author Chase Smith <chsm5267@colorado.edu>
 * \version $Revision:   $
 * \date $Date: $
 */



#include "ClusterIso.h"
#include <iostream>

//pulling these out for testing purposes
#include <fun4all/Fun4AllServer.h>  //can't find
#include <phool/getClass.h>         //good
#include <phool/PHCompositeNode.h>  //good

#include <calotrigger/CaloTriggerInfo.h> //can't find
#include <calobase/RawClusterContainer.h> //good
#include <calobase/RawCluster.h>          //good
#include <calobase/RawClusterUtility.h>   //good

#include <calobase/RawTowerContainer.h> //good 
#include <calobase/RawTowerGeomContainer_Cylinderv1.h> //can't find
#include <calobase/RawTowerGeomContainer.h>  //good

#include <g4main/PHG4Particle.h> //can't find
#include <g4main/PHG4VtxPoint.h> //can't find
#include <g4vertex/GlobalVertex.h>    //good
#include <g4vertex/GlobalVertexMap.h> //good



ClusterIso::ClusterIso(const std::string &name, float eTCut, float coneSize) : SubsysReco("ClusterIso"), eTCut(eTCut), coneSize(coneSize){
  std::cout<<"Begining Cluster Isolation Energy Calculation"<<'\n';
}

int ClusterIso::Init(PHCompositeNode *topNode)
{
  return 0;
}

void ClusterIso::seteTCut(float eTCut){
  this->eTCut = eTCut;
}

void ClusterIso::setConeSize(float coneSize){
  this->coneSize=coneSize;
}

float ClusterIso::geteTCut(){
  return eTCut;
}

float ClusterIso::getConeSize(){
  return coneSize;
}

int ClusterIso::process_event(PHCompositeNode *topNode)
{

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  std::cout << "ClusterIso::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << '\n';

  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  std::cout << "ClusterIso::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << '\n';

  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  std::cout << "ClusterIso::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;

  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  {
    RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
    
    RawClusterContainer::ConstRange begin_end = clusters->getClusters();
    RawClusterContainer::ConstIterator rtiter;
    
    std::cout << " ClusterIso sees " << clusters->size() << " clusters " << '\n';
    
    //declare new vertex to get correct cluster and tower eta
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
  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {

     
      RawCluster *cluster = rtiter->second;
      
      CLHEP::Hep3Vector vertex( vx, vy, vz); //set new correct vertex for eta calculation
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      double cluster_energy = E_vec_cluster.mag();
      double cluster_eta = E_vec_cluster.pseudoRapidity(); 
      double cluster_phi = E_vec_cluster.phi();
      double et = cluster_energy / cosh( cluster_eta );
      std::cout<<"Et:"<<et<<'\n';
      double isoEt=0;
      if (et < eTCut){

        continue; 
      } 
      //for each cluster go through all of the towers that are not in that cluster 
      //if the towers are within the iso cone add their energy to the sum 
      {
        RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
         RawTower *tower = rtiter->second; 
         RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
         double this_phi = tower_geom->get_phi();
         double this_eta= getTowerEta(tower_geom,vx,vy,vz);//needs be be recalculated getTowerEta(*tower_geom,vx,vy,vz); //get tower eta using new vertex

          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      {
        RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second; 
          RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
         double this_phi = tower_geom->get_phi();
         double this_eta= getTowerEta(tower_geom,vx,vy,vz);//needs be be recalculated getTowerEta(*tower_geom,vx,vy,vz); //get tower eta using new vertex
          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      {
        RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
        for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
          RawTower *tower = rtiter->second; 
          RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
         double this_phi = tower_geom->get_phi();
         double this_eta= getTowerEta(tower_geom,vx,vy,vz);//needs be be recalculated getTowerEta(*tower_geom,vx,vy,vz); //get tower eta using new vertex
          if ( deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < coneSize){//if this tower is within .3 (ort the conse size) of the truth photon add its ET to the isolated calorimeter
              isoEt += tower->get_energy() / cosh( this_eta );
          }
        }
      }
      isoEt-=et; // instead of checking which towers are in the cluster sum all tower et then subtract the cluster 
      std::cout<<"Set:"<<isoEt<<'\n';
      cluster->set_et_iso(isoEt);
    }
  }

  return 0;
}



int ClusterIso::End(PHCompositeNode *topNode)
{
  return 0;
}
