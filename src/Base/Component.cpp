/*!
 * \file Component.cpp
 * \brief Abstract interface for all components.
 */

#include "Component.hpp"

#include "Event.hpp"
#include "EventHandler.hpp"
#include "DataStreamInterface.hpp"
#include "Logger.hpp"
#include "Timer.hpp"
#include "Property.hpp"


#include <boost/foreach.hpp>

namespace Base {


Component::~Component()
{
	// delete all events and handlers
	BOOST_FOREACH(EventPair event, events) {
		delete event.second;
	}
}

//------------------------------------------------------------------------------------
// State change methods
//------------------------------------------------------------------------------------
bool Component::initialize() {
	if (state == Unready) {
		if (onInit()) {
			state = Ready;
			return true;
		} else {
			return false;
		}
	}

	if (state == Ready) {
		LOG(LWARNING) << name_ << " already initialized.\n";
		return true;
	}

	if (state == Running) {
		LOG(LWARNING) << name_ << " already initialized and running.\n";
		return true;
	}

	return false;
}

bool Component::start() {
	if (state == Ready) {
		if (onStart()) {
			state = Running;
			return true;
		} else {
			return false;
		}
	}

	if (state == Running) {
		LOG(LWARNING) << name_ << " already running.\n";
		return true;
	}

	if (state == Unready) {
		LOG(LWARNING) << name_ << " is not ready to run.\n";
		return false;
	}

	return false;
}

bool Component::stop() {
	if (state == Running) {
		if (onStop()) {
			state = Ready;
			return true;
		} else {
			return false;
		}
	}

	if (state == Ready) {
		LOG(LWARNING) << name_ << " already stopped.\n";
		return true;
	}

	if (state == Unready) {
		LOG(LWARNING) << name_ << " is not initialized.\n";
		return false;
	}

	return false;
}

bool Component::finish() {
	if (state == Ready) {
		if (onFinish()) {
			state = Unready;
			return true;
		} else {
			return false;
		}
	}

	if (state == Unready) {
		LOG(LWARNING) << name_ << " is already finished.\n";
		return true;
	}

	if (state == Running) {
		LOG(LWARNING) << name_ << " still running. Trying to stop...\n";
		if (stop())
			LOG(LWARNING) << name_ << " stopped. Finishing...\n";
		else
			LOG(LWARNING) << name_ << " didn't stop. Finishing anyway...\n";

		onFinish();

		return false;
	}

	return false;
}

double Component::step() {
	Common::Timer timer;
	if (state == Running) {
		timer.restart();
		onStep();
		return timer.elapsed();
	} else {
		LOG(LWARNING) << name_ << " is not running. Step can't be done.\n";
		return 0;
	}

	return 0;
}

//------------------------------------------------------------------------------------
// State check methods
//------------------------------------------------------------------------------------

bool Component::running() const {
	return state == Running;
}

bool Component::initialized() const {
	return state == Ready;
}

//------------------------------------------------------------------------------------
// Event related methods
//------------------------------------------------------------------------------------

void Component::printEvents() {
	LOG(LINFO) << "Registered events:\n";
	BOOST_FOREACH(EventPair event, events) {
		LOG(LINFO) << "\t" << event.first << "\n";
	}
}

Event * Component::getEvent(const std::string& name) {
	if (events.count(name) > 0) {
		return events[name];
	} else {
		return NULL;
	}
}

Event * Component::registerEvent(const std::string& name) {
	/// \todo check, if event already exists
	Event * event = new Event();
	events[name] = event;
	return event;
}

//------------------------------------------------------------------------------------
// Event handler related methods
//------------------------------------------------------------------------------------

void Component::printHandlers() {
	LOG(LINFO) << "Registered handlers:\n";
	BOOST_FOREACH(HandlerPair handler, handlers) {
		LOG(LINFO) << "\t" << handler.first << "\n";
	}
}

std::string Component::listHandlers() {
	std::string ret;
	BOOST_FOREACH(HandlerPair handler, handlers) {
		ret += handler.first + "\n";
	}
	return ret;
}

EventHandlerInterface * Component::getHandler(const std::string& name) {
	if (handlers.count(name) > 0) {
		return handlers[name];
	} else {
		return NULL;
	}
}

EventHandlerInterface * Component::registerHandler(const std::string& name, EventHandlerInterface * handler) {
	/// \todo check, if handler already exists
	handlers[name] = handler;
	return handler;
}

//------------------------------------------------------------------------------------
// Data stream related methods
//------------------------------------------------------------------------------------

void Component::printStreams() {
	LOG(LINFO) << "Registered data streams:\n";
	BOOST_FOREACH(StreamPair stream, streams) {
		LOG(LINFO) << "\t" << stream.first << "\n";
	}
}

DataStreamInterface * Component::getStream(const std::string& name) {
	if (streams.count(name) > 0) {
		return streams[name];
	} else {
		return NULL;
	}
}

DataStreamInterface * Component::registerStream(const std::string& name, DataStreamInterface * stream) {
	/// \todo check, if handler already exists
	streams[name] = stream;
	return stream;
}

//------------------------------------------------------------------------------------
// Property related methods
//------------------------------------------------------------------------------------

void Component::printProperties() {
	if (properties.empty())
		return;

	LOG(LINFO) << "Registered properties:";
	BOOST_FOREACH(PropertyPair prop, properties) {
		LOG(LINFO) << "\t" << prop.first;
	}
}

std::string Component::listProperties() {
	std::string ret;
	BOOST_FOREACH(PropertyPair prop, properties) {
		ret += prop.first + "\n";
	}
	return ret;
}

std::vector<std::string> Component::getAllProperties() {
	std::vector<std::string> ret;
	BOOST_FOREACH(PropertyPair prop, properties) {
		ret.push_back( prop.first );
	}
	return ret;
}

PropertyInterface * Component::getProperty(const std::string& name) {
	if (properties.count(name) > 0) {
		return properties[name];
	} else {
		return NULL;
	}
}

PropertyInterface * Component::registerProperty(const std::string& name, PropertyInterface * prop) {
	/// \todo check, if handler already exists
	properties[name] = prop;
	return prop;
}

PropertyInterface * Component::registerProperty(PropertyInterface & prop) {
	/// \todo check, if handler already exists
	properties[prop.name()] = &prop;
	return &prop;
}




//------------------------------------------------------------------------------------
// Deprecated methods
//------------------------------------------------------------------------------------

Props * Component::getProperties() {
	// by default return NULL indicating, that given component has no properties
	return NULL;
}



}//: namespace Base

