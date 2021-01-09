#include <LazyConfigFileH.hpp>
#include <fstream>
#include <iostream>

int main()
{
	// Init config file
	LCF::LazyConfigOut outConfig("Config.etpcf");
	
	// Write values
	outConfig.writeValue("Usefullness", 0);
	outConfig << (LCF::configValue){"Name", "LazyConfigFile"};

	// Write and clean
	outConfig.writeBuffered();
	outConfig.clearBuffered();

	// Check if file exists
	std::ifstream configFile("./Config.etpcf");
	if (!configFile.is_open())
		throw std::invalid_argument("./Config.etpcf file can't be accessed");
	configFile.close();

	// Extract integer
	LCF::LazyConfigIn* Configuration = new LCF::LazyConfigIn("./Config.etpcf");
	long long int levelOfUsefulNess;
	if (LCF::isInteger((*Configuration)["Usefullness"]))
		levelOfUsefulNess = std::stoll((*Configuration)["Usefullness"]);
	std::cout << levelOfUsefulNess << std::endl;
	
	// Extract string
	std::string programName = (*Configuration)["Name"];
	std::cout << programName << std::endl;

	// Extract specific index
	LCF::configValue secondElement = (*Configuration)[1];
	std::cout << secondElement.Value << std::endl;

	// To output all values you can do this 
	for (const LCF::configValue& currentValue : Configuration->getConfiguration())
	{
		std::cout << currentValue.Key << " -> " << currentValue.Value << std::endl;
	}
	// Or this
	for (unsigned int Count = 0; Count < Configuration->getConfiguration().size(); Count++)
	{
		LCF::configValue currentValue;
		(*Configuration) >> currentValue;
		std::cout << currentValue.Key << " -> " << currentValue.Value << std::endl;
	}
	delete Configuration;
	return 0;
}