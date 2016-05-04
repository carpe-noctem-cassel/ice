/*
 * CommunicationInterface.h
 *
 *  Created on: Apr 15, 2016
 *      Author: sni
 */

#ifndef COMMUNICATIONINTERFACE_H_
#define COMMUNICATIONINTERFACE_H_

#include <map>
#include <memory>

#include <ice/information/InformationSpecification.h>
#include <easylogging++.h>

#include "EntityDirectory.h"

namespace ice
{

class Entity;
class IceServalBridge;

enum IceCmd
{
  SCMD_IDS_REQUEST                      = 10,
  SCMD_IDS_RESPONSE                     = 11,
  SCMD_ID_REQUEST                       = 20,
  SCMD_ID_RESPONSE                      = 21,
  SCMD_INFORMATION_REQUEST              = 30,
  SCMD_INFORMATION_RESPONSE             = 31
};

struct Message
{
  std::shared_ptr<Entity>       entity;
  int                           command;
  std::vector<uint8_t>          payload;
};

class CommunicationInterface
{
public:
  CommunicationInterface();
  virtual ~CommunicationInterface();
  virtual void init();
  virtual void cleanUp();

  virtual void requestId(std::shared_ptr<Entity> const &identity, std::string const &id);
  virtual void onRequestId(std::shared_ptr<Entity> const &identity, std::string const &id);
  virtual void requestIds(std::shared_ptr<Entity> const &identity);
  virtual void onRequestIds(std::shared_ptr<Entity> const &identity);
  virtual void requestOfferedInformation(std::shared_ptr<Entity> const &identity);
  virtual void onRequestOfferedInformation(std::shared_ptr<Entity> const &identity);

  virtual void workerTask();

  // methodes which need to be implemented by child class
  virtual void discover() = 0;
  virtual int readMessage(std::vector<Message> &outMessages) = 0;
  virtual void sendMessage(Message &msg) = 0;

protected:
  virtual void initInternal() = 0;
  virtual void cleanUpInternal() = 0;
  virtual void handleMessage(Message &message);

private:
  void pushMessage(Message &message);

protected:
  IceServalBridge                             *bridge;
  std::shared_ptr<Entity>                     self;
  std::shared_ptr<EntityDirectory>            directory;

  std::thread                                 worker;
  bool                                        running;
  std::vector<Message>                        messages;

  std::mutex                                  _mtx;

private:
  el::Logger                                  *_log;
};

} /* namespace ice */

#endif /* COMMUNICATIONINTERFACE_H_ */
