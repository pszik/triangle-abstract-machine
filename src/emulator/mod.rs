use crate::consts::*;

const MEMORY_SIZE: usize = (u16::MAX as usize) + 1;

pub struct TamEmulator {
    code_store: [u32; MEMORY_SIZE],
    data_store: [i16; MEMORY_SIZE],
    registers: [u16; 16],
}

impl TamEmulator {
    pub fn new() -> Self {
        let mut emu = TamEmulator {
            code_store: [0; MEMORY_SIZE],
            data_store: [0; MEMORY_SIZE],
            registers: [0; 16],
        };

        emu.registers[PT] = (MEMORY_SIZE - 1) as u16;
        emu.registers[PB] = emu.registers[PT] - 28;
        emu.registers[HB] = (MEMORY_SIZE - 1) as u16;
        emu.registers[HT] = (MEMORY_SIZE - 1) as u16;
        emu
    }
}
