#include <LazyConfigFileH.hpp>
#include <fstream>
#include <iostream>

int main()
{
	// Init config file
	LCF::LazyConfigOut outConfig;
	
	// Write values
	outConfig.writeValue("Usefullness", (int)0);
	outConfig << (LCF::ConfigValue){"Name", "LazyConfigFile"};

	// Write and clean
	outConfig.saveConfig("Config.etpcf");
	outConfig.clearConfig();

	// Check if file exists
	std::ifstream configFile("Config.etpcf");
	if (!configFile.is_open())
		throw std::invalid_argument("Config.etpcf file can't be accessed");
	configFile.close();

	// Extract integer
	LCF::LazyConfigIn* Configuration = new LCF::LazyConfigIn("Config.etpcf");
	long long int levelOfUsefulNess;
	if (LCF::isInt((*Configuration)["Usefullness"]))
		levelOfUsefulNess = std::stoll((*Configuration)["Usefullness"]);
	std::cout << levelOfUsefulNess << std::endl;
	
	// Extract string
	std::string programName = (*Configuration)["Name"];
	std::cout << programName << std::endl;

	// Extract specific index
	LCF::ConfigValue secondElement = (*Configuration)[1];
	std::cout << secondElement.Value << std::endl;

	// To output all values you can do this 
	for (const LCF::ConfigValue& currentValue : Configuration->getConfig())
	{
		std::cout << currentValue.Key << " -> " << currentValue.Value << std::endl;
	}
	// Or this
	for (unsigned int Count = 0; Count < Configuration->getConfig().size(); Count++)
	{
		LCF::ConfigValue currentValue;
		(*Configuration) >> currentValue;
		std::cout << currentValue.Key << " -> " << currentValue.Value << std::endl;
	}
	delete Configuration;
	return 0;
}