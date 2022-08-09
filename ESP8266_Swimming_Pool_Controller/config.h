#pragma once

//Project Info
#define NAME "Swimming Pool Controller"
#define VERSION "Version 0.1.0 dev"

//Serial Info
#define ENABLE_DEBUG
#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200

//Functions
#define RESET_BY_BUTTON true
#define RESET_BY_BUTTON_GPIO 14

// WiFi
#define WIFI_MODE 2

// WiFi Access Point Info
#define AP_SSID "Swimming Pool Controller"
#define AP_PASSWD ""
#define AP_HIDDEN false
#define AP_IP_ADDR {5,5,5,5}
#define AP_DNS_TTL 60
#define AP_DNS_PORT 53
#define AP_DNS_DOMAIN "*"

// WiFi Station Info
#define ST_SSID ""
#define ST_PASSWD ""
#define ST_STATIC false
#define ST_IP_ADDR {0,0,0,0}
#define ST_IP_SUBNET {0,0,0,0}
#define ST_IP_GATEWAY {0,0,0,0}

// OTA Info
#define OTA_SSID NAME
#define OTA_PASSWD "root"
#define OTA_PORT 8266

//SPIFFS Editor info
#define SPIFFS_SSID "root"
#define SPIFFS_PASSWD "root"

// EEPROM
#define EEPROM_SIZE       FLASH_SECTOR_SIZE
#define EEPROM_BOOT_ADDR  (FLASH_SECTOR_SIZE - 1 - sizeof(storage_t))
#define BOOT_MAGIC_NUM    2137001337

//Project config
#define DS18B20_PIN 5
#define STATE_PIN 4
#define DS18B20Time 1e3

