/*!
 * \file ComponentManager.hpp
 * \brief File containing the ComponentManager class
 */

#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "Component_Aux.hpp"
#include "DisCODeException.hpp"
#include "SharedLibraryCommon.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
#include "ComponentFactory.hpp"

using namespace std;

namespace Core {

/*!
 * \class ComponentManager
 * \brief
 */
class ComponentManager
{
protected:
	/*!
	 * List of component factories properly loaded by the manager.
	 */
	boost::ptr_map <string, Core::ComponentFactory> component_factories;

	/*!
	 * List of created components
	 */
	std::map <string, Base::Component*> components;

	typedef std::pair<std::string, Base::Component *> comp_pair;

public:
	/*!
	 * Constructor
	 */
	ComponentManager()
	{
		LOG(LTRACE) << "ComponentManager: Hello private \n";
	}

	/*!
	 * Public destructor.
	 */
	~ComponentManager()
	{
		LOG(LTRACE) << "ComponentManager: Goodbye public\n";
	}

	void release() {
		BOOST_FOREACH(comp_pair comp, components) {
			LOG(LINFO) << "Removing component: " << comp.first << " (ptr=" << comp.second << ")";
			delete comp.second;
		}
	}

	/*!
	 * Method tries to create components from all shared libraries loaded from the . directory.
	 */
	void initializeComponentsList(const std::vector<std::string> dcl_locations)
	{

		if (dcl_locations.size() < 1) {
			LOG(LWARNING) << "No DCL locations speecified.";
			return;
		}

		std::string dcl_location;
		std::vector<std::string> dcls;
		BOOST_FOREACH(dcl_location, dcl_locations) {
			std::vector<std::string> tmp_dcls = Utils::getSubdirs(dcl_location, true);
			dcls.insert( dcls.end(), tmp_dcls.begin(), tmp_dcls.end() );
		}


		BOOST_FOREACH(dcl_location, dcls) {

			// Get filenames.
			vector <string> files;
			try {
				getSOList(dcl_location + "/dist/lib", files);
			}
			catch(...) {
			}

			// Check number of so's to import.
			if (files.empty()) {
				LOG(LWARNING) << "ComponentManager: There are no dynamic libraries in " << dcl_location;
			}

			// Iterate through so names and add retrieved components to list.
			BOOST_FOREACH(string file, files)
			{
				// Create component empty "shell".
				ComponentFactory* k = new ComponentFactory();
				// Try to initialize component.
				if (k->lazyInitialize(file))
				{
					// Add component to list.
					component_factories.insert(k->getName(), k);
				}
				else
					// Delete incorrect component.
					delete (k);
			}//: FOREACH

		}//: FOREACH

		// Check number of successfully loaded components.
		LOG(LNOTICE) << "Found " << component_factories.size() << " components\n";
	}

	/*!
	 *
	 */
	void deactivateComponentList() {
		component_factories.release();
	}

	/*!
	 *
	 * @param name
	 * @param type
	 * @return
	 */
	Base::Component* createComponent(const std::string & name, const std::string & type) {
		if (components.count(name) > 0) {
			LOG(LWARNING) << "Module " << name << " already created. Returning previous one.\n";
			return components[name];
		}

		if (component_factories.count(type) < 1) {
			LOG(LERROR) << "Module type " << type << " not found!\n";
			throw Common::DisCODeException("createComponent");
		}

		components[name] = component_factories[type].create(name);
		LOG(LINFO) << name << " (" << type << ") component created\n";
		return components[name];
	}

	/*!
	 *
	 * @param name
	 * @return
	 */
	Base::Component * getComponent(const std::string & name) {
		if (components.count(name) < 1) {
			LOG(LERROR) << "Module " << name << " can't be found!\n";
			throw Common::DisCODeException("getComponent");
		}

		return components[name];
	}

protected:

	/*!
	 *
	 * @param dir_
	 * @param files
	 */
	void getSOList(string dir_, vector <string>& files)
	{
		std::string regexp = "\\w*.";
		regexp += LIB_EXT;

		LOG(LTRACE) << "LIB_EXT = " LIB_EXT << "\n";

		files = Utils::searchFiles(dir_, regexp);
	}

};

}//: namespace Core

#endif /* COMPONENTMANAGER_HPP_ */
