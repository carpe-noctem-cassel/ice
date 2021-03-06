/*
 * SubModelMessage.cpp
 *
 *  Created on: 04.08.2016
 *      Author: sni
 */

#include "ice/communication/messages/SubModelMessage.h"

#include <string.h>

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ice/Entity.h"
#include "ice/model/ProcessingModel.h"

namespace ice
{

SubModelMessage::SubModelMessage() : Message(IceMessageIds::IMI_SUBMODEL, true)
{
  _log = el::Loggers::getLogger("SubModelMessage");
}

SubModelMessage::~SubModelMessage()
{
  //
}

std::shared_ptr<SubModelDesc>& SubModelMessage::getSubModel()
{
  return this->subModel;
}

void SubModelMessage::setSubModel(std::shared_ptr<SubModelDesc> &desc)
{
  this->subModel = desc;
}

void SubModelMessage::payloadToJson(rapidjson::Document &document)
{
  std::stringstream ss;
  boost::archive::text_oarchive ar(ss);
  ar << this->subModel;

  string str = ss.str();
  this->entity->toShortIris(str);

  document.SetString(str.c_str(), document.GetAllocator());
}

bool SubModelMessage::parsePayload(rapidjson::Document& value, std::shared_ptr<GContainerFactory> factory)
{
  if (false == value.IsString())
  {
    _log->error("Payload could not be parsed: Is not a string");
    return false;
  }

  std::stringstream ss;
  ss.str(value.GetString());
  string str = ss.str();
  this->entity->toLongIris(str);
  ss.str(str);
  boost::archive::text_iarchive ar(ss);
  ar >> this->subModel;

  return true;
}

} /* namespace ice */
