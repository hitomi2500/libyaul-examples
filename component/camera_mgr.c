/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "../blue.h"

void
component_camera_mgr_on_init(struct component *this __unused)
{
}

void
component_camera_mgr_on_update(struct component *this __unused)
{
        cons_buffer("Hello from component camera_mgr\n");

        struct transform *transform;
        transform = (struct transform *)object_component_find(
                THIS(camera_mgr, object), COMPONENT_ID_TRANSFORM);

        COMPONENT(transform, position).x = fix16_add(COMPONENT(transform, position).x, F16(0.33f));
}

void
component_camera_mgr_on_draw(struct component *this __unused)
{
}

void
component_camera_mgr_on_destroy(struct component *this __unused)
{
}
