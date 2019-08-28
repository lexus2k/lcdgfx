#ifdef SDL_EMULATION
    // TODO: Add SDL_LCD_~CONTROLLER~ enum value to enum list in tools/sdl/sdl_core.h
    SDL_LCD_~CONTROLLER~, 0x00,
    // TODO: Add initialization argument for SDL emulator (display specific)
    0x00, 0x00,
#endif
    // Place your data here in the following format
    // ...
    // ID_CMD, N_ARGS, ARG1, ARG2, ..., ARGN,    // This is for regular LCD commands
    // ...
    // ID_CMD, CMD_DELAY, DELAY_IN_MS,           // This is for LCD command without args, requiring delay
    // ...
