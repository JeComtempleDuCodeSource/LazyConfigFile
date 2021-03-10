#ifndef LAZYCONFIGFILEH_HPP
#define LAZYCONFIGFILEH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

namespace LCF
{   
    // Row
    typedef struct _configValue
    {
        std::string Key;
        std::string Value;
    }configValue;

    static bool isFloat(std::string Str)
    {
        if (std::stof(std::to_string(std::stof(Str))) == std::stof(Str))
            return true;
        else return false; 
    }

    static bool isInt(std::string Str)
    {
        if (std::to_string(std::stoi(Str)) == Str)
            return true;
        else return false; 
    }

    static bool isuInt(std::string Str)
    {
        if (std::to_string(std::stoul(Str)) == Str)
            return true;
        else return false; 
    }

    static bool isFile(std::string Path)
    {
        std::ifstream inputFile = std::ifstream(Path, std::ios::in);
        if (!inputFile.is_open())
            return false;
        inputFile.close();
        return true;
    }

    class LazyConfigIn
    {
        // Input read config
        private: std::vector<configValue> inConfiguration;

        // Input file we're reading from
        private: std::ifstream inputFile;

        // Overloaded Operator >> count
        private: std::size_t extractCount;

        // Read
        private: void getConfig(std::string inputFileName)
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
            inConfiguration.push_back((configValue){"", ""});
            Target = &inConfiguration[inConfiguration.size() - 1].Key; 
            while (inputFile.get(currentChar))
            {
                // If new line end comment and create empty row
                if (currentChar == '\r' || currentChar == '\n')
                {
                    if (Comment == true)
                    {
                        inConfiguration.push_back((configValue){"", ""});
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
                    inConfiguration.push_back((configValue){"", ""});
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

        // Calls read
        public: LazyConfigIn(std::string inputFileName)
        {
            readConfiguration(inputFileName);
        }

        // Clear buffer and calls getconfig
        public: void readConfiguration(std::string inputFileName)
        {
            if (inputFile.is_open())
                inConfiguration.clear();
            getConfig(inputFileName);
        }

        // Only call getconfig
        public: void concatenateConfiguration(std::string inputFileName)
        {
            getConfig(inputFileName);
        }

        // Get row of given key
        public: configValue getWhere(std::string keyName)
        {
            for (std::size_t Index = 0; Index < inConfiguration.size(); Index++)
            {
                if (inConfiguration[Index].Key == keyName)
                {
                    return inConfiguration[Index];
                }
            }
            std::clog << "LCF: Key \"" << keyName << "\" not found!\n";
            return (configValue){"LCF: NOT FOUND", "LCF: NOT FOUND"};
        }

        // Get row of given key and set as reference the position in the vector of the row
        public: configValue getWhere(std::string keyName, std::size_t& Position)
        {
            for (std::size_t Index = 0; Index < inConfiguration.size(); Index++)
            {
                if (inConfiguration[Index].Key == keyName)
                {
                    Position = Index;
                    return inConfiguration[Index];
                }
            }
            std::clog << "LCF: Key \"" << keyName << "\" not found!\n";
            return (configValue){"LCF: NOT FOUND", "LCF: NOT FOUND"};
        }

        // Return row at vector index
        public: const configValue& operator[](std::size_t Index) const
        {
            return inConfiguration.at(Index);
        }

        // Returns wherever is there this keyname
        public: const std::string& operator[](std::string keyName) const
        {
            for (unsigned int Index = 0; Index < inConfiguration.size(); Index++)
            {
                if (inConfiguration[Index].Key == keyName)
                    return inConfiguration[Index].Value;
            }
            std::clog << "LCF: Key \"" << keyName << "\" not found!\n";
            return NULL;
        }

        // Extract rows one by one
        public: LazyConfigIn& operator>>(configValue& V)
        {
            // If out of range
            if (extractCount >= this->inConfiguration.size())
            {
                std::clog << "LCF: Key \"" << V.Key << "\" not found!\n";
                V = (configValue){"LCF: NOT FOUND", "LCF: NOT FOUND"};
                return (*this);
            }
            V = (*this)[extractCount];
            extractCount++;
            return (*this);
        }

        // Return undelying vector of config values
        public: const std::vector<configValue>& getConfiguration() const
        {
            return this->inConfiguration;
        }
        
        // Close file
        public: ~LazyConfigIn()
        {
            this->inConfiguration.clear();
            this->inputFile.close();
        }
    };

    class LazyConfigOut
    {
        // Buffered rows
        private: std::vector<configValue> outConfiguration;

        // File to write to
        private: std::ofstream outputFile;

        // Calls create config file
        public: LazyConfigOut(std::string outputFileName)
        {
            createConfiguration(outputFileName);
        }

        // Creates configuration file
        public: void createConfiguration(std::string outputFileName)
        {
            if (outputFile.is_open())
                writeBuffered();
            this->outputFile = std::ofstream(outputFileName, std::ios::out);
            outputFile << "# Lazy Config File \"LCF\" or \"ETPCF\" (Easy-To-Parse-Configuration-File)\n";
            outputFile << "# Syntax is dead simple: Key=Value;\n";
        }

        // Write key and value to buffer
        public: void writeValue(std::string Key, std::string Value)
        {
            outConfiguration.push_back((configValue){Key, Value});
        }

        // Overload that take directly a configvalue
        public: void writeValue(configValue Value)
        {
            outConfiguration.push_back(Value);
        }

        // Overload that take directly a integer as value
        public: void writeValue(std::string Key, long long int Value)
        {
            std::string strValue = std::to_string(Value);
            outConfiguration.push_back((configValue){Key, strValue});
        }

        // Inserts to buffer
        public: LazyConfigOut& operator<<(configValue V)
        {
            this->writeValue(V);
            return (*this);
        }
        
        // Appends to current buffered config list another config list
        public: void appendConfiguration(std::vector<configValue> configList)
        {
            this->outConfiguration.insert(outConfiguration.end(), configList.begin(), configList.end());
        }

        // Returns buffered rows
        public: const std::vector<configValue>& getConfiguration() const
        {
            return this->outConfiguration;
        }

        // Write buffered rows
        public: void writeBuffered()
        {
            for (configValue& currentValue : outConfiguration)
            {
                outputFile << currentValue.Key << '=' << currentValue.Value << ";\n";
            }
            outputFile.flush();
        }

        // Clear buffered rows
        public: void clearBuffered()
        {
            this->outConfiguration.clear();
            outputFile.close();
        }
        
        // Close file
        public: ~LazyConfigOut()
        {
            this->outputFile.close();
            this->outConfiguration.clear();
        }
    };

    static bool getInt(LazyConfigIn* configFile, std::string keyName, int& Dest, int Min = std::numeric_limits<int>::lowest(), int Max = std::numeric_limits<int>::max())
    {
        if (!isInt((*configFile)[keyName]))
        {
            std::cerr << "Config File Check: " << (*configFile)[keyName] << " isn't an integer.\n";
            return false;
        }   
        Dest = std::stoi((*configFile)[keyName]);
        if (Dest > Max)
            Dest = Max;
        else if (Dest < Min)
            Dest = Min;
        return true;
    }

    static bool getuInt(LazyConfigIn* configFile, std::string keyName, unsigned int& Dest, unsigned int Min = 0U, unsigned int Max = std::numeric_limits<unsigned int>::max())
    {
        if (!isuInt((*configFile)[keyName]))
        {
            std::cerr << "Config File Check: " << (*configFile)[keyName] << " isn't an positive integer.\n";
            return false;
        }   
        Dest = std::stoul((*configFile)[keyName]);
        if (Dest > Max)
            Dest = Max;
        else if (Dest < Min)
            Dest = Min;
        return true;
    }

    static bool getFloat(LazyConfigIn* configFile, std::string keyName, float& Dest, float Min = std::numeric_limits<float>::lowest(), float Max = std::numeric_limits<float>::max())
    {
        if (!isFloat((*configFile)[keyName]))
        {
            std::cerr << "Config File Check: " << (*configFile)[keyName] << " isn't a floating point number.\n";
            return false;
        }   
        Dest = std::stof((*configFile)[keyName]);
        if (Dest > Max)
            Dest = Max;
        else if (Dest < Min)
            Dest = Min;
        return true;
    }

    static bool getPath(LazyConfigIn* configFile, std::string keyName, std::string& Dest)
    {
        if (!isFile((*configFile)[keyName]))
        {
            std::cerr << "Config File Check: " << (*configFile)[keyName] << " can't be accessed or isn't a file.\n";
            return false;
        }   
        Dest = (*configFile)[keyName];
        return true;
    }
}
#endif /* LAZYCONFIGFILEH_HPP */