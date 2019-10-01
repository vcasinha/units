#include <UnitManagerModule.h>

Vector<UnitFactory *> UnitManagerModule::_factories = Vector<UnitFactory *>();

UnitManagerModule::UnitManagerModule()
{
    this->init("unit_manager", 100);
}

void UnitManagerModule::makeUnit(JsonObject config)
{
    String unit_type = config["type"].as<String>();
    //String json_config = "";
    //serializeJson(config, json_config);
    //Log.notice("(unitManager.makeDevice) Init '%s' device (%s)", unit_type.c_str(), json_config.c_str());

    UnitFactory *factory = UnitManagerModule::getFactory(unit_type);
    if(factory)
    {
        Unit *unit = factory->make();
        unit->boot(this->_application, config);
        this->_units.push(unit);
    }
    else
    {
        Log.error("(unitManager.makeDevice) ERROR Unknown device type '%s'.", unit_type.c_str());
    }
}

void UnitManagerModule::config(JsonObject &config)
{
    Log.notice("(UnitManager.config) Set units configuration");
    serializeJson(config, Serial);
    if (config.containsKey("units"))
    {
        for (unsigned int i = 0; i < this->_units.size(); i++)
        {
            Unit *unit = this->_units[i];
            if (config["units"][unit->_id])
            {
                JsonObject unit_config = config["units"][unit->_id];
                String json_config = "";
                serializeJson(unit_config, json_config);
                Log.notice("(unitsManager.config) Update unit configuration '%s' ()", unit->_id.c_str(), json_config.c_str());
                this->_units[i]->unitConfig(unit_config);
            }
        }
    }
}

void UnitManagerModule::boot(JsonObject & config)
{
    if (config.size() == 0)
    {
        Log.warning("(MQTT) Empty configuration, disabling Device Manager");
        this->disable();
        return;
    }

    this->_mqtt = (MQTTModule *)this->_application->getModule("mqtt");
    this->_mqtt->registerCallback(this);

    this->_time = (TimeModule *)this->_application->getModule("time");

    if (config.containsKey("units"))
    {
        JsonObject root = config["units"].as<JsonObject>();
        Log.notice("(unitsManager.boot) Booting %d units", config["units"].size());
        for (JsonObject::iterator it = root.begin(); it != root.end(); ++it)
        {
            //config.prettyPrintTo(Serial);
            this->makeUnit(it->value().as<JsonObject>());
        }
    }
}

void UnitManagerModule::setup(void)
{
    //Log.notice("Setup devices");
    for (unsigned int i = 0; i < this->_units.size(); i++)
    {
        Unit *unit = this->_units[i];
        unit->ready();
    }
}

void UnitManagerModule::callback(char *topic, unsigned char *payload, unsigned int length)
{
    String topic_string = topic;
    payload[length] = '\0';
    String data = (char *)payload;
    data.trim();

    Log.trace("(unitsManager.callback) Unit callback on topic '%s' Data: %s", topic_string.c_str(), data.c_str());
    for (unsigned int i = 0; i < this->_units.size(); i++)
    {
        this->_units[i]->callback(topic_string, data);
    }
}
