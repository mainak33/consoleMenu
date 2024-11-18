/*********************************************************************
 * @file  osName.h
 *
 * @brief Class OS to determine for OS Name
 * 
 *********************************************************************/

#pragma once

#include<unordered_set>

namespace osUtils{
    using std::unordered_set;
}

namespace osUtils{
    /**
    * Singleton Class containing info about the OS this code is compiled on
    */
    
    class OS{
        public :
            
            enum class NAME{ //!< All the feasible OS (Platform) Names
                WINDOWS,
                WINDOWS32,
                WINDOWS64,
                LINUX,
                UNIX,
                APPLE,
                IOS,
                OSX,
                ANDROID,
                BSD,
                HP_UX,
                IBM_AIX,
                SOLARIS
            };

            /**
            * @brief returns an instance of OS singleton
            *
            * @return an reference to a static OS object created by the private constructor 
            */
            static OS& instance();

            /**
            * @brief returns if this OS matches the queried Platform Name
            *
            * @param name a valid OS (Platform) Name (as defined by OS::NAME) 
            * @return true if name matches a desciption of the OS this code was compiled on; false otherwise
            */
            static bool is(OS::NAME name);

            // Delete copy and move constructors and operators
            OS(OS const&) = delete;           
            OS(OS&&) = delete;                
            OS& operator=(OS const&) = delete;
            OS& operator=(OS&&) = delete;     
        
        private :

            static unordered_set<OS::NAME> platformNames; //!<  OS (Platform) Names associated with the OS this code is compiled on 

        protected:

            OS();

            ~OS();
    };

    using OSNameSet = unordered_set<OS::NAME>;
}
