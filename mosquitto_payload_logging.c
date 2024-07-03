/*
 * Copyright (c) 2024 jay <jagmohankumar5246@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * This is an *example* plugin which demonstrates how to log the payload of
 * a message after it is received by the broker and before it is sent on to
 * other clients.
 *
 * You should be very sure of what you are doing before making use of this feature.
 *
 * Compile with:
 *   make
 *
 * Use in config with:
 *
 *   plugin /path/to/mosquitto_payload_logging.so
 *
 * Note that this only works on Mosquitto 2.0 or later.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "mosquitto_broker.h"
#include "mosquitto_plugin.h"
#include "mosquitto.h"
#include "mqtt_protocol.h"

#define _GNU_SOURCE

#define UNUSED(A) (void)(A)

static mosquitto_plugin_id_t *mosq_pid = NULL;
static const char* root_folder = "/var/log/mosquitto/payloadLogs";

/*
 * Generates the file path for storing payload logs for a specific client.
 * The file path is constructed based on the root folder and client ID.
 */
static char *get_file_path(const char *client_id)
{
    char *file_path = NULL;
    size_t len = (size_t) snprintf(NULL, 0, "%s/%s/payload.csv", root_folder, client_id);
    if (len > 0) {
        file_path = (char *)malloc(sizeof(char) * (len + 1));
        if (file_path != NULL) {
            snprintf(file_path, len + 1, "%s/%s/payload.csv", root_folder, client_id);
        }
    }
    return file_path;
}

/*
 * Callback function that handles logging the message payloads.
 * It logs the payload, current date, and time to a CSV file.
 */
static int callback_message(int event, void *event_data, void *userdata)
{
    struct mosquitto_evt_message *ed = event_data;
    const char *client_id = mosquitto_client_id(ed->client);
    char *payload = ed->payload;
    uint32_t payloadlen = ed->payloadlen;
    struct timespec ts;
    struct tm *ti;
    char date_str[20];
    char time_str[20];
    char *file_path = NULL;
    FILE *file = NULL;
    int ret = MOSQ_ERR_SUCCESS;

    UNUSED(event);
    UNUSED(userdata);

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        return MOSQ_ERR_UNKNOWN;
    }

    ti = localtime(&ts.tv_sec);
    if (ti == NULL) {
        return MOSQ_ERR_UNKNOWN;
    }

    if (strftime(date_str, sizeof(date_str), "%Y-%m-%d", ti) == 0) {
        return MOSQ_ERR_UNKNOWN;
    }

    if (strftime(time_str, sizeof(time_str), "%T", ti) == 0) {
        return MOSQ_ERR_UNKNOWN;
    }

    char client_folder[256];
    snprintf(client_folder, sizeof(client_folder), "%s/%s", root_folder, client_id);
    struct stat st = {0};
    if (stat(root_folder, &st) == -1) {
        if (mkdir(root_folder, 0755) == -1) {
            return MOSQ_ERR_UNKNOWN;
        }
    }

    if (stat(client_folder, &st) == -1) {
        if (mkdir(client_folder, 0755) == -1) {
            return MOSQ_ERR_UNKNOWN;
        }
    }

    file_path = get_file_path(client_id);
    if (file_path == NULL) {
        return MOSQ_ERR_NOMEM;
    }

    if (access(file_path, F_OK) == -1) {
        file = fopen(file_path, "w");
        if (file == NULL) {
            free(file_path);
            return MOSQ_ERR_UNKNOWN;
        }
        if (fwrite("Payload,Date,time\n", 18, 1, file) != 1) {
            fclose(file);
            free(file_path);
            return MOSQ_ERR_UNKNOWN;
        }
        fclose(file);
    }

    file = fopen(file_path, "a");
    if (file == NULL) {
        free(file_path);
        return MOSQ_ERR_UNKNOWN;
    }

    if (fwrite(payload, payloadlen, 1, file) != 1 ||
        fwrite(",", 1, 1, file) != 1 ||
        fwrite(date_str, strlen(date_str), 1, file) != 1 ||
        fwrite(",", 1, 1, file) != 1 ||
        fwrite(time_str, strlen(time_str), 1, file) != 1 ||
        fwrite("\n", 1, 1, file) != 1) {
        ret = MOSQ_ERR_UNKNOWN;
    }

    fclose(file);
    free(file_path);
    return ret;
}

/*
 * Function to return the plugin version supported.
 * Mosquitto plugin version 5 is returned if supported.
 */
int mosquitto_plugin_version(int supported_version_count, const int *supported_versions)
{
    for (int i = 0; i < supported_version_count; i++) {
        if (supported_versions[i] == 5) {
            return 5;
        }
    }
    return -1;
}

/*
 * Plugin initialization function.
 * Registers the callback_message function with Mosquitto.
 */
int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count)
{
    UNUSED(user_data);
    UNUSED(opts);
    UNUSED(opt_count);

    mosq_pid = identifier;
    return mosquitto_callback_register(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL, NULL);
}

/*
 * Plugin cleanup function.
 * Unregisters the callback_message function from Mosquitto.
 */
int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count)
{
    UNUSED(user_data);
    UNUSED(opts);
    UNUSED(opt_count);

    return mosquitto_callback_unregister(mosq_pid, MOSQ_EVT_MESSAGE, callback_message, NULL);
}
