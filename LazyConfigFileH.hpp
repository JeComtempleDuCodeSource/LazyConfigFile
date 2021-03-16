#ifndef LAZYCONFIGFILEH_HPP
#define LAZYCONFIGFILEH_HPP

/*
LazyConfigFile v1.0.0 the 19/12/2020
LazyConfigFile v1.1.0 the 13/03/2021
                            THE BARE-BONES CONFIG FILE

 __  __ ___ _   _ ___ __  __    _    _      
|  \/  |_ _| \ | |_ _|  \/  |  / \  | |     
| |\/| || ||  \| || || |\/| | / _ \ | |     
| |  | || || |\  || || |  | |/ ___ \| |___  
|_|  |_|___|_| \_|___|_|  |_/_/   \_\_____| 

  ____ ___  _   _ _____ ___ ____ 
 / ___/ _ \| \ | |  ___|_ _/ ___|
| |  | | | |  \| | |_   | | |  _ 
| |__| |_| | |\  |  _|  | | |_| |
 \____\___/|_| \_|_|   |___\____|
                                                                             
 _____ ___ _     _____ 
|  ___|_ _| |   | ____|
| |_   | || |   |  _|  
|  _|  | || |___| |___ 
|_|   |___|_____|_____|

*/

#define LAZYCONFIGFILE_MAJOR 1
#define LAZYCONFIGFILE_MINOR 1
#define LAZYCONFIGFILE_PATCH 0

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

namespace LCF
{   
    // Row
    typedef struct Config_Value
    {
        std::string Key;
        std::string Value;
    }ConfigValue;

    inline bool isFloat(std::string Str)
    {
        if (std::stof(std::to_string(std::stof(Str))) == std::stof(Str))
            return true;
        else return false; 
    }

    inline bool isInt(std::string Str)
    {
        if (std::to_string(std::stoi(Str)) == Str)
            return true;
        else return false; 
    }

    inline bool isuInt(std::string Str)
    {
        if (std::to_string(std::stoul(Str)) == Str)
            return true;
        else return false; 
    }

    inline bool isFile(std::string Path)
    {
        std::ifstream inputFile = std::ifstream(Path, std::ios::in);
        if (!inputFile.is_open())
            return false;
        inputFile.close();
        return true;
    }

    inline bool isBool(std::string Str)
    {
        if (Str == "TRUE" || Str == "FALSE" || 
            Str == "ON" || Str == "OFF" ||
            Str == "true" || Str == "false" || 
            Str == "on" || Str == "off" ||
            Str == "1" || Str == "0"
        )
        return true;
        else return false;
    }

    class LazyConfigIn
    {
        // Read config vector
        private: std::vector<ConfigValue> Config;

        // Name of last file read
        private: std::string lastFileName;

        // Count of LazyConfigIn& operator>>(ConfigValue& Value)
        private: std::size_t extractCount;

        // Read
        private: void readFile(const std::string& inputFileName)
        {
            // Open
            std::ifstream inputFile = std::ifstream(inputFileName, std::ios::in);
            lastFileName = inputFileName;

            int currentChar = inputFile.get();
            while (!inputFile.eof())
            {
                if (currentChar == '\r' || currentChar == '\n')
                {
                    currentChar = inputFile.get();
                    continue;
                }
                else if (currentChar == '#')
                {
                    // Read until new line
                    currentChar = inputFile.get();
                    while (!inputFile.eof())
                    {
                        if (currentChar == '\r' || currentChar == '\n')
                            break;
                        currentChar = inputFile.get();
                    }
                }
                else if (isalnum(currentChar) || currentChar == '_' || currentChar == '-')
                {
                    std::string Key;
                    Key.push_back(static_cast<char>(currentChar));
                    currentChar = inputFile.get();
                    while (!inputFile.eof() && currentChar != '=')
                    {
                        if (isalnum(currentChar) || currentChar == '_' || currentChar == '-')
                            Key.push_back(currentChar);
                        currentChar = inputFile.get();
                    }

                    std::string Value;
                    currentChar = inputFile.get();
                    while (!inputFile.eof() && currentChar != ';')
                    {
                        if (isprint(currentChar))
                            Value.push_back(currentChar);
                        currentChar = inputFile.get();
                    }
                    bool canAdd = true;
                    for (ConfigValue& currentRow : Config)
                    {
                        if (currentRow.Value == Key)
                        {
                            canAdd = false;
                            std::cerr 
                                << "LCF: Duplicate Rows: " 
                                << currentRow.Key << " => " << currentRow.Value
                                << "\nAnd (the one which will be discarded) " << Key << " => " << Value << '\n';
                            break;
                        }
                    }
                    if (canAdd)
                        Config.push_back((ConfigValue){Key, Value});
                    // Discard ';' / Go next
                    currentChar = inputFile.get();
                }
                // Discard if unrecognized, out of context char
                else currentChar = inputFile.get();
            }

            // Clean
            inputFile.close();
        }

