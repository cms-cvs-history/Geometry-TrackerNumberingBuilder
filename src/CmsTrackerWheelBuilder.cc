#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerWheelBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerRingBuilder.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerPetalBuilder.h"
#include "Geometry/TrackerNumberingBuilder/interface/TrackerStablePhiSort.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include<vector>

#include <bitset>

void CmsTrackerWheelBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){
  CmsTrackerRingBuilder theCmsTrackerRingBuilder ;
  CmsTrackerPetalBuilder theCmsTrackerPetalBuilder ;

  GeometricDet * subdet = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv)));
  switch (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv))){
  case GeometricDet::ring:
    theCmsTrackerRingBuilder.build(fv,subdet,s);      
    break;
  case GeometricDet::petal:
    theCmsTrackerPetalBuilder.build(fv,subdet,s);      
    break;
  default:
    edm::LogError("CmsTrackerWheelBuilder")<<" ERROR - I was expecting a Ring or Petal, I got a "<<ExtractStringFromDDD::getString(s,&fv);
  }  
  g->addComponent(subdet);
}

void CmsTrackerWheelBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  std::vector< GeometricDet*> comp = det->components();
       
  if(comp.size()){
    if(comp.front()->type()==GeometricDet::petal){
      std::vector< GeometricDet* > compfw;
      std::vector< GeometricDet* > compbw;
      compfw.clear();
      compbw.clear();
      for(uint32_t i=0; i<comp.size();i++){
	if(fabs(comp[i]->translation().z())<fabs(det->translation().z())){
	  compfw.push_back(comp[i]);
	}else{
	  compbw.push_back(comp[i]);      
	}
      }    
      
      // TEC+
      if( comp.front()->translation().z() > 0 ) {
	TrackerStablePhiSort(compfw.begin(), compfw.end(), ExtractPhiModule());
	TrackerStablePhiSort(compbw.begin(), compbw.end(), ExtractPhiModule());
      }
      // TEC-
      else {
	TrackerStablePhiSort(compfw.begin(), compfw.end(), ExtractPhiModuleMirror());
	TrackerStablePhiSort(compbw.begin(), compbw.end(), ExtractPhiModuleMirror());	
      }
      //
      // TEC
      // Wheel Part:   3 bits [back:1 front:2]
      // Petal Number: 4 bits [1,...,8]
      //
      for(uint32_t i=0; i<compbw.size(); i++){
	uint32_t temp = i+1;
	temp|=(1<<4);
	compbw[i]->setGeographicalID(DetId(temp));
      }
      for(uint32_t i=0; i<compfw.size(); i++){
	uint32_t temp = i+1;
	temp|=(2<<4);
	compfw[i]->setGeographicalID(DetId(temp));
      }
      
      det->deleteComponents();
      det->addComponents(compfw);
      det->addComponents(compbw);
      
    }else{
      std::stable_sort(comp.begin(),comp.end(),LessR_module());

      // TID
      // Disk Number: 2 bits [1,2,3]
      for(uint32_t i=0; i<comp.size(); i++){
	comp[i]->setGeographicalID(DetId(i+1));
      }
      
      det->deleteComponents();
      det->addComponents(comp);
    }
  }else{
    edm::LogError("CmsTrackerWheelBuilder")<<"Where are the Petals or Rings?";
  }
  
}




