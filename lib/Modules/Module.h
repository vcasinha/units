#ifndef MODULE_H
#define MODULE_H

    #include <Arduino.h>
    #include <ArduinoJson.h>
    #include "Application.h"
    class Application;
    class Module
    {
        public:
            const char * _name = "unnamed";
            unsigned long _loop_time = 0;
            unsigned long _loop_period_ms = 0;
            bool _enabled = true;

            Module()
            {
            }

            void setApplication(Application * application)
            {
                this->_application = application;
            }
            
            virtual void boot(JsonObject &) {};
            virtual void setup() {};
            virtual void loop() {};
            virtual void callback(char * topic, unsigned char * payload, unsigned int length) {};
            void disable(void) { this->_enabled = false; }
            void enable(void) { this->_enabled = true; }
            
        protected:
            Application * _application;
    };

#endif