        // Calls read
        public: LazyConfigIn(const std::string& configFileName)
        {
            resetExtract();
            readConfig(configFileName);
        }

        // Clear config and calls readFile
        public: void readConfig(const std::string& configFileName)
        {
            Config.clear();
            resetExtract();
            readFile(configFileName);
        }

        // Calls readFile
        public: void appendConfig(const std::string& configFileName)
        {
            readFile(configFileName);
        }

        // Get row of given key
        // If not found returns first row
        public: const ConfigValue& getRow(const std::string& keyName)
        {
            for (const ConfigValue& currentRow : Config)
            {
                if (currentRow.Key == keyName)
                    return currentRow;
            }
            std::cerr << "LCF: Key: \"" << keyName << "\" not found!\n";
            return Config[0];
        }

        // Get row of given key and gets the position
        // If not found returns -1
        public: std::size_t getPosition(const std::string& keyName)
        {
            for (std::size_t Index = 0; Index < Config.size(); Index++)
            {
                if (Config[Index].Key == keyName)
                    return Index;
            }
            std::cerr << "LCF: Key: \"" << keyName << "\" not found!\n";
            return -1;
        }

        // Return row at vector index
        public: ConfigValue& operator[](std::size_t Index) const
        {
            return const_cast<ConfigValue&>(Config[Index]);
        }

        // Returns wherever is there this keyname
        // If not found returns first row's value
        public: std::string& operator[](const std::string& keyName) const
        {
            for (unsigned int Index = 0; Index < Config.size(); Index++)
            {
                if (Config[Index].Key == keyName)
                    return const_cast<std::string&>(Config[Index].Value);
            }
            std::cerr << "LCF: Key: \"" << keyName << "\" not found!\n";
            return const_cast<std::string&>(Config[0].Value);
        }

        // Extract rows one by one
        public: LazyConfigIn& operator>>(ConfigValue& Value)
        {
            // If out of range
            if (extractCount >= Config.size())
            {
                std::cerr << "LCF: All rows already extracted\n";
                Value = (ConfigValue){"", ""};
                return (*this);
            }
            Value = (*this)[extractCount];
            extractCount++;
            return (*this);
        }

        // Return underlying vector of config values
        public: std::vector<ConfigValue>& getConfig()
        {
            return Config;
        }

        // Return true on success
        public: bool getInt(const std::string& keyName, int& Dest, int Min = std::numeric_limits<int>::lowest(), int Max = std::numeric_limits<int>::max())
        {
            if (!isInt((*this)[keyName]))
            {
                std::cerr << "LCF: Row: " << (*this)[keyName] << " isn't an integer.\n";
                return false;
            }   
            Dest = std::stoi((*this)[keyName]);
            if (Dest > Max)
                Dest = Max;
            else if (Dest < Min)
                Dest = Min;
            return true;
        }

        // Return true on success
        public: bool getuInt(const std::string& keyName, unsigned int& Dest, unsigned int Min = 0U, unsigned int Max = std::numeric_limits<unsigned int>::max())
        {
            if (!isuInt((*this)[keyName]))
            {
                std::cerr << "LCF: Row: " << (*this)[keyName] << " isn't an positive integer.\n";
                return false;
            }   
            Dest = std::stoul((*this)[keyName]);
            if (Dest > Max)
                Dest = Max;
            else if (Dest < Min)
                Dest = Min;
            return true;
        }

