/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "gfps.h"
#if GFPS_PERSONALIZED_NAME_SUPPORT
#include "trace.h"
#include "ftl.h"
#include "app_relay.h"
#include "errno.h"
#include "stdlib.h"
#include "app_gfps_sync.h"
#include "app_gfps_personalized_name.h"

typedef struct
{
    uint8_t is_valid;
    uint8_t stored_personalized_name_len;
    uint8_t reserved[2];
    uint8_t stored_personalized_name[GFPS_PERSONALIZED_NAME_MAX_LEN];
} T_PERSONALIZED_NAME;

T_PERSONALIZED_NAME *p_personalized_name;

void app_gfps_personalized_name_set_default(void)
{
    p_personalized_name->is_valid = false;
    p_personalized_name->stored_personalized_name_len = 0;

    ftl_save_to_storage(p_personalized_name, PERSONALIZED_NAME_FLASH_OFFSET,
                        sizeof(T_PERSONALIZED_NAME));
}

void app_gfps_personalized_name_init(void)
{
    p_personalized_name = calloc(1, sizeof(T_PERSONALIZED_NAME));

    if (p_personalized_name)
    {
        uint32_t read_result = ftl_load_from_storage(p_personalized_name, PERSONALIZED_NAME_FLASH_OFFSET,
                                                     sizeof(T_PERSONALIZED_NAME));

        if (read_result == ENOF || p_personalized_name->is_valid == false)
        {
            APP_PRINT_INFO0("app_gfps_personalized_name_init: initialize");
            app_gfps_personalized_name_set_default();
        }
        else
        {
            APP_PRINT_TRACE3("app_gfps_personalized_name_init: personalized_name in ftl %b, is_valid %d, stored_personalized_name_len %d",
                             TRACE_BINARY(p_personalized_name->stored_personalized_name_len,
                                          p_personalized_name->stored_personalized_name),
                             p_personalized_name->is_valid, p_personalized_name->stored_personalized_name_len);
        }
    }
}

/*personalzied name store */
T_GFPS_PERSON_NAME_RST app_gfps_personalized_name_store(uint8_t *stored_personalized_name,
                                                        uint8_t stored_personalized_name_len)
{
    T_GFPS_PERSON_NAME_RST result = APP_GFPS_PERSONALIZED_NAME_SUCCESS;

    if (stored_personalized_name == NULL || stored_personalized_name_len == 0)
    {
        result = APP_GFPS_PERSONALIZED_NAME_POINTER_NULL;
        return result;
    }

    if (p_personalized_name->is_valid &&
        (stored_personalized_name_len == p_personalized_name->stored_personalized_name_len) &&
        memcmp(p_personalized_name->stored_personalized_name, stored_personalized_name,
               p_personalized_name->stored_personalized_name_len) == 0)
    {
        APP_PRINT_INFO0("app_gfps_personalized_name_store: this name has already stored in ftl");
    }
    else
    {
        if (stored_personalized_name_len > GFPS_PERSONALIZED_NAME_MAX_LEN)
        {
            p_personalized_name->stored_personalized_name_len = GFPS_PERSONALIZED_NAME_MAX_LEN;
        }
        else
        {
            p_personalized_name->stored_personalized_name_len = stored_personalized_name_len;
        }

        memcpy(p_personalized_name->stored_personalized_name, stored_personalized_name,
               p_personalized_name->stored_personalized_name_len);

        uint8_t name_len = p_personalized_name->stored_personalized_name_len;
        p_personalized_name->stored_personalized_name[name_len] = '\0';

        if (!p_personalized_name->is_valid)
        {
            p_personalized_name->is_valid = true;
        }

        if (ftl_save_to_storage(p_personalized_name, PERSONALIZED_NAME_FLASH_OFFSET,
                                sizeof(T_PERSONALIZED_NAME)))
        {
            result  = APP_GFPS_PERSONALIZED_NAME_SAVE_FAIL;
            return result;
        };
    }

    APP_PRINT_TRACE3("app_gfps_personalized_name_store: new personalized_name in ftl %s, stored_personalized_name_len %d, is_valid %d",
                     TRACE_STRING(p_personalized_name->stored_personalized_name),
                     p_personalized_name->stored_personalized_name_len,
                     p_personalized_name->is_valid);
    return result;
}

T_GFPS_PERSON_NAME_RST app_gfps_personalized_name_read(uint8_t *stored_personalized_name,
                                                       uint8_t *stored_personalized_name_len)
{
    T_GFPS_PERSON_NAME_RST result = APP_GFPS_PERSONALIZED_NAME_SUCCESS;

    if (stored_personalized_name == NULL || stored_personalized_name_len == NULL)
    {
        result = APP_GFPS_PERSONALIZED_NAME_POINTER_NULL;
        return result;
    }

    if (p_personalized_name->is_valid && p_personalized_name->stored_personalized_name_len != 0)
    {
        *stored_personalized_name_len = p_personalized_name->stored_personalized_name_len;
        memcpy(stored_personalized_name, p_personalized_name->stored_personalized_name,
               *stored_personalized_name_len);

        stored_personalized_name[*stored_personalized_name_len] = '\0';
        APP_PRINT_TRACE3("app_gfps_personalized_name_read: personalized_name in ftl %s, stored_personalized_name_len %d, is_valid %d",
                         TRACE_STRING(stored_personalized_name),
                         p_personalized_name->stored_personalized_name_len, p_personalized_name->is_valid);
    }
    else
    {
        result = APP_GFPS_PERSONALIZED_NAME_INVALID;
        return result;
    }

    return result;
}

void app_gfps_personalized_name_send(uint8_t conn_id, uint8_t service_id)
{
    T_GFPS_PERSON_NAME_RST result;
    bool is_send = false;

    uint8_t  stored_personalized_name[GFPS_PERSONALIZED_NAME_MAX_LEN] = {0};
    uint8_t stored_personalized_name_len = 0;

    result = app_gfps_personalized_name_read(stored_personalized_name, &stored_personalized_name_len);

    if (result == APP_GFPS_PERSONALIZED_NAME_SUCCESS)
    {
        is_send = gfps_send_personalized_name(stored_personalized_name, stored_personalized_name_len,
                                              conn_id, service_id);
    }
    else
    {
        is_send = gfps_send_personalized_name(NULL, 0, conn_id, service_id);
    }

    APP_PRINT_INFO1(" app_gfps_personalized_name_send: is_send =  %d", is_send);
}

void app_gfps_personalized_name_clear(void)
{
    app_gfps_personalized_name_set_default();
}

bool app_gfps_personalized_name_remote_sync(void)
{
    uint8_t name[GFPS_PERSONALIZED_NAME_MAX_LEN];
    uint8_t name_len;

    if (app_gfps_personalized_name_read(name, &name_len) == APP_GFPS_PERSONALIZED_NAME_SUCCESS)
    {
        APP_PRINT_INFO0("app_gfps_personalized_name_remote_sync sync");
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS,
                                            APP_REMOTE_MSG_GFPS_PERSONALIZED_NAME,
                                            name, name_len);
        return true;
    }
    return false;
}

#endif
