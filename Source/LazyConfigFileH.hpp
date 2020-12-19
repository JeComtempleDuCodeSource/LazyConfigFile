#pragma once

#ifndef LAZYCONFIGFILE_HPP
#define LAZYCONFIGFILE_HPP

#include <fstream>
#include <string>
#include <vector>

namespace LCF
{   
    // Row
    typedef struct _configValue
    {
        std::string Key;
        std::string Value;
    }configValue;

    bool isInteger(std::string stringToCheck)
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
            return (configValue){"NOT FOUND", "NOT FOUND"};
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
            return (configValue){"NOT FOUND", "NOT FOUND"};
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
            return NULL;
        }

        // Extract rows one by one
        public: LazyConfigIn& operator>>(configValue& V)
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
            std::string strValue = std::to_string(Value);
            outConfiguration.push_back((configValue){Key, strValue});
        }

        // Overload that take directly a integer as value
        public: void writeValue(std::string Key, long long int Value)
        {
            outConfiguration.push_back(Value);
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
        }

        // Clear buffered rows
        public: void clearBuffered()
        {
            this->outConfiguration.clear();
        }
        
        // Close file
        public: ~LazyConfigOut()
        {
            this->outputFile.close();
            this->outConfiguration.clear();
        }
    };
}
#endif /* LAZYCONFIGFILE_HPP */