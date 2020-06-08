#include <kos.h>
#include <wav/sndwav.h>

#define LOOP 1

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);

static void draw_instructions(int faucet_vol, int brushing_vol);

static cont_state_t* get_cont_state();
static int button_pressed(unsigned int current_buttons, unsigned int changed_buttons, unsigned int button);

int main(int argc, char **argv) {
    uint8_t faucet_volume = 240;
    uint8_t brushing_volume = 240;
    int volume_changed = 1;
    cont_state_t* cond;

    vid_set_mode(DM_640x480, PM_RGB555);
    // Initialize sound system and WAV
    snd_stream_init();
    wav_init();

    wav_stream_hnd_t faucet = wav_create("/rd/bathroom-faucet-1.wav", LOOP);
    wav_volume(faucet, faucet_volume);

    wav_stream_hnd_t brushing = wav_create("/rd/brushing-teeth-1.wav", LOOP);
    wav_volume(brushing, brushing_volume);

    wav_play(faucet);
    wav_play(brushing);

    unsigned int current_buttons = 0;
    unsigned int changed_buttons = 0;
    unsigned int previous_buttons = 0;

    for(;;) {
        cond = get_cont_state();
        current_buttons = cond->buttons;
        changed_buttons = current_buttons ^ previous_buttons;
        previous_buttons = current_buttons;
        
        // Play wav files if they arent playing already
        if(button_pressed(current_buttons, changed_buttons, CONT_X)) {
            if(!wav_isplaying(faucet))
                wav_play(faucet);
        }
        if(button_pressed(current_buttons, changed_buttons, CONT_Y)) {
            if(!wav_isplaying(brushing))
                wav_play(brushing);
        }
        // Stop playing faucet
        if(button_pressed(current_buttons, changed_buttons, CONT_A)) {
            wav_stop(faucet);
        }
        // Stop playing brushing
        if(button_pressed(current_buttons, changed_buttons, CONT_B)) {
            wav_stop(brushing);
        }

        // Adjust Volume
        if(cond->ltrig > 0) {
            if(faucet_volume < 255)
                faucet_volume++;
            else
                faucet_volume = 0;
            volume_changed = 1;
            wav_volume(faucet, faucet_volume);
        }
        if(cond->rtrig > 0) {
            if(faucet_volume < 255)
                brushing_volume++;
            else
                brushing_volume = 0;
            volume_changed = 1;
            wav_volume(brushing, brushing_volume);
        }

        // Exit Program
        if(button_pressed(current_buttons, changed_buttons, CONT_START))
            break;

        if(volume_changed) {
            volume_changed = 0;
            draw_instructions(faucet_volume, brushing_volume);
        }
    }

    // Unload all sound effects from sound RAM
    snd_sfx_unload_all();	

    wav_shutdown();
    snd_stream_shutdown();

    return 0;
}

static void draw_instructions(int faucet_vol, int brushing_vol) {
    int x = 20, y = 20+24;
    int color = 1;
    char volume_str[32];

    bfont_draw_str(vram_s + y*640+x, 640, color, "Press X to play faucet sound");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press A to stop faucet sound");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press L-Trigger to increase faucet volume");
    y += 24;
    memset(volume_str, 0, 32);
    snprintf(volume_str, 32, "Faucet Volume: %3i", faucet_vol);
    bfont_draw_str(vram_s + y*640+x, 640, color, volume_str);
    
    y += 48;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press Y to play brushing sound");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press B to stop brushing sound");
    y += 24;
    bfont_draw_str(vram_s + y*640+x, 640, color, "Press R-Trigger to increase brushing volume");
    y += 24;
    memset(volume_str, 0, 32);
    snprintf(volume_str, 32, "Brushing Volume: %3i", brushing_vol);
    bfont_draw_str(vram_s + y*640+x, 640, color, volume_str);

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
