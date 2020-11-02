/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <sega3d.h>

#include <assert.h>
#include <stdlib.h>

#define RESOLUTION_WIDTH    (320)
#define RESOLUTION_HEIGHT   (224)

#define SCREEN_WIDTH    (320)
#define SCREEN_HEIGHT   (224)

#define VDP1_VRAM_CMDT_COUNT    (8192)
#define VDP1_VRAM_TEXTURE_SIZE  (0x3BFE0)
#define VDP1_VRAM_GOURAUD_COUNT (1024)
#define VDP1_VRAM_CLUT_COUNT    (256)

#define ORDER_SYSTEM_CLIP_COORDS_INDEX  (0)
#define ORDER_LOCAL_COORDS_INDEX        (1)
#define ORDER_SEGA3D_INDEX              (2)
#define ORDER_BASE_COUNT                (3)

extern PDATA PD_PLANE1[];
extern PDATA PD_CUBE1[];
extern PDATA PD_SONIC[];
extern PDATA PD_QUAKE[];

extern Uint16 GR_SMS[];
extern PDATA PD_SMS3[];

static void _vblank_out_handler(void *);

static smpc_peripheral_digital_t _digital;

int
main(void)
{
        sega3d_init();

        uint16_t *gouraud_tbl;
        gouraud_tbl = (uint16_t *)VDP1_VRAM(0x2BFE0);

        PDATA *pdata;
        pdata = PD_SMS3;

        uint16_t polygon_count;
        polygon_count = sega3d_polycount_get(pdata);

        uint16_t cmdt_list_count;
        cmdt_list_count = ORDER_BASE_COUNT + polygon_count;

        vdp1_cmdt_list_t *cmdt_list;
        cmdt_list = vdp1_cmdt_list_alloc(cmdt_list_count);
        assert(cmdt_list != NULL);

        /* Set up the first few command tables */
        vdp1_env_preamble_populate(&cmdt_list->cmdts[0], NULL);

        sega3d_cmdt_prepare(pdata, cmdt_list, ORDER_SEGA3D_INDEX);

        /* Be sure to terminate list */
        vdp1_cmdt_end_set(&cmdt_list->cmdts[cmdt_list_count - 1]);

        /* Set the number of command tables to draw from the list */
        cmdt_list->count = cmdt_list_count;

        (void)memcpy(gouraud_tbl, GR_SMS, sizeof(vdp1_gouraud_table_t) * polygon_count); 

        MATRIX matrix;
        matrix[0][0] = toFIXED(0.5000000); matrix[0][1] = toFIXED(-0.5000000); matrix[0][2] = toFIXED( 0.7071068);
        matrix[1][0] = toFIXED(0.8535534); matrix[1][1] = toFIXED( 0.1464466); matrix[1][2] = toFIXED(-0.5000000);
        matrix[2][0] = toFIXED(0.1464466); matrix[2][1] = toFIXED( 0.8535534); matrix[2][2] = toFIXED( 0.5000000);
        sega3d_matrix_load(&matrix);

        FIXED z;
        z = toFIXED(0.0f);

        while (true) {
                smpc_peripheral_process();
                smpc_peripheral_digital_port(1, &_digital);

                dbgio_printf("[H[2J");
                dbgio_printf("z: %li\n", z);

                sega3d_matrix_push(MATRIX_TYPE_PUSH); {
                        sega3d_matrix_translate(toFIXED(0.0f), toFIXED(0.0f), z);
                        /* sega3d_matrix_scale(toFIXED(20.0f), toFIXED(20.0f), toFIXED(20.0f)); */
                        sega3d_cmdt_transform(pdata);
                } sega3d_matrix_pop();

                if ((_digital.pressed.raw & PERIPHERAL_DIGITAL_UP) != 0) {
                        z += toFIXED(-1.0f);
                } else if ((_digital.pressed.raw & PERIPHERAL_DIGITAL_DOWN) != 0) {
                        z += toFIXED( 1.0f);
                }

                vdp1_sync_cmdt_list_put(cmdt_list, NULL, NULL);

                dbgio_flush();
                vdp_sync();
        }
}

void
user_init(void)
{
        vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
            VDP2_TVMD_VERT_224);

        vdp2_scrn_back_screen_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
            COLOR_RGB1555(1, 0, 3, 15));

        vdp_sync_vblank_out_set(_vblank_out_handler);

        vdp1_vram_partitions_set(VDP1_VRAM_CMDT_COUNT,
            VDP1_VRAM_TEXTURE_SIZE,
            VDP1_VRAM_GOURAUD_COUNT,
            VDP1_VRAM_CLUT_COUNT);

        vdp1_env_default_set();
        vdp2_sprite_priority_set(0, 6);

        cpu_intc_mask_set(0);

        dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
        dbgio_dev_font_load();
        dbgio_dev_font_load_wait();

        vdp2_tvmd_display_set();
}

static void
_vblank_out_handler(void *work __unused)
{
        smpc_peripheral_intback_issue();
}