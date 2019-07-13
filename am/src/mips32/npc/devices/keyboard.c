#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define KBD_CODE 0
#define KBD_STAT 4
#define KEYDOWN_MASK 0x8000

int normal_scancode[256] = {
    [0x01] = _KEY_F9,
    [0x03] = _KEY_F5,
    [0x04] = _KEY_F3,
    [0x05] = _KEY_F1,
    [0x06] = _KEY_F2,
    [0x07] = _KEY_F12,
    [0x09] = _KEY_F10,
    [0x0A] = _KEY_F8,
    [0x0B] = _KEY_F6,
    [0x0C] = _KEY_F4,
    [0x0D] = _KEY_TAB,
    [0x0E] = _KEY_GRAVE,
    [0x11] = _KEY_LALT,
    [0x12] = _KEY_LSHIFT,
    [0x14] = _KEY_LCTRL,
    [0x15] = _KEY_Q,
    [0x16] = _KEY_1,
    [0x1A] = _KEY_Z,
    [0x1B] = _KEY_S,
    [0x1C] = _KEY_A,
    [0x1D] = _KEY_W,
    [0x1E] = _KEY_2,
    [0x21] = _KEY_C,
    [0x22] = _KEY_X,
    [0x23] = _KEY_D,
    [0x24] = _KEY_E,
    [0x25] = _KEY_4,
    [0x26] = _KEY_3,
    [0x29] = _KEY_SPACE,
    [0x2A] = _KEY_V,
    [0x2B] = _KEY_F,
    [0x2C] = _KEY_T,
    [0x2D] = _KEY_R,
    [0x2E] = _KEY_5,
    [0x31] = _KEY_N,
    [0x32] = _KEY_B,
    [0x33] = _KEY_H,
    [0x34] = _KEY_G,
    [0x35] = _KEY_Y,
    [0x36] = _KEY_6,
    [0x3A] = _KEY_M,
    [0x3B] = _KEY_J,
    [0x3C] = _KEY_U,
    [0x3D] = _KEY_7,
    [0x3E] = _KEY_8,
    [0x41] = _KEY_COMMA,
    [0x42] = _KEY_K,
    [0x43] = _KEY_I,
    [0x44] = _KEY_O,
    [0x45] = _KEY_0,
    [0x46] = _KEY_9,
    [0x49] = _KEY_PERIOD,
    [0x4A] = _KEY_BACKSLASH,
    [0x4B] = _KEY_L,
    [0x4C] = _KEY_SEMICOLON,
    [0x4D] = _KEY_P,
    [0x4E] = _KEY_MINUS,
    /* [0x52] = _KEY_QUOTE, */
    [0x54] = _KEY_LEFTBRACKET,
    [0x55] = _KEY_EQUALS,
    [0x58] = _KEY_CAPSLOCK,
    [0x59] = _KEY_RSHIFT,
    [0x5A] = _KEY_RETURN,
    [0x5B] = _KEY_RIGHTBRACKET,
    [0x5D] = _KEY_SLASH,
    [0x66] = _KEY_BACKSPACE,
    [0x69] = _KEY_1,
    [0x6B] = _KEY_4,
    [0x6C] = _KEY_7,
    [0x70] = _KEY_0,
    [0x71] = _KEY_EQUALS,
    [0x72] = _KEY_2,
    [0x73] = _KEY_5,
    [0x74] = _KEY_6,
    [0x75] = _KEY_8,
    [0x76] = _KEY_ESCAPE,
    /* [0x77] = _KEY_NUMLOCK, */
    [0x78] = _KEY_F11,
    /* [0x79] = _KEY_PLUS, */
    [0x7A] = _KEY_3,
    [0x7B] = _KEY_MINUS,
    /* [0x7C] = _KEY_KP_MULTIPLY, */
    [0x7D] = _KEY_9,
    [0x83] = _KEY_F7,
};

int e0_scan_code[256] = {
    [0x11] = _KEY_RALT,     [0x14] = _KEY_RCTRL,
    [0x4A] = _KEY_SLASH,    [0x5A] = _KEY_RETURN,
    [0x69] = _KEY_END,      [0x6B] = _KEY_LEFT,
    [0x6C] = _KEY_HOME,     [0x70] = _KEY_INSERT,
    [0x71] = _KEY_DELETE,   [0x72] = _KEY_DOWN,
    [0x74] = _KEY_RIGHT,    [0x75] = _KEY_UP,
    [0x7A] = _KEY_PAGEDOWN, [0x7D] = _KEY_PAGEUP,
};

static size_t keyboard_read(_DEV_INPUT_KBD_t *kbd) {
  if (inl(KBD_ADDR + KBD_STAT) == 0) {
    kbd->keydown = 0;
    kbd->keycode = 0;
    return sizeof(*kbd);
  }

  int code = inl(KBD_ADDR + KBD_CODE);
  int *table = normal_scancode;

  kbd->keydown = 1;
  kbd->keycode = 0;
  for (int i = 0; i < 4; i++) {
    int byte =
        (code & (0xFF << ((3 - i) * 8))) >> ((3 - i) * 8);
    if (byte == 0xE0)
      table = e0_scan_code;
    else if (byte == 0xF0)
      kbd->keydown = 0;
    else
      kbd->keycode = table[byte];
  }
  return sizeof(*kbd);
}

size_t __am_input_read(uintptr_t reg, void *buf,
                       size_t size) {
  switch (reg) {
  case _DEVREG_INPUT_KBD: return keyboard_read(buf);
  }
  return 0;
}
