#pragma once

#ifndef LAZYCONFIGFILE_HPP
#define LAZYCONFIGFILE_HPP

#include <iostream>
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

    bool isInteger(std::string stringToCheck);

    class LazyConfigIn
    {
        // Input read config
        private: std::vector<LCF::configValue> inConfiguration;

        // Input file we're reading from
        private: std::ifstream inputFile;

        // Overloaded Operator >> count
        private: std::size_t extractCount;

        // Read
        private: void getConfig(std::string inputFileName);

        // Calls read
        public: LazyConfigIn(std::string inputFileName);

        // Clear buffer and calls getconfig
        public: void readConfiguration(std::string inputFileName);

        // Only call getconfig
        public: void concatenateConfiguration(std::string inputFileName);

        // Get row of given key
        public: LCF::configValue getWhere(std::string keyName);

        // Get row of given key and set as reference the position in the vector of the row
        public: LCF::configValue getWhere(std::string keyName, std::size_t& Position);

        // Return row at vector index
        public: const LCF::configValue& operator[](std::size_t Index) const; 

        // Returns wherever is there this keyname
        public: const std::string& operator[](std::string keyName) const; 

        // Extract rows one by one
        public: LazyConfigIn& operator>>(LCF::configValue& V);

        // Return undelying vector of config values
        public: const std::vector<LCF::configValue>& getConfiguration() const;
        
        // Close file
        public: ~LazyConfigIn();
    };

    class LazyConfigOut
    {
        // Buffered rows
        private: std::vector<LCF::configValue> outConfiguration;

        // File to write to
        private: std::ofstream outputFile;

        // Calls create config file
        public: LazyConfigOut(std::string inputFileName);

        // Creates configuration file
        public: void createConfiguration(std::string outputFileName);

        // Write key and value to buffer
        public: void writeValue(std::string Key, std::string Value);

        // Overload that take directly a configvalue
        public: void writeValue(LCF::configValue Value);

        // Overload that take directly a integer as value
        public: void writeValue(std::string Key, long long int Value);

        // Inserts to buffer
        public: LazyConfigOut& operator<<(LCF::configValue V);
        
        // Appends to current buffered config list another config list
        public: void appendConfiguration(std::vector<LCF::configValue> configList);

        // Returns buffered rows
        public: const std::vector<LCF::configValue>& getConfiguration() const;

        // Write buffered rows
        public: void writeBuffered();

        // Clear buffered rows
        public: void clearBuffered();
        
        // Close file
        public: ~LazyConfigOut();
    };
}
#endif /* LAZYCONFIGFILE_HPP */