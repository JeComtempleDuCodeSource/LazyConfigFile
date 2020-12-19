#include "LazyConfigFile.hpp"

bool LCF::isInteger(std::string stringToCheck)
{
    int Count = 0;
    if (stringToCheck[0] == '-')
        Count = 1;
    for (; stringToCheck[Count] != 0; Count++)
    {
        if (!isdigit(stringToCheck[Count]))
            return false;
    }
    return true;
}

LCF::LazyConfigIn::LazyConfigIn(std::string inputFileName)
{
    readConfiguration(inputFileName);
}

void LCF::LazyConfigIn::readConfiguration(std::string inputFileName)
{
    if (inputFile.is_open())
        inConfiguration.clear();
    getConfig(inputFileName);
}

void LCF::LazyConfigIn::concatenateConfiguration(std::string inputFileName)
{
    getConfig(inputFileName);
}

void LCF::LazyConfigIn::getConfig(std::string inputFileName)
{
    // Init extract count
    this->extractCount = 0;
	std::ifstream inputFile = std::ifstream(inputFileName, std::ios::in);
    char currentChar = 0;

    // Is comment
    bool Comment = false;

    // Pointer to what we need to write to
    std::string* Target = nullptr;

    // Init blank row and set it as target
    inConfiguration.push_back((LCF::configValue){"", ""});
    Target = &inConfiguration[inConfiguration.size() - 1].Key; 
	while (inputFile.get(currentChar))
	{
        // If new line end comment and create empty row
        if (currentChar == '\r' || currentChar == '\n')
        {
            if (Comment == true)
            {
                inConfiguration.push_back((LCF::configValue){"", ""});
                Target = &inConfiguration[inConfiguration.size() - 1].Key; 
            }
            Comment = false;
        }
        // Skip if comment space
        else if (Comment == true || ((currentChar == ' ' && (*Target).size() < 1)))
        {
            continue;
        }
        // If start of comment
        else if (currentChar == '#')
        {
            Comment = true; 
        }
        // If start of Value and end of key
        else if (currentChar == '=')
        {
			Target = &inConfiguration[inConfiguration.size() - 1].Value; 
        }
        // If end of row
        else if (currentChar == ';')
        {
            inConfiguration.push_back((LCF::configValue){"", ""});
            Target = &inConfiguration[inConfiguration.size() - 1].Key; 
        }
        // Char of key or value
		else
        {
			(*Target).push_back(currentChar); 
        }
	}
	inputFile.close();

    // Erase empty rows
    for (unsigned int Index = 0; Index < inConfiguration.size(); Index++)
    {
        if (inConfiguration[Index].Key == "" || inConfiguration[Index].Value == "" ||
            inConfiguration[Index].Key.size() < 1 || inConfiguration[Index].Value.size() < 1)
            inConfiguration.erase(inConfiguration.begin() + Index);
    }
}

LCF::configValue LCF::LazyConfigIn::getWhere(std::string keyName)
{
    for (std::size_t Index = 0; Index < inConfiguration.size(); Index++)
    {
        if (inConfiguration[Index].Key == keyName)
        {
            return inConfiguration[Index];
        }
    }
    return (LCF::configValue){"NOT FOUND", "NOT FOUND"};
}

LCF::configValue LCF::LazyConfigIn::getWhere(std::string keyName, std::size_t& Position)
{
    for (std::size_t Index = 0; Index < inConfiguration.size(); Index++)
    {
        if (inConfiguration[Index].Key == keyName)
        {
            Position = Index;
            return inConfiguration[Index];
        }
    }
    return (LCF::configValue){"NOT FOUND", "NOT FOUND"};
}

const LCF::configValue& LCF::LazyConfigIn::operator[](std::size_t Index) const
{
    return inConfiguration.at(Index);
}

const std::string& LCF::LazyConfigIn::operator[](std::string keyName) const
{
    for (unsigned int Index = 0; Index < inConfiguration.size(); Index++)
    {
        if (inConfiguration[Index].Key == keyName)
            return inConfiguration[Index].Value;
    }
    return NULL;
}

LCF::LazyConfigIn& LCF::LazyConfigIn::operator>>(LCF::configValue& V)
{
    // If out of range
    if (extractCount >= this->inConfiguration.size())
    {
        V = (configValue){"NOT FOUND", "NOT FOUND"};
        return (*this);
    }
    V = (*this)[extractCount];
    extractCount++;
    return (*this);
}

const std::vector<LCF::configValue>& LCF::LazyConfigIn::getConfiguration() const
{
    return this->inConfiguration;
}

LCF::LazyConfigIn::~LazyConfigIn()
{
    this->inConfiguration.clear();
    this->inputFile.close();
}

LCF::LazyConfigOut::LazyConfigOut(std::string outputFileName)
{
    createConfiguration(outputFileName);
}


void LCF::LazyConfigOut::createConfiguration(std::string outputFileName)
{
    if (outputFile.is_open())
        writeBuffered();
    this->outputFile.close();
    this->outputFile.open(outputFileName, std::ios::out);
    outputFile << "# Lazy Config File \"LCF\" or ETPCF, Easy-To-Parse-Configuration-File\n";
    outputFile << "# Syntax is Key=Value;\n";
}

void LCF::LazyConfigOut::writeValue(std::string Key, std::string Value)
{
    outConfiguration.push_back((LCF::configValue){Key, Value});
}

void LCF::LazyConfigOut::writeValue(std::string Key, long long int Value)
{
    std::string strValue = std::to_string(Value);
    outConfiguration.push_back((LCF::configValue){Key, strValue});
}

void LCF::LazyConfigOut::writeValue(LCF::configValue Value)
{
    outConfiguration.push_back(Value);
}

LCF::LazyConfigOut& LCF::LazyConfigOut::operator<<(LCF::configValue V)
{
    this->writeValue(V);
    return (*this);
}

void LCF::LazyConfigOut::appendConfiguration(std::vector<LCF::configValue> configList)
{
    this->outConfiguration.insert(outConfiguration.end(), configList.begin(), configList.end());
}

const std::vector<LCF::configValue>& LCF::LazyConfigOut::getConfiguration() const
{
    return this->outConfiguration;
}

void LCF::LazyConfigOut::writeBuffered()
{
    for (LCF::configValue& currentValue : outConfiguration)
    {
        outputFile << currentValue.Key << '=' << currentValue.Value << ";\n";
    }
    outputFile.flush();
}

void LCF::LazyConfigOut::clearBuffered()
{
    this->outConfiguration.clear();
    outputFile.close();
}

LCF::LazyConfigOut::~LazyConfigOut()
{
    this->outputFile.close();
    this->outConfiguration.clear();
}
