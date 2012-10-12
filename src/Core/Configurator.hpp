/*!
 * \file Configurator.hpp
 * \brief File containing the declaration of Configurator class.
 *
 * \author tkornuta
 * \date Apr 8, 2010
 */

#ifndef CONFIGURATOR_HPP_
#define CONFIGURATOR_HPP_

#include "Task.hpp"

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <map>

namespace Core {

using namespace boost::property_tree;

class ComponentManager;
class ExecutorManager;
class ConnectionManager;

/*!
 * \class Configurator
 * \brief Class responsible for configuration management.
 * Inherits singleton-behavior from the Singleton class.
 * \date April 8 2010
 * \author tkornuta
 */
class Configurator
{
private:

	typedef std::pair<std::string, ptree> TreeNode;

	/*!
	 * Stored configuration.
	 */
	ptree configuration;

	/*!
	 * Configuration filename.
	 */
	std::string configuration_filename;

	ExecutorManager * executorManager;
	ComponentManager * componentManager;
	ConnectionManager * connectionManager;

	/*!
	 * List of all registered locations of DCL
	 */
	std::vector<std::string> dcl_locations;

	/*!
	 * List containing execution thread name for each component
	 */
	std::map<std::string, std::string> component_executor;


	void expandMacros(ptree & pt, const std::vector<std::pair<std::string, std::string> > & dict);

	std::string & substitute(std::string & text, const std::vector<std::pair<std::string, std::string> > & dict);

public:
	Configurator();

	virtual ~Configurator();

	/*!
	 * Loads configuration from xml file.
	 */
	void loadConfiguration(const ptree * config);

	/*!
	 * Prepare task from given task file
	 * @param filename
	 * @param overrides
	 * @return
	 */
	Task loadTask(std::string filename, const std::vector<std::pair<std::string, std::string> > & overrides);

	void loadExecutors(const ptree * node, Task & task);
	void loadComponents(const ptree * node, Task & task);
	void loadEvents(const ptree * node);
	void loadConnections(const ptree * node);

	void setExecutorManager(ExecutorManager * em) {
		executorManager = em;
	}

	void setComponentManager(ComponentManager * km) {
		componentManager = km;
	}

	void setConnectionManager(ConnectionManager * cm) {
		connectionManager = cm;
	}

	const std::vector<std::string> getDCLLocations() const {
		return dcl_locations;
	}
};

}//: namespace Core

#endif /* CONFIGURATOR_HPP_ */
