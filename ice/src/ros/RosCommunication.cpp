/*
 * RosCommunication.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: sni
 */

#include "ice/ros/RosCommunication.h"

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ice/container/Position.h"
#include "ice/processing/LambdaTask.h"
#include "ice/processing/NodeDescription.h"
#include "ice/ros/RosTimeFactory.h"
#include "ice/ICEngine.h"

#include "ice_msgs/Position.h"
#include "easylogging++.h"

namespace ice
{

RosCommunication::RosCommunication(std::weak_ptr<ICEngine> engine) :
    CommunicationInterface(engine), iceId(-1)
{
  this->_log = el::Loggers::getLogger("RosCommunication");
  this->running = false;
  this->discoveryInterval = 200;
}

RosCommunication::~RosCommunication()
{
  //
}

void RosCommunication::initInternal()
{
  std::string sid;
  this->self->getId(EntityDirectory::ID_ICE, sid);
  this->iceId = std::stoul(sid);

  // init ros channels
  this->heartbeatPublisher = this->nodeHandel.advertise<ice_msgs::Heartbeat>("ice/heartbeat", 100);
  this->heartbeatSubscriber = this->nodeHandel.subscribe("ice/heartbeat", 100, &RosCommunication::onHeartbeat, this);
  this->coordinationPublisher = this->nodeHandel.advertise<ice_msgs::ICECoordination>("ice/coordination", 100);
  this->coordinationSubscriber = this->nodeHandel.subscribe("ice/coordination", 100, &RosCommunication::onCoordination, this);

  this->running = true;
  this->worker = std::thread(&RosCommunication::spin, this);
}

void RosCommunication::cleanUpInternal()
{
  this->heartbeatPublisher.shutdown();
  this->heartbeatSubscriber.shutdown();
  this->coordinationPublisher.shutdown();
  this->coordinationSubscriber.shutdown();

  if (this->running == false)
    return;

  this->running = false;
  this->worker.join();
}

void RosCommunication::discover()
{
  _log->verbose(1, "Sending heartbeat");

  ice_msgs::Heartbeat heartbeat;
  heartbeat.header.senderId.value = this->iceId;
  heartbeat.header.timestamp = ros::Time::now();

  this->heartbeatPublisher.publish(heartbeat);

  // discovery is done in onHeartbeat
}

int RosCommunication::readMessage(std::vector<std::shared_ptr<Message>> &outMessages)
{
  return 0;
}

void RosCommunication::sendMessage(std::shared_ptr<Message> msg)
{
  _log->info("Sending message for job '%v' with index '%v' to '%v'", std::to_string(msg->getJobId()),
             std::to_string(msg->getJobIndex()), msg->getEntity()->toString());

  std::string sid;
  if (false == msg->getEntity()->getId(EntityDirectory::ID_ICE, sid))
  {
    this->_log->error("Trying to send message with ROS to none ice instance %v", msg->getEntity()->toString());
    return;
  }
  ice_msgs::ICECoordination coordinationMsg;
  coordinationMsg.header.senderId.value = this->iceId;

  ice_msgs::Identifier receiver;
  receiver.value = std::stoul(sid);
  coordinationMsg.header.receiverIds.push_back(receiver);

  coordinationMsg.id = msg->getId();
  coordinationMsg.jobId = msg->getJobId();
  coordinationMsg.jobIndex = msg->getJobIndex();

  if (msg->isPayload())
  {
    std::string json = msg->toJson();
    int size = json.size();

    if (size > 1024)
    {
//      this->_log->error("Message could not be send to instance '%v', to large '%v' byte", msg->getEntity()->toString(), size);
//      return;
    }
    coordinationMsg.bytes.resize(size);
    std::copy(json.begin(), json.end(), coordinationMsg.bytes.begin());
  }

  this->coordinationPublisher.publish(coordinationMsg);
}

std::shared_ptr<BaseInformationSender> RosCommunication::createSender(std::shared_ptr<InformationCollection> collection)
{
  if (collection->isGContainer())
  {
    _log->debug("Creating sender for stream %v with GContainer",
                collection->getName());

    auto sender = std::make_shared<RosGContainerSender>(collection, this->iceId, &this->nodeHandel,
                                                        this->createTopic(collection), 100);

    return sender;
  }

  auto type = collection->getTypeInfo();

  if (typeid(Position) == *type)
  { // ice::Position -> ice_msgs::Position
    _log->debug("Creating sender for stream %v with mapping ice::Position -> ice_msgs::Position",
                collection->getName());
    transformC2M<Position, ice_msgs::Position> method = &RosMessageTransform::transformC2MPosition;
    return this->_createSender<Position, ice_msgs::Position>(collection, method);
  }

  return nullptr;
}

std::shared_ptr<InformationReceiver> RosCommunication::createReceiver(std::shared_ptr<InformationCollection> collection)
{
  if (collection->isGContainer())
  {
    _log->debug("Creating sender for stream %v with GContainer",
                collection->getName());

    auto receiver = std::make_shared<RosGContainerReceiver>(collection, this->timeFactory, this->iceId, &this->nodeHandel,
                                                        this->createTopic(collection), 100, this->containerFactory);

    return receiver;
  }

  auto type = collection->getTypeInfo();

//  if (typeid(Position) == *type)
//  { // ice_msgs::Position -> ice::Position
//    _log->debug("Creating receiver for stream %v with mapping ice_msgs::Position -> ice::Position",
//                collection->getName());
//    transformM2C<Position, ice_msgs::Position> method = &RosMessageTransform::transformM2CPosition;
//    return this->_createReceiver<Position, ice_msgs::Position>(collection, method);
//  }

  return nullptr;
}

void RosCommunication::spin()
{
  ros::Rate loop_rate(30);

  while (this->running && this->nodeHandel.ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
  }
}

// ROS listener methods

void RosCommunication::onHeartbeat(const ice_msgs::Heartbeat::ConstPtr& msg)
{
  if (false == this->running)
    return;

  std::string sid = std::to_string(msg->header.senderId.value);
  auto entity = this->directory->lookup(EntityDirectory::ID_ICE, sid);

  if (entity == nullptr)
  {
    // Create new instance and request ids
    entity = this->directory->create(EntityDirectory::ID_ICE, sid);
    // At the beginning each discovered node is expected to be an ice node
    entity->setAvailable(true);
    _log->info("New ID discovered: %v", entity->toString());
    this->discoveredEntity(entity);
  }

  entity->setActiveTimestamp();

  identifier senderId = msg->header.senderId.value;

  if (senderId == this->iceId)
    return;

  _log->debug("Heartbeat received from engine %v", IDGenerator::toString(senderId));
}

void RosCommunication::onCoordination(const ice_msgs::ICECoordination::ConstPtr& msg)
{
  if (false == this->running)
    return;

  identifier senderId = msg->header.senderId.value;

  if (false == this->checkReceiverIds(msg->header) || senderId == this->iceId)
    return;

  std::string sid = std::to_string(msg->header.senderId.value);
  auto entity = this->directory->lookup(EntityDirectory::ID_ICE, sid, false);

  if (entity == nullptr)
  {
    this->_log->info("Received message from unknown ice instance '%v'", sid);

    // Create new instance and request ids
    entity = this->directory->create(EntityDirectory::ID_ICE, sid);
    // At the beginning each discovered node is expected to be an ice node
    entity->setAvailable(true);
    this->discoveredEntity(entity);
  }

  std::string json(msg->bytes.begin(), msg->bytes.end());

  auto message = this->parse(msg->id, json, entity);

  if (message == nullptr)
  {
    this->_log->info("Received unknown or broken message from serval node '%v'", sid);

    return;
  }

  message->setJobId(msg->jobId);
  message->setJobIndex(msg->jobIndex);

  this->handleMessage(message);
}

bool RosCommunication::checkReceiverIds(ice_msgs::ICEHeader header)
{
  for (auto id : header.receiverIds)
  {
    identifier receiverId = id.value;

    if (receiverId == this->iceId)
      return true;
  }

  return false;
}

std::string RosCommunication::createTopic(std::shared_ptr<InformationCollection> &collection)
{
  auto &spec = collection->getDescription()->getInformationSpecification();
  if (collection->getCollectionType() == CollectionType::CT_SET)
  {
    return this->ontology->toShortIriAll("ice/set/" + spec->getEntityType() + "/" + spec->getScope() + "/" + std::to_string(collection->getHash()));
  }
  else if (collection->getCollectionType() == CollectionType::CT_STREAM)
  {
    return this->ontology->toShortIriAll("ice/stream/" + spec->getEntity() + "/" + spec->getScope() + "/" + std::to_string(collection->getHash()));
  }
  else
  {
    return "";
  }
}

} /* namespace ice */
