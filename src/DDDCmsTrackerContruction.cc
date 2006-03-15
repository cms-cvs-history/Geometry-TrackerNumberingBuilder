#include "Geometry/TrackerNumberingBuilder/interface/DDDCmsTrackerContruction.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerBuilder.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerDetIdBuilder.h"

using namespace cms;

DDDCmsTrackerContruction::DDDCmsTrackerContruction(){

}

const GeometricDet* DDDCmsTrackerContruction::construct(const DDCompactView* cpv){
  attribute = "TkDDDStructure"; // could come from .orcarc
  std::string value     = "any";
  DDSpecificsFilter filter;
  DDValue ddv(attribute,value,0);
  filter.setCriteria(ddv,DDSpecificsFilter::not_equals);
  
  DDFilteredView fv(*cpv); 
  fv.addFilter(filter);
  fv.firstChild();
  
  if (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(attribute,&fv)) != GeometricDet::Tracker){
     throw cms::Exception("NoTrackerDDD") <<" The first child of the DDFilteredView is not what is expected \""<<ExtractStringFromDDD::getString(attribute,&fv)<<"\"";
    abort();
  }
  
  GeometricDet* tracker  = new GeometricDet(&fv,GeometricDet::Tracker);
  CmsTrackerBuilder theCmsTrackerBuilder;
  theCmsTrackerBuilder.build(fv,tracker,attribute);

  
  CmsTrackerDetIdBuilder theCmsTrackerDetIdBuilder;
  
  tracker = theCmsTrackerDetIdBuilder.buildId(tracker);
  fv.parent();
  //
  // set the Tracker
  //
  //TrackerMapDDDtoID::instance().setTracker(tracker);
  //NOTE: If it is decided that the TrackerMapDDDtoID should be
  // constructed here, then we should return from this
  // function so that the EventSetup can manage it

  return tracker;
}

