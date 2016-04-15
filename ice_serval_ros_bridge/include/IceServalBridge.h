/*
 * ice_serval_bridge.h
 *
 *  Created on: Apr 11, 2016
 *      Author: sni
 */

#ifndef ICE_SERVAL_BRIDGE_H_
#define ICE_SERVAL_BRIDGE_H_

#include <memory>
#include <ros/ros.h>

#include "IdentityDirectory.h"
#include "ice/ontology/OntologyInterface.h"


namespace ice
{
class CommunicationInterface;

struct InitParams
{
  std::string ontologyPath;
  std::string ontologyIri;

  std::string servalInstancePath;
  std::string servalHost;
  int         servalPort;
  std::string servalUser;
  std::string servalPassword;
};

class ice_serval_bridge
{
public:
  ice_serval_bridge(ros::NodeHandle nh_, ros::NodeHandle pnh_);
  ice_serval_bridge(ros::NodeHandle nh_, ros::NodeHandle pnh_, InitParams* params);
  virtual ~ice_serval_bridge();
  void init();

public:
  IdentityDirectory                                     identityDirectory;

private:
  ros::NodeHandle                                       nh_;
  ros::NodeHandle                                       pnh_;
  InitParams*                                           params;
  std::shared_ptr<OntologyInterface>                    ontologyInterface;
  std::shared_ptr<CommunicationInterface>               communicationInterface;
};

} /* namespace ice */

#endif /* ICE_SERVAL_BRIDGE_H_ */
