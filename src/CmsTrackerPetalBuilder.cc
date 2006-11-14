#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerPetalBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerRingBuilder.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>


void CmsTrackerPetalBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){

  GeometricDet * det  = 
    new GeometricDet(&fv,
		     theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv)));
  CmsTrackerRingBuilder theCmsTrackerRingBuilder;
  theCmsTrackerRingBuilder.build(fv,det,s);  
  g->addComponent(det);
}

void CmsTrackerPetalBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  std::vector< GeometricDet* > comp = det->components();

  switch(det->components().front()->type()){
  case GeometricDet::ring: std::stable_sort(comp.begin(),comp.end(),LessR_module()); break;	
  default:
    edm::LogError("CmsTrackerPetalBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type(); 
  }
  
  //Maximum Numebr fo TEC Ring is 7 in order 
  //to discover from which number we have to strat
  //the operation is MaxRing - RealRingNumber + 1 (C++)

  uint32_t startring = 8 - comp.size();

  for(uint32_t i=0; i<comp.size(); i++){
    comp[i]->setGeographicalID(DetId(startring+i));
  }
  det->deleteComponents();
  det->addComponents(comp);
}



