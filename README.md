# Mosquitto Payload Logging Plugin

This plugin for Mosquitto demonstrates how to log the payload of messages received by the broker. It logs the payload along with the current date and time into CSV files specific to each client.

## Prerequisites

Before compiling and using this plugin, ensure you have the following:

- Mosquitto source code downloaded. This plugin works with Mosquitto version 2.0 or later.
- Basic understanding of compiling and configuring Mosquitto plugins.
- Development tools including GCC and Make.

## Installation

1. **Include `config.mk`:**
   - Locate the `config.mk` file in the Mosquitto source code.
   - Include `config.mk` in the plugin's `Makefile`.

2. **Compile the Plugin:**
   ```bash
   make

3. **Move the Shared Object File:**
 Move the compiled `mosquitto_payload_logging.so` file to the Mosquitto plugins directory _(e.g., /usr/local/mosquitto/plugins/)_.

4. **Update mosquitto.conf:**
 Add the following line to your mosquitto.conf file _(typically found in /etc/mosquitto/)_:
      `plugin /path/to/mosquitto_payload_logging.so`

5. **Restart Mosquitto:**
 Restart the Mosquitto broker for changes to take effect.
    ```
    Sudo systemctl restart mosquitto.service
    ```

## Usage
Once installed and configured, this plugin will log message payloads to CSV files located in `/var/log/mosquitto/payloadLogs`.

## CSV Log Format
 The plugin logs each message payload in the following CSV format:
 
    
    Payload,Date,Time
    message_payload,YYYY-MM-DD,HH:MM:SS
    
  - Payload: Contains the payload of the MQTT message.
  - Date: Represents the date when the message was received.
  - Time: Represents the time of day when the message was received.
You can modify the CSV format by editing the callback_message function in `mosquitto_payload_logging.c`.

## Important Notes
  - Ensure that the directory `/var/log/mosquitto/payloadLogs` exists and is writable by the Mosquitto process.
  - This plugin assumes that Mosquitto is running with sufficient permissions to create directories and write files.

## License
  - This plugin is licensed under the MIT License. See LICENSE for details.

## Author
  - Jay
  - Email: jagmohankumar5246@gmail.com

### Explanation:

- **Project Overview**: Provides a brief description of what the plugin does.
- **Prerequisites**: Lists the requirements for using the plugin.
- **Installation Steps**: Detailed steps to download, compile, install, and configure the plugin.
- **Usage**: Describes the CSV format used for logging message payloads.
- **Important Notes**: Highlights important considerations for using the plugin.
- **License**: Mentions the licensing information.
- **Author**: Provides contact information for the author.

Make sure to adjust paths and details according to your specific setup and preferences.
