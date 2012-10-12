/*!
 * \file
 * \brief
 */

#ifndef EXAMPLE_HPP_
#define EXAMPLE_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"

namespace Processors {
namespace Example {

/*!
 * \class Example
 * \brief Example processor class.
 */
class Example: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	Example(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~Example();


protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();

};

}//: namespace Example
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("Example", Processors::Example::Example, Common::Panel_Empty)

#endif /* EXAMPLE_HPP_ */
