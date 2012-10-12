/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>

#include "Example.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace Example {

Example::Example(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello Example\n";
}

Example::~Example()
{
	LOG(LTRACE) << "Good bye Example\n";
}

bool Example::onInit()
{
	LOG(LTRACE) << "Example::initialize\n";

	// Register data streams, events and event handlers HERE!

	return true;
}

bool Example::onFinish()
{
	LOG(LTRACE) << "Example::finish\n";

	return true;
}

bool Example::onStep()
{
	LOG(LTRACE) << "Example::step\n";
	return true;
}

bool Example::onStop()
{
	return true;
}

bool Example::onStart()
{
	return true;
}

}//: namespace Example
}//: namespace Processors