        // Return true on success
        public: bool getFloat(const std::string& keyName, float& Dest, float Min = std::numeric_limits<float>::lowest(), float Max = std::numeric_limits<float>::max())
        {
            if (!isFloat((*this)[keyName]))
            {
                std::cerr << "LCF: Row " << (*this)[keyName] << " isn't a floating point number.\n";
                return false;
            }   
            Dest = std::stof((*this)[keyName]);
            if (Dest > Max)
                Dest = Max;
            else if (Dest < Min)
                Dest = Min;
            return true;
        }

        // Return true on success
        public: bool getPath(const std::string& keyName, std::string& Dest)
        {
            if (!isFile((*this)[keyName]))
            {
                std::cerr << "LCF: Row " << (*this)[keyName] << " can't be accessed or isn't a file.\n";
                return false;
            }   
            Dest = (*this)[keyName];
            return true;
        }

        // Return true on success
        public: bool getBool(const std::string& keyName, bool& Dest)
        {
            if (!isBool((*this)[keyName]))
            {
                std::cerr << "LCF: Row " << (*this)[keyName] << " isn't a boolean value.\n";
                return false;
            }   
            if ((*this)[keyName] == "true" || (*this)[keyName] == "on" || (*this)[keyName] == "1" || 
                (*this)[keyName] == "TRUE" || (*this)[keyName] == "ON"
            )
                Dest = true;
            else Dest = false;
            return true;
        }

        public: bool isDefined(const std::string& keyName)
        {
            for (ConfigValue& currentRow : Config)
            {
                if (currentRow.Key == keyName)
                    return true;
            }
            return false;
        }
        
        // Returns the name of the last file read
        public: const std::string& getLastReadFileName()
        {
            return lastFileName;
        }

        // Resets the extract count to zero
        public: void resetExtract()
        {
            extractCount = 0;
        }
        
        // Does nothing
        public: ~LazyConfigIn()
        {
        }
    };

    class LazyConfigOut
    {
        // Config
        private: std::vector<ConfigValue> Config;

        // Last file wrote
        private: std::string lastFileName;

        // Does nothing
        public: LazyConfigOut()
        {
        }

        // Write key and value to config
        public: void writeValue(const std::string& Key, const std::string& Value)
        {
            Config.push_back((ConfigValue){Key, Value});
        }

        // Overload that take directly a ConfigValue
        public: void writeValue(const ConfigValue& Value)
        {
            Config.push_back(Value);
        }

        // Overloads

        public: void writeValue(const std::string& Key, int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }
        
        public: void writeValue(const std::string& Key, long int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, long long int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, unsigned int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, unsigned long int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, unsigned long long int Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, float Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, double Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        public: void writeValue(const std::string& Key, long double Value)
        {
            Config.push_back((ConfigValue){Key, std::to_string(Value)});
        }

        // Inserts to buffer
        public: LazyConfigOut& operator<<(const ConfigValue& V)
        {
            writeValue(V);
            return (*this);
        }
        
        // Appends to current config another config
        public: void appendConfig(std::vector<ConfigValue> configVector)
        {
            Config.insert(Config.end(), configVector.begin(), configVector.end());
        }

        // Returns config
        public: std::vector<ConfigValue>& getConfig()
        {
            return Config;
        }

        // Write config to file
        public: void saveConfig(std::string outputFileName)
        {
            lastFileName = outputFileName;
            std::ofstream outputFile = std::ofstream(outputFileName, std::ios::out);
            outputFile << "# Lazy Config File \"LCF\" or \"ETPCF\" (Easy-To-Parse-Configuration-File)\n";
            outputFile << "# Key must be alphanumeric, '_' or '-'.\n";
            outputFile << "# Value must contain characters that return true on isprint() C function.\n";
            outputFile << "# Syntax is simple: Key=Value;\n";
            outputFile 
                << "# Generated using LCF " 
                << LAZYCONFIGFILE_MAJOR << '.' 
                << LAZYCONFIGFILE_MINOR << '.' 
                << LAZYCONFIGFILE_PATCH << '\n';
            for (ConfigValue& currentValue : Config)
                outputFile << currentValue.Key << '=' << currentValue.Value << ";\n";
            outputFile.flush();
            outputFile.close();
        }

        // Clear config
        public: void clearConfig()
        {
            Config.clear();
        }
        
        // Close file
        public: ~LazyConfigOut()
        {
        }
    };
}
#endif /* LAZYCONFIGFILEH_HPP */