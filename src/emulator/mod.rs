use crate::{consts::*, errors::*, instructions::TamInstruction};

const MEMORY_SIZE: usize = (u16::MAX as usize) + 1;

pub struct TamEmulator {
    code_store: [u32; MEMORY_SIZE],
    data_store: [i16; MEMORY_SIZE],
    registers: [u16; 16],
}

impl TamEmulator {
    /// Creates a new emulator with default registers and zeroed memory.
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

    /// Retrieves the next instruction to be executed. Returns a [TamError::AccessViolation]
    /// if the code pointer points beyond the last instruction.
    pub fn fetch_decode(&mut self) -> TamResult<TamInstruction> {
        if self.registers[CP] >= self.registers[CT] {
            return Err(TamError::AccessViolation);
        }

        let code = self.code_store[self.registers[CP] as usize];
        Ok(TamInstruction::from(code))
    }
}

mod execute;

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::*;

    #[fixture]
    fn emulator() -> TamEmulator {
        TamEmulator::new()
    }

    #[rstest]
    fn new_values_set_correctly(emulator: TamEmulator) {
        for i in 0..MEMORY_SIZE {
            assert_eq!(0, emulator.code_store[i], "code word {i} not zeroed");
            assert_eq!(0, emulator.data_store[i], "data word {i} not zeroed")
        }

        for i in 0..16 {
            match i {
                HB | HT | PT => assert_eq!(
                    (MEMORY_SIZE - 1) as u16,
                    emulator.registers[i],
                    "register {i} not set correctly"
                ),
                PB => assert_eq!(
                    (MEMORY_SIZE - 29) as u16,
                    emulator.registers[i],
                    "register {i} not set correctly"
                ),
                _ => assert_eq!(0, emulator.registers[i], "register {i} not zeroed"),
            }
        }
    }

    #[rstest]
    fn fetch_decode_cp_in_range_ok(mut emulator: TamEmulator) {
        emulator.code_store[0] = 0x13572468;
        emulator.registers[CT] = 1;

        let result = emulator.fetch_decode();
        if result.is_err() {
            panic!("fetch_decode should not have errored");
        }

        let instr = result.unwrap();
        assert_eq!(
            TamInstruction {
                op: 1,
                r: 3,
                n: 87,
                d: 9320
            },
            instr,
            "decoded wrong instruction"
        );
    }

    #[rstest]
    fn fetch_decode_cp_out_of_range_err(mut emulator: TamEmulator) {
        emulator.registers[CT] = 10;
        emulator.registers[CP] = 20;

        let result = emulator.fetch_decode();
        if result.is_ok() {
            panic!("fetch_decode should have errored");
        }

        let error = result.unwrap_err();
        assert_eq!(TamError::AccessViolation, error);
    }
}
