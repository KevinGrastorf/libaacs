/*
 * This file is part of libaacs
 * Copyright (C) 2009-2010  Obliter0n
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "configfile.h"
#include "file.h"
#include "util/macro.h"
#include "util/logging.h"

#include <stdio.h>

uint8_t *configfile_record(CONFIGFILE *kf, enum configfile_types type,
                           uint16_t *entries, size_t *entry_len)
{
    size_t pos = 0, len = 0;

    if (entries) {
        *entries = 0;
    }

    while (pos + 4 <= kf->size) {
        len = MKINT_BE24(kf->buf + pos + 1);

        if (entries) {
            *entries = MKINT_BE16(kf->buf + pos + 4);
        }

        if (entry_len) {
            *entry_len = MKINT_BE32(kf->buf + pos + 6);
        }

        if (kf->buf[pos] == type) {
            DEBUG(DBG_CONFIGFILE, "Retrieved CONFIGFILE record 0x%02x (%p)\n",
                  type, kf->buf + pos + 10);

            // only return ptr to first byte of entry
            return kf->buf + pos + 10;
        }

        pos += len;
    }

    return NULL;
}

CONFIGFILE *configfile_open(const char *path)
{
    FILE_H *fp = NULL;
    CONFIGFILE *kf = malloc(sizeof(CONFIGFILE));

    DEBUG(DBG_CONFIGFILE, "Opening configfile %s... (%p)\n", path, kf);

    if ((fp = file_open(path, "rb"))) {
        file_seek(fp, 0, SEEK_END);
        kf->size = file_tell(fp);
        file_seek(fp, 0, SEEK_SET);

        kf->buf = malloc(kf->size);

        file_read(fp, kf->buf, kf->size);

        file_close(fp);

        return kf;
    }

    return NULL;
}

void configfile_close(CONFIGFILE *kf)
{
    DEBUG(DBG_CONFIGFILE, "configfile closed (%p)\n", kf);

    X_FREE(kf->buf);
    X_FREE(kf);
}
