/*!
 * \file src/Core/main.cpp
 * \brief Main body responsible for menu showing and image processing.
 * \author tkornuta
 * \date 11.09.2007
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <utility>

#include <signal.h>

#include "DisCODeException.hpp"
#include "ConnectionManager.hpp"
#include "ComponentManager.hpp"
#include "ExecutorManager.hpp"
#include "ComponentFactory.hpp"
#include "Configurator.hpp"
#include "Executor.hpp"
#include "Logger.hpp"

#include "CommandInterpreter.hpp"
#include "CommandServer.hpp"

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "TaskInformer.hpp"
#include "ComponentInformer.hpp"
#include "SystemInformer.hpp"
#include "ExecutorInformer.hpp"

using namespace std;
using namespace Common;
using namespace Core;

namespace po = boost::program_options;
namespace pt = boost::property_tree;

volatile bool running = true;
bool unstoppable = false;



std::string print(std::vector<std::string> args) {
	std::stringstream ss;
	for (unsigned int i = 0; i < args.size(); ++i) {
		ss << args[i] << ",";
	}

	std::cout << ss.str();

	return ss.str();
}






void terminate (int param) {
	if (unstoppable) {
		std::cout << "\rGet lost! WMAHAHA!\n";
		return;
	}

	std::cout << "\rTerminating program...\n";
	running = false;
}

/*!
 * Main body - creates two threads - one for window and and one
 * for images acquisition/processing.
 */
int main(int argc, char* argv[])
{
	// DisCODe config filename.
	std::string config_name;

	// Task config filename.
	std::string task_name;

	// Task settings overrides
	std::vector <std::string> task_overrides;

	// Log level
	int log_lvl = 2;

	void (*prev_fn)(int);

	prev_fn = signal (SIGINT, terminate);
	if (prev_fn==SIG_IGN) signal (SIGINT,SIG_IGN);

	// =========================================================================
	// === Program command-line options
	// =========================================================================

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("config,C", po::value<std::string>(&config_name)->default_value("config.xml"), "choose config file")
		("create-config,D", "create default configuration file")
		("task,T", po::value<std::string>(&task_name), "choose task file")
		("create-task", "create default task file")
		("log-level,L", po::value<int>(&log_lvl)->default_value(3), "set log severity level")
		("unstoppable","MWAHAHAHA!")
		("set,S",po::value< vector<string> >(&task_overrides),"override task settings")
		("interactive,I", "interactive mode")
	;

	po::variables_map vm;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
	}
	catch (const po::error & u) {
		std::cout << u.what() << "\n";
		return 0;
	}

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}

	// set logger severity level
	LOGGER.setLevel((Utils::Logger::Severity)log_lvl);
	LOGGER.addOutput(new Utils::Logger::ConsoleOutput, (Utils::Logger::Severity)log_lvl);

	if (vm.count("create-task")) {
		cout << "Creating task file " << task_name << "\n";

		std::ofstream cfg(task_name.c_str());

		cfg << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				"<Task>\n"
				"\t<!-- List of components -->\n"
				"\t<Components>\n"
				"\t</Components>\n"
				"\n"
				"\t<!-- Threads of execution -->\n"
				"\t<Executors>\n"
				"\t\t<Thread1 type=\"passive\">\n"
				"\t\t</Thread1>\n"
				"\t</Executors>\n"
				"\n"
				"\t<!-- Event connections -->\n"
				"\t<Events>\n"
				"\t</Events>\n"
				"\n"
				"\t<!-- Data stream assignments -->\n"
				"\t<DataStreams>\n"
				"\t</DataStreams>\n"
				"</Task>\n";

		cfg.close();

		return 0;
	}

	if (vm.count("create-config")) {
		cout << "Creating configuration file " << config_name << "\n";

		std::ofstream cfg(config_name.c_str());

		cfg << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				"<DisCODe>\n"
				"\t<task>blob.xml</task>"
				"</DisCODe>\n";

		cfg.close();

		return 0;
	}

	if (vm.count("unstoppable")) {
		unstoppable = true;
	}

	// =========================================================================
	// === DisCODe configuration
	// =========================================================================

	config_name = vm["config"].as<std::string>();

	pt::ptree conf;
	try {
		read_xml(config_name, conf);
	}
	catch(xml_parser_error&) {
		LOG(LWARNING) << "Configuration file " << config_name + " not found.";
		LOG(LNOTICE) << "Quick fixes:";
		LOG(LNOTICE) << "   specify config file name with -C switch";
		LOG(LNOTICE) << "   create default configuration using -D switch";
	}

	if (!vm.count("task")) {
		if (!conf.get_optional<std::string>("DisCODe.task")) {
			LOG(LERROR) << "No task specified!";
			LOG(LNOTICE) << "Quick fixes:";
			LOG(LNOTICE) << "   specify task name using -T switch";
			LOG(LNOTICE) << "   set default task name in config file";
			LOG(LNOTICE) << "   run DisCODe in interactive mode (-I)";
			exit(EXIT_FAILURE);
		} else {
			task_name = conf.get<std::string>("DisCODe.task");
			LOG(LINFO) << "Task: " << task_name << " from configuration file\n";
		}
	} else {
		LOG(LINFO) << "Task: " << task_name << " from command line\n";
	}

	std::vector<std::pair<std::string, std::string> > overrides;
	for (size_t i = 0; i < task_overrides.size(); ++i) {
		std::vector<std::string> strs;
		boost::split(strs, task_overrides[i], boost::is_any_of("="));
		if (strs.size() == 1) {
			LOG(LWARNING) << strs[0] << "have no assigned value";
		} else {
			overrides.push_back(std::make_pair(strs[0], strs[1]));
		}
	}

	// =========================================================================
	// === Main program part
	// =========================================================================

	Configurator configurator;
	ComponentManager km;
	ExecutorManager em;
	ConnectionManager cm;

	Task task;

	configurator.loadConfiguration(&conf);
	configurator.setExecutorManager(&em);
	configurator.setComponentManager(&km);
	configurator.setConnectionManager(&cm);

	TaskInformer task_informer(task);
	ComponentInformer component_informer(km);
	SystemInformer system_informer(running);
	ExecutorInformer executor_informer(em);

	try {
		CommandServer server;

		server.addInformer(&task_informer);
		server.addInformer(&component_informer);
		server.addInformer(&system_informer);
		server.addInformer(&executor_informer);
		server.printCommands();

		server.start();

		km.initializeComponentsList(configurator.getDCLLocations());

		task = configurator.loadTask(task_name, overrides);
		if (!task.start()) {
			LOG(LFATAL) << "Task::start() returned false\n";
			running = false;
		}


		while(running) {
			Common::Thread::msleep(50);
		}

		Common::Thread::msleep(500);

		task.finish();

		km.release();
		cm.release();
		em.release();

		km.deactivateComponentList();

		server.stop();
	}//: try

	// =========================================================================
	// === Exception handling
	// =========================================================================

	catch (Common::DisCODeException& ex) {
		LOG(LFATAL) << ex.what() << "\n";
		ex.printStackTrace();
		exit(EXIT_FAILURE);
	}
	catch (exception& ex) {
		LOG(LFATAL) << ex.what() << "\n";
		exit(EXIT_FAILURE);
	}
	catch (const char * ex) {
		LOG(LFATAL) << ex << "\n";
		exit(EXIT_FAILURE);
	}
	catch (...) {
		LOG(LFATAL) << "Unknown exception.\n";
		exit(EXIT_FAILURE);
	}//: catch
}
