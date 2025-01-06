#pragma once
#include <mutex>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <random>
#include <concepts>

#define LOG_DEFAULT_POLICY true;

namespace Utils {
    class Debug {
    private:
        std::mutex mutex_;
        Debug(const Debug&) = delete;
        Debug& operator=(const Debug&) = delete;
        std::map<std::string, bool> prefixPolicies;
        std::map<std::string, int> prefixColors;
        static const bool defaultPolicy = LOG_DEFAULT_POLICY;
        int largestPrefix = 2;

        // Colors & rnd stuff
        std::vector<std::string> colors = {
            "\033[0;31m", "\033[0;32m", "\033[0;33m", "\033[0;34m", "\033[0;35m", "\033[0;36m",
            "\033[0;91m", "\033[0;92m", "\033[0;93m", "\033[0;94m", "\033[0;95m", "\033[0;96m"
        };
        std::random_device rd;
        std::mt19937 gen;

        int getRandomColor() {
            std::uniform_int_distribution<> dis(0, colors.size() - 1);
            return dis(gen);
        }

        bool checkWildcardPolicy(const std::string& prefix) {
            for (const auto& policy : prefixPolicies) {
                if (policy.first.back() == '*') {
                    if (prefix.compare(0, policy.first.length() - 1, policy.first, 0, policy.first.length() - 1) == 0) {
                        return policy.second;
                    }
                }
            }
            return defaultPolicy;
        }

    public:

        Debug() : gen(rd()) {}

        static Debug& getInstance() {
            static Debug instance;
            return instance;
        }

        static void Log(const std::string& log, const std::string& prefix) {
            auto& _debug = Debug::getInstance();
            bool locked = false;
            // Unknown prefix ? Add it to the policies and assign a color
            if(_debug.prefixPolicies.find(prefix) == _debug.prefixPolicies.end()) {
                bool policy = _debug.checkWildcardPolicy(prefix);
                std::lock_guard<std::mutex> lock(_debug.mutex_);
                locked = true;
                _debug.prefixPolicies[prefix] = policy;
                _debug.prefixColors[prefix] = _debug.getRandomColor();
                if(prefix.length() > _debug.largestPrefix)
                    _debug.largestPrefix = prefix.length();
            }

            if(_debug.prefixPolicies[prefix]) {
                if(!locked)
                    std::lock_guard<std::mutex> lock(_debug.mutex_);
                std::cout << _debug.colors[_debug.prefixColors[prefix]] 
                          << "\e[1m" // bold
                          << "[" << prefix << "]" 
                          << "\e[0m" // non bold
                          << _debug.colors[_debug.prefixColors[prefix]] 
                          << separator(_debug.largestPrefix, prefix.length()) 
                          << log
                          << "\033[0m" // Reset color
                          << std::endl;
            }
        }

        static std::string separator(int len, int skip){
            std::string sep("");
            for(int i = skip; i < len+2; i ++)
                sep += " ";
            return sep;
        }

        static void addShow(std::string prefix){
            auto& _debug = Debug::getInstance();
            std::lock_guard<std::mutex> lock(_debug.mutex_);
            // Wildcard ? 
            if(prefix[prefix.length()-1] == '*'){
                for(auto& it : _debug.prefixPolicies){
                    if(it.first.substr(0, prefix.length()-1) == prefix.substr(0, prefix.length()-1)){
                        std::cout << "Showing " << it.first << std::endl;
                        it.second = true;
                    }
                }
            }
            _debug.prefixPolicies[prefix] = true;
        }

        static void addIgnore(std::string prefix){
            auto& _debug = Debug::getInstance();
            std::lock_guard<std::mutex> lock(_debug.mutex_);
            // Wildcard ? 
            if(prefix[prefix.length()-1] == '*'){
                for(auto& it : _debug.prefixPolicies){
                    if(it.first.substr(0, prefix.length()-1) == prefix.substr(0, prefix.length()-1)){
                        std::cout << "Ignoring " << it.first << std::endl;
                        it.second = false;
                    }
                }
            }
            _debug.prefixPolicies[prefix] = false;
        }

        static void displayPolicies(){
            auto& _debug = Debug::getInstance();

            for(auto it : _debug.prefixPolicies){
                std::cout << "  " << it.first << " : " << (it.second ? "true" : "false") << std::endl;
            }
        }
    };
}