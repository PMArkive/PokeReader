// PNP Compatibility file
// PNP uses wasm, so all functions are wasm compatible - hence the weirdness.

#include <3ds.h>
#include <string.h>
#include "ov.h"
#include "csvc.h"
#include "pnp.h"
#include "common.h"
#include "title_info.h"
#include "hid.h"

const u32 default_print_x = 8;
const u32 default_print_y = 10;
const u32 default_print_max_len = 30;

u32 print_x = default_print_x;
u32 print_y = default_print_y;
u32 print_max_len = default_print_max_len;

#define MAX_LINES 18
#define MAX_LINE_LENGTH 46

char print_buffer[MAX_LINES][MAX_LINE_LENGTH];
u32 buffer_index = 0;

void reset_print()
{
  print_x = default_print_x;
  print_y = default_print_y;
  print_max_len = default_print_max_len;
  memset(print_buffer, 0x00, MAX_LINES * MAX_LINE_LENGTH);
  buffer_index = 0;
}

void draw_to_screen(u32 screenId, u8 *framebuffer, u32 stride, u32 format)
{
  if (buffer_index == 0)
  {
    return;
  }

  ovDrawTranspartBlackRect((u32)framebuffer, stride, format, print_y, print_x, buffer_index * 12 + 4, print_max_len * 8 + 8, 1);

  print_x += 4;
  print_y += 4;

  for (u32 i = 0; i < buffer_index; i++)
  {
    ovDrawString((u32)framebuffer, stride, format, SCREEN_WIDTH, print_y, print_x, 0xff, 0xff, 0xff, print_buffer[i]);
    print_y += 12;
  }

  reset_print();
}

void host_print(u32 ptr, u32 size)
{
  if (buffer_index < MAX_LINES)
  {
    u32 copy_size = (size < print_max_len - 1) ? size : print_max_len - 1;
    memcpy(print_buffer[buffer_index], (char *)ptr, copy_size);
    print_buffer[buffer_index][copy_size] = '\0'; // Null-terminate the string
    buffer_index++;
  }
}

void host_read_mem(u32 game_addr, u32 size, u32 out_ptr)
{
  memcpy((void *)out_ptr, (void *)game_addr, size);
}

void host_write_mem(u32 game_addr, u32 size, u32 in_ptr)
{
  memcpy((void *)game_addr, (void *)in_ptr, size);
}

u32 host_just_pressed()
{
  return (get_previous_keys() ^ 0xffffffff) & get_current_keys();
}

u32 host_is_just_pressed(u32 io_bits)
{
  u32 just_pressed = host_just_pressed();
  bool is_just_pressed = (just_pressed & io_bits) != 0 && io_bits == get_current_keys();
  return (u32)is_just_pressed;
}

void host_set_print_max_len(u32 max_len)
{
  u32 max_len_with_terminator = max_len + 1;
  print_max_len = max_len_with_terminator > MAX_LINE_LENGTH ? MAX_LINE_LENGTH : max_len_with_terminator;
}

u64 host_get_game_title_id()
{
  return get_title_id();
}
