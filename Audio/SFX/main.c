#include <kos.h>

#define LEFT 0
#define CENTER 128
#define RIGHT 255

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

static void draw_instructions(uint8_t volume);

static cont_state_t* get_cont_state();
static int button_pressed(unsigned int current_buttons, unsigned int changed_buttons, unsigned int button);

int main(int argc, char **argv) {
    uint8_t volume = 128;
    int volume_changed = 1;
    cont_state_t* cond;

    vid_set_mode(DM_640x480, PM_RGB555);
    // Initialize sound system
    snd_init();

    // Load wav files found in romdisk
    sfxhnd_t beep1 = snd_sfx_load("/rd/beep-1.wav");
    sfxhnd_t beep2 = snd_sfx_load("/rd/beep-2.wav");
    sfxhnd_t beep3 = snd_sfx_load("/rd/beep-3.wav");
    sfxhnd_t beep4 = snd_sfx_load("/rd/beep-4.wav");

    unsigned int current_buttons = 0;
    unsigned int changed_buttons = 0;
    unsigned int previous_buttons = 0;

    for(;;) {
        cond = get_cont_state();
        current_buttons = cond->buttons;
        changed_buttons = current_buttons ^ previous_buttons;
        previous_buttons = current_buttons;
        
        // Play sounds on different channels
        if(button_pressed(current_buttons, changed_buttons, CONT_A)) {
            snd_sfx_play(beep1, volume, CENTER);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_B)) {
            snd_sfx_play(beep2, volume, RIGHT);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_X)) {
            snd_sfx_play(beep3, volume, LEFT);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_Y)) {
            snd_sfx_play(beep4, volume, CENTER);
        }
        
        // Play sounds on same channel
        if(button_pressed(current_buttons, changed_buttons, CONT_DPAD_DOWN)) {
            snd_sfx_play_chn(0, beep1, volume, CENTER);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_DPAD_RIGHT)) {
            snd_sfx_play_chn(0, beep2, volume, RIGHT);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_DPAD_LEFT)) {
            snd_sfx_play_chn(0, beep3, volume, LEFT);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_DPAD_UP)) {
            snd_sfx_play_chn(0, beep4, volume, CENTER);
        }

        // Adjust Volume
        if(cond->ltrig > 0) {
            volume_changed = 1;

            if(volume < 255)
                volume++;
        }
        if(cond->rtrig > 0) {
            volume_changed = 1;

            if(volume > 0)
                volume--;
        }

        // Exit Program
        if(button_pressed(current_buttons, changed_buttons, CONT_START))
            break;

        if(volume_changed) {
            volume_changed = 0;
            draw_instructions(volume);
        }
    }

    // Unload all sound effects from sound RAM
    snd_sfx_unload(beep1);	
    snd_sfx_unload(beep2);
    snd_sfx_unload(beep3);	
    snd_sfx_unload(beep4);
    // OR
    // snd_sfx_unload_all();	

    snd_shutdown();

    return 0;
}

static void draw_instructions(uint8_t volume) {
    int x = 20, y = 20+24;
    int color = 1;
    char current_volume_str[32];
    memset(current_volume_str, 0, 32);
    snprintf(current_volume_str, 32, "Current Volume: %3i", volume);
    
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press A,B,X,Y to play beeps on separate channels");
    y += 48;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press UP,DOWN,LEFT,RIGHT on D-Pad to play beeps");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, "on the same channel");
    y += 48;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press L-Trigger/R-Trigger to +/- volume");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, current_volume_str);
    y += 48;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press Start to exit program");
}

static cont_state_t* get_cont_state() {
    maple_device_t* cont;
    cont_state_t* state;

    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if(cont) {
        state = (cont_state_t*)maple_dev_status(cont);
        return state;
    }

    return NULL;
}

static int button_pressed(unsigned int current_buttons, unsigned int changed_buttons, unsigned int button) {
    if (changed_buttons & button) {
        if (current_buttons & button)
            return 1;
    }

    return 0;
}

