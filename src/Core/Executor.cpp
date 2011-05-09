/*!
 * \file Executor.cpp
 * \brief
 */

#include "Executor.hpp"
#include "Component.hpp"
#include "Timer.hpp"

#include <boost/foreach.hpp>

namespace Core {

std::vector<std::string> Executor::listComponents() {
	std::vector<std::string> ret;
	BOOST_FOREACH(ComponentPair cp, components) {
		ret.push_back(cp.first);
	}
	return ret;
}


bool Executor::ensureState(ExecutorState st, const std::string & errmsg) {
	if ( (m_state == Loaded) && (st != Loaded) ) {
		LOG(LWARNING) << "Thread " << name() << " is not initialized yet. " << errmsg;
		return false;
	}

	if ( (m_state == Finished) && (st != Finished) ) {
		LOG(LWARNING) << "Thread " << name() << " is already finished. " << errmsg;
		return false;
	}

	if ( (m_state == Running) && (st != Running) ) {
		LOG(LWARNING) << "Thread " << name() << " is running. " << errmsg;
		return false;
	}

	if ( (m_state == Paused) && (st != Paused) ) {
		LOG(LWARNING) << "Thread " << name() << " is paused. " << errmsg;
		return false;
	}

	return true;
}

void Executor::restart() {
	if (!ensureState(Paused, "Can't restart."))
		return;

	// set state to running
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Running;
	}
	m_cond.notify_all();
}

void Executor::pause() {
	if (!ensureState(Running, "Can't pause."))
		return;

	// set state to paused
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Paused;
	}
	m_cond.notify_all();
}

/*!
 * Initialize all managed components.
 */
void Executor::initialize() {
	if (!ensureState(Paused, "Can't initialize."))
		return;

	// set state to paused
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Paused;
	}
	m_cond.notify_all();
}

/*!
 * Reset execution thread, making it possible to start over again
 * (reinitialize components etc.).
 */
void Executor::reset() {
	/// \todo IMPLEMENT!
}

/*!
 * Finish main Executor loop thus ending associated thread.
 */
void Executor::finish() {
	if (!ensureState(Paused, "Can't finish."))
		return;

	// set state to finished
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Running;
	}
	m_cond.notify_all();
}

void Executor::run() {
	boost::unique_lock<boost::mutex> lock(m_cond_mtx);

	// wait until executor is initialized
	while(m_state == Loaded) {
		m_cond.wait(lock);
	}
}

}//: namespace Core

