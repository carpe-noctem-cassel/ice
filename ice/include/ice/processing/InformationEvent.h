/*
 * InformationEvent.h
 *
 *  Created on: May 21, 2014
 *      Author: sni
 */

#ifndef INFORMATION_EVENT_H_
#define INFORMATION_EVENT_H_

#include <memory>

#include "ice/information/InformationElement.h"
#include "ice/processing/AsynchronousTask.h"
#include "ice/information/InformationCollection.h"
#include "ice/information/AbstractInformationListener.h"

namespace ice
{

//* InformationEvent
/**
 * This class is an event triggered by a new information within a stream. The event executes
 * the registered listener asynchronous.
 *
 */
template<typename T>
  class InformationEvent : public AsynchronousTask
  {
  public:

    /*!
     * \brief Default constructor
     *
     * Default constructor
     *
     * \param listener The listener to notify about a new element within a stream.
     * \param element The new element.
     * \param stream The stream.
     */
    InformationEvent(std::shared_ptr<AbstractInformationListener<T>> listener,
                     std::shared_ptr<InformationElement<T>> element, std::shared_ptr<InformationCollection> collection)
    {
      this->listener = listener;
      this->element = element;
      this->collection = collection;
    }

    /*!
     * \brief Default destructor
     *
     * Default destructor
     */
    virtual ~InformationEvent()
    {
      //
    }

    /*!
     * \brief Executes the asynchronous task.
     *
     * Executes the asynchronous task.
     */
    virtual int performTask()
    {
      return this->listener->newEvent(element, collection);
    }

  private:
    std::shared_ptr<AbstractInformationListener<T>> listener; /**< The listener to call for asynchronous operation */
    std::shared_ptr<InformationElement<T>> element; /**< The new information element */
    std::shared_ptr<InformationCollection> collection; /**< The collection of the new information element */
  };

} /* namespace ice */

#endif /* ASYNCHRONOUS_EVENT_H_ */
