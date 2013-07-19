/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <dlfcn.h>

#define LOG_TAG "Qualcomm PowerHAL"
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>
#define TOUCHBOOST_SOCKET       "/dev/socket/mpdecision/touchboost"

static void power_init(struct power_module *module)
{
}

static void power_set_interactive(struct power_module *module, int on)
{
}

static void touch_boost()
{
    static int client_comsoc = -1;
    static struct sockaddr_un client_addr;
    int rc = 0;

    client_comsoc = socket(PF_UNIX, SOCK_DGRAM, 0);

    if (client_comsoc < 0) {
        return;
    }

    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    snprintf(client_addr.sun_path, UNIX_PATH_MAX, TOUCHBOOST_SOCKET);

    rc = sendto(client_comsoc, "1", 1, 0, (const struct sockaddr *)&client_addr, sizeof(struct sockaddr_un));

    if (rc == -1) {
        ALOGE("Failed to send rc=%d", rc);
    }

    if (client_comsoc >= 0) {
        close(client_comsoc);
        client_comsoc = -1;
    }
}

static void power_hint(struct power_module *module, power_hint_t hint,
                       void *data) {
    switch (hint) {

        case POWER_HINT_INTERACTION:
            touch_boost();
	    break;
        default:
             break;
    }
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct power_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = POWER_MODULE_API_VERSION_0_2,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = POWER_HARDWARE_MODULE_ID,
        .name = "Qualcomm Power HAL",
        .author = "The Android Open Source Project",
        .methods = &power_module_methods,
    },

    .init = power_init,
    .setInteractive = power_set_interactive,
    .powerHint = power_hint,
};
