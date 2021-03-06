/*
 * CommandMessage.h
 *
 *  Created on: Jul 7, 2016
 *      Author: sni
 */

#ifndef COMMANDMESSAGE_H_
#define COMMANDMESSAGE_H_

#include "ice/communication/messages/Message.h"

namespace ice
{

class CommandMessage : public Message
{
public:
  CommandMessage(int id);
  virtual ~CommandMessage();

protected:
  virtual void payloadToJson(rapidjson::Document &document);
  virtual bool parsePayload(rapidjson::Document& value, std::shared_ptr<GContainerFactory> factory);
};

} /* namespace ice */

#endif /* COMMANDMESSAGE_H_ */
