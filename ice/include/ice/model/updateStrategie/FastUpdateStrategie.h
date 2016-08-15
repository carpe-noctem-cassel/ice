/*
 * FastUpdateStrategie.h
 *
 *  Created on: Sep 17, 2015
 *      Author: sni
 */

#ifndef FASTUPDATESTRATEGIE_H_
#define FASTUPDATESTRATEGIE_H_

#include "ice/model/updateStrategie/UpdateStrategie.h"

namespace ice
{

class FastUpdateStrategie : public UpdateStrategie
{
public:
  FastUpdateStrategie(std::weak_ptr<ICEngine> engine);
  virtual ~FastUpdateStrategie();

  void update(std::shared_ptr<ProcessingModel> model);
  void initInternal();
  void cleanUpInternal();
  bool handleSubModel(std::shared_ptr<Entity> &entity, std::shared_ptr<SubModelDesc> &subModel);
  bool handleSubModelResponse(std::shared_ptr<Entity> &entity, int modelIndex);
  void onEntityDiscovered(std::shared_ptr<Entity> &entity);
};

} /* namespace ice */

#endif /* FASTUPDATESTRATEGIE_H_ */
