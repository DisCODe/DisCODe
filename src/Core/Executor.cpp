/*!
 * \file Executor.cpp
 * \brief
 */

#include "Executor.hpp"
#include "Component.hpp"
#include "EventHandler.hpp"

#include "Timer.hpp"
#include "Logger.hpp"

#include <cstdlib>

#include <boost/foreach.hpp>

namespace Core {

Executor::Executor(const std::string & n) :  m_name(n), m_state(Loaded), m_period(0) {
}

Executor::~Executor() {
}

std::vector<std::string> Executor::listComponents() {
	std::vector<std::string> ret;
	BOOST_FOREACH(ComponentPair cp, components) {
		ret.push_back(cp.first);
	}
	return ret;
}

void Executor::addComponent(const std::string & name, Base::Component * component, int priority) {
	if (priority > 0) {
		size_t i = 0;
		for (i = 0; i < active_components.size(); ++i) {
			if (active_components[i].first > priority) break;
		}

		active_components.insert(active_components.begin() + i, std::make_pair(priority, component));
	}

	components[name] = component;
}

Base::EventHandlerInterface * Executor::scheduleHandler(Base::EventHandlerInterface * h) {
	Base::EventScheduler<Executor> * handler = new Base::EventScheduler<Executor>();
	handler->setup(this, &Executor::queueEvent);
	handler->registerHandler(h);
	return handler;
}

void Executor::queueEvent(Base::EventHandlerInterface * h) {
	mtx.lock();
	queue.push_back(h);
	mtx.unlock();
	m_event_cond.notify_all();
}

void Executor::executeEvents() {
	mtx.lock();
	loc_queue = queue;
	queue.clear();
	mtx.unlock();

	while (!loc_queue.empty()) {
		Base::EventHandlerInterface * h;
		h = loc_queue.front();
		loc_queue.pop_front();
		h->execute();
	}
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

	if ( (m_state == Pausing) && (st != Pausing) ) {
		LOG(LWARNING) << "Thread " << name() << " is pausing. " << errmsg;
		return false;
	}

	if ( (m_state == Starting) && (st != Starting) ) {
		LOG(LWARNING) << "Thread " << name() << " is starting. " << errmsg;
		return false;
	}

	if ( (m_state == Finishing) && (st != Finishing) ) {
		LOG(LWARNING) << "Thread " << name() << " is finishing. " << errmsg;
		return false;
	}


	return true;
}

void Executor::restart() {
	LOG(LTRACE) << "Executor("<<name()<<")::restart";
	if (!ensureState(Paused, "Can't restart."))
		return;

	// set state to running
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Starting;
	}
	m_cond.notify_all();
}

void Executor::pause() {
	LOG(LTRACE) << "Executor::pause";
	if (!ensureState(Running, "Can't pause."))
		return;

	// set state to paused
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Pausing;
	}
	m_cond.notify_all();
	m_event_cond.notify_all();
}

/*!
 * Initialize all managed components.
 */
void Executor::initialize() {
	LOG(LTRACE) << "Executor::initialize";
	if (!ensureState(Loaded, "Can't initialize."))
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
	LOG(LTRACE) << "Executor::finish";
	if (!ensureState(Paused, "Can't finish."))
		return;

	// set state to finished
	{
		boost::lock_guard<boost::mutex> lock(m_cond_mtx);
		m_state = Finishing;
	}
	m_cond.notify_all();
}

void Executor::run() {
	{
		boost::unique_lock<boost::mutex> lock(m_cond_mtx);

		LOG(LINFO) << "Executor " << name() << " thread started.";

		// wait until executor is initialized
		while(m_state == Loaded) {
			m_cond.wait(lock);
		}

		// initialize all components
		BOOST_FOREACH(ComponentPair cmp, components) {
			cmp.second->initialize();
		}

		LOG(LINFO) << "Executor " << name() << " initialized.";
	}

	bool timeout = true;

	for(;;) {

		if (m_state == Starting) {
			boost::unique_lock<boost::mutex> lock(m_cond_mtx);

			// start all components
			if (m_state == Starting) {
				BOOST_FOREACH(ComponentPair cmp, components) {
					cmp.second->start();
				}

				next_wakeup = boost::get_system_time();

				m_state = Running;


				LOG(LINFO) << "Executor " << name() << " components started.";
			}
		}

		if (m_state == Running) {

			// handle all pending events
			LOG(LTRACE) << "Executing events in " << name();
			while (!queue.empty())
				executeEvents();

			if (timeout && !active_components.empty()) {
				BOOST_FOREACH(ComponentPriority cp, active_components) {
					cp.second->step();
				}
			}

			// if period set, then sleep until next wake-up
			if (m_period > 0) {
				boost::unique_lock<boost::mutex> lock(m_event_cond_mtx);

				if (timeout) {
					timeout = false;
					next_wakeup += boost::posix_time::milliseconds(1000 * m_period);
				}

				timeout = !m_event_cond.timed_wait(lock, next_wakeup);
			} else {
				// if there is no period set, then yield thread
				yield();
			}

			LOG(LTRACE) << "Thread " << name() << " works!";

		}

		// double-check if executor is paused
		if ( (m_state == Paused) || (m_state == Pausing) ) {
			boost::unique_lock<boost::mutex> lock(m_cond_mtx);

			LOG(LTRACE) << "Thread " << name() << " pause or pausing...";

			// pause all components
			if (m_state == Pausing) {
				LOG(LTRACE) << "Thread " << name() << " pausing...";
				BOOST_FOREACH(ComponentPair cmp, components) {
					cmp.second->stop();
				}

				m_state = Paused;
			}

			// wait until component is paused
			while(m_state == Paused) {
				m_cond.wait(lock);
			}
		}

		// stop main execution loop
		if (m_state == Finishing)
			break;
	}


	BOOST_FOREACH(ComponentPair cmp, components) {
		cmp.second->finish();
	}

	m_state = Finished;

	LOG(LINFO) << "Executor " << name() << " thread finished.";
}

}//: namespace Core

