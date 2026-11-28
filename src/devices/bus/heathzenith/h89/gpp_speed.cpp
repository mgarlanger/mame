// license:BSD-3-Clause
// copyright-holders:Mark Garlanger
/***************************************************************************

  CPU speed upgrades switched by MEM 0 of the general purpose port

  Setting MEM 0 runs the CPU at the higher speed. Only the CPU clock is
  changed; the bus clock is left alone.

  2 / 4 MHz - offered by several companies including Kres, ANAPRO, and an
              article in REMark magazine.
  2 / 6 MHz - offered by at least ANAPRO, and a how to article in the CHUG
              newsletter.

  The device clock is the CPU board's 12.288 MHz crystal, which is divided
  by 6 for the normal 2 MHz, and by 3 or 2 for 4 or 6 MHz.

****************************************************************************/

#include "emu.h"

#include "gpp_speed.h"

namespace {

class h89_gpp_speed_device : public device_t, public device_h89bus_cpu_board_mod_interface
{
protected:
	h89_gpp_speed_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, u32 fast_divisor);

	// device_t implementation
	virtual void device_start() override ATTR_COLD;

private:
	void gpp_w(u8 data);

	// divisor of the CPU board's crystal for the normal 2 MHz speed
	static constexpr u32 SLOW_DIVISOR = 6;

	// divisor of the CPU board's crystal for the fast speed
	u32 const m_fast_divisor;
};

h89_gpp_speed_device::h89_gpp_speed_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, u32 fast_divisor)
	: device_t(mconfig, type, tag, owner, clock)
	, device_h89bus_cpu_board_mod_interface(mconfig, *this)
	, m_fast_divisor(fast_divisor)
{
}

void h89_gpp_speed_device::device_start()
{
	h89bus().add_gpp_handler(h89bus::GPP_MEM0, write8smo_delegate(*this, FUNC(h89_gpp_speed_device::gpp_w)));
}

void h89_gpp_speed_device::gpp_w(u8 data)
{
	set_cpu_clock(clock() / ((data & h89bus::GPP_MEM0) ? m_fast_divisor : SLOW_DIVISOR));
}


class h89_gpp_speed_4mhz_device : public h89_gpp_speed_device
{
public:
	h89_gpp_speed_4mhz_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: h89_gpp_speed_device(mconfig, H89BUS_GPP_SPEED_4MHZ, tag, owner, clock, 3)
	{
	}
};

class h89_gpp_speed_6mhz_device : public h89_gpp_speed_device
{
public:
	h89_gpp_speed_6mhz_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: h89_gpp_speed_device(mconfig, H89BUS_GPP_SPEED_6MHZ, tag, owner, clock, 2)
	{
	}
};

} // anonymous namespace

DEFINE_DEVICE_TYPE_PRIVATE(H89BUS_GPP_SPEED_4MHZ, device_h89bus_cpu_board_mod_interface, h89_gpp_speed_4mhz_device, "h89_gpp_4mhz", "2 / 4 MHz Speed Upgrade (GPP MEM 0)")
DEFINE_DEVICE_TYPE_PRIVATE(H89BUS_GPP_SPEED_6MHZ, device_h89bus_cpu_board_mod_interface, h89_gpp_speed_6mhz_device, "h89_gpp_6mhz", "2 / 6 MHz Speed Upgrade (GPP MEM 0)")
