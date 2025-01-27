/*
 * update.cc (for Ultimate-II+)
 *
 *  Created on: May 19, 2016
 *      Author: gideon
 */

#include "update_common.h"

extern uint32_t _ultimate_run_rbf_start;
extern uint32_t _ultimate_run_rbf_end;

extern uint32_t _ultimate_app_start;
extern uint32_t _ultimate_app_end;

extern uint32_t _ultimate_recovery_rbf_start;
extern uint32_t _ultimate_recovery_rbf_end;

extern uint32_t _recovery_app_start;
extern uint32_t _recovery_app_end;

extern uint8_t _1541_bin_start;
extern uint8_t _1571_bin_start;
extern uint8_t _1581_bin_start;
extern uint8_t _snds1541_bin_start;
extern uint8_t _snds1571_bin_start;
extern uint8_t _snds1581_bin_start;

void do_update(void)
{
    REMOTE_FLASHSEL_1;
    REMOTE_FLASHSELCK_0;
    REMOTE_FLASHSELCK_1;

    setup("\033\025** Ultimate II+ Updater **\n\033\037");

/*
    if(user_interface->popup("Flash Recovery?", BUTTON_YES | BUTTON_NO) == BUTTON_YES) {
    	REMOTE_FLASHSEL_0;
        REMOTE_FLASHSELCK_0;
        REMOTE_FLASHSELCK_1;

        Flash *flash1 = get_flash();
        flash1->protect_disable();
        flash_buffer_at(flash1, screen, 0x000000, false, &_ultimate_recovery_rbf_start,   &_ultimate_recovery_rbf_end,   "V1.0", "Recovery FPGA");
        flash_buffer_at(flash1, screen, 0x080000, false, &_recovery_app_start,  &_recovery_app_end,  "V1.0", "Recovery Application");

    	console_print(screen, "\nConfiguring Flash write protection..\n");
    	console_print(screen, "Done!                            \n");
    }
*/
    check_flash_disk();

    if(user_interface->popup("Flash Runtime?", BUTTON_YES | BUTTON_NO) == BUTTON_YES) {
    	REMOTE_FLASHSEL_1;
        REMOTE_FLASHSELCK_0;
        REMOTE_FLASHSELCK_1;

        clear_field();
        create_dir(ROMS_DIRECTORY);
        create_dir(CARTS_DIRECTORY);
        write_flash_file("1581.rom", &_1581_bin_start, 0x8000);
        write_flash_file("1571.rom", &_1571_bin_start, 0x8000);
        write_flash_file("1541.rom", &_1541_bin_start, 0x4000);
        write_flash_file("snds1541.bin", &_snds1541_bin_start, 0xC000);
        write_flash_file("snds1571.bin", &_snds1571_bin_start, 0xC000);
        write_flash_file("snds1581.bin", &_snds1581_bin_start, 0xC000);

        Flash *flash2 = get_flash();
        flash2->protect_disable();
        flash_buffer(flash2, screen, FLASH_ID_BOOTFPGA, &_ultimate_run_rbf_start, &_ultimate_run_rbf_end, "", "Runtime FPGA");
        flash_buffer(flash2, screen, FLASH_ID_APPL,     &_ultimate_app_start,     &_ultimate_app_end,  APPL_VERSION, "Ultimate Application");

        write_protect(flash2);
    }
    turn_off();
}

extern "C" int ultimate_main(int argc, char *argv[])
{
	do_update();
}
