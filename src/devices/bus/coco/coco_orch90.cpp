// license:BSD-3-Clause
// copyright-holders:Nathan Woods
/***************************************************************************

    coco_orch90.cpp

    Code for emulating the CoCo Orch-90 (Orchestra 90) sound cartridge

    The Orch-90 was a simple sound cartridge; it had two 8-bit DACs
    supporting stereo sound.  The left channel was at $FF7A, and the right
    channel was at $FF7B


    Hidden Code exists in the ORCH-90 to produce a higher quality output.
    Once you start the PAK, hit "SHIFT+ENTER" at the title screen.  Once you
    are at command, type the following (playing the William Tell default):
    "S" + "ENTER"
    "P" + "SHIFT+ENTER"

    The output will be cleaner due to code causing a change in CPU speed by
    addressing CPU registers to run the 6809 at 2x speed.

    "P" + "ENTER" will play at regular CPU speed.  The difference should be
    very noticable.

***************************************************************************/

#include "emu.h"
#include "coco_orch90.h"
#include "cococart.h"

#include "sound/dac.h"
#include "speaker.h"


//**************************************************************************
//  ROM DECLARATIONS
//**************************************************************************

ROM_START(coco_orch90)
	ROM_REGION(0x2000, "eprom", ROMREGION_ERASE00)
	ROM_LOAD("orchestra 90 (1984)(26 - 3143)(tandy).rom", 0x0000, 0x2000, CRC(15fb39af) SHA1(6a20fee9c70b36a6435ac8378f31d5b626017df0))
ROM_END


//**************************************************************************
//  ORCH90 DEVICE CLASS
//**************************************************************************

namespace
{
	// ======================> coco_orch90_device

	class coco_orch90_device :
		public device_t,
		public device_cococart_interface
	{
	public:
		// construction/destruction
		coco_orch90_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
			: device_t(mconfig, COCO_ORCH90, tag, owner, clock)
			, device_cococart_interface(mconfig, *this)
			, m_ldac(*this, "ldac")
			, m_rdac(*this, "rdac")
		{
		}

		// optional information overrides
		virtual void device_add_mconfig(machine_config &config) override;

	protected:
		// device-level overrides
		virtual void device_start() override
		{
			// install handlers
			install_write_handler(0xFF7A, 0xFF7A, write8_delegate(FUNC(coco_orch90_device::write_left), this));
			install_write_handler(0xFF7B, 0xFF7B, write8_delegate(FUNC(coco_orch90_device::write_right), this));

			// Orch-90 ties CART to Q
			set_line_value(line::CART, line_value::Q);
		}

		virtual const tiny_rom_entry *device_rom_region() const override
		{
			return ROM_NAME(coco_orch90);
		}

		// CoCo cartridge level overrides
		virtual uint8_t *get_cart_base() override
		{
			return memregion("eprom")->base();
		}

	private:
		WRITE8_MEMBER(write_left)   { m_ldac->write(data); }
		WRITE8_MEMBER(write_right)  { m_rdac->write(data); }

		// internal state
		required_device<dac_byte_interface> m_ldac;
		required_device<dac_byte_interface> m_rdac;
	};
};


//**************************************************************************
//  MACHINE AND ROM DECLARATIONS
//**************************************************************************

MACHINE_CONFIG_MEMBER(coco_orch90_device::device_add_mconfig)
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("ldac", DAC_8BIT_R2R, 0) MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.5) // ls374.ic5 + r7 (8x20k) + r9 (8x10k)
	MCFG_SOUND_REFERENCE_INPUT(DAC_VREF_POS_INPUT, 1.0) MCFG_SOUND_REFERENCE_INPUT(DAC_VREF_NEG_INPUT, -1.0)
	MCFG_SOUND_ADD("rdac", DAC_8BIT_R2R, 0) MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.5) // ls374.ic4 + r6 (8x20k) + r8 (8x10k)
	MCFG_SOUND_REFERENCE_INPUT(DAC_VREF_POS_INPUT, 1.0) MCFG_SOUND_REFERENCE_INPUT(DAC_VREF_NEG_INPUT, -1.0)
MACHINE_CONFIG_END


//**************************************************************************
//  DEVICE DECLARATION
//**************************************************************************

DEFINE_DEVICE_TYPE(COCO_ORCH90, coco_orch90_device, "coco_orch90", "CoCo Orch-90 PAK")
