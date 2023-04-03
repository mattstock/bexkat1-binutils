#include "defs.h"
#include "sim-main.h"
#include "devices.h"

struct bexkat1_serial
{
  uint32_t base;
};

static unsigned
bexkat1_serial_io_read_buffer (struct hw *me, void *dest,
			       int space, address_world addr, unsigned nr_bytes)
{
  struct bexkat1_serial *serial = hw_data (me);
  return 0;
}

static unsigned
bexkat1_serial_io_write_buffer (struct hw *me, const void *source,
				int space, address_world addr,
				unsigned nr_bytes)
{
  struct bexkat1_serial *serial = hw_data (me);
  return 0;
}


static void
bexkat1_serial_finish (struct hw *me)
{
  struct bexkat1_serial *serial;

  serial = HW_ZALLOC (me, struct bexkat1_serial);

  set_hw_data (me, serial);
  set_hw_io_read_buffer (me, bexkat1_serial_io_read_buffer);
  set_hw_io_write_buffer (me, bexkat1_serial_io_write_buffer);

  serial->base = 0x10000000;
  hw_attach_address(hw_parent (me), 0, io_map, serial->base, 2, me);
}

const struct hw_descriptor dv_bexkat1_serial_descriptor[] =
  {
    {"bexkat1_serial", bexkat1_serial_finish,},
    {NULL, NULL},
  };
