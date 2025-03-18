use crate::{consts::*, errors::*, instructions::TamInstruction};
use std::collections::VecDeque;

use super::TamEmulator;

impl TamEmulator {
    /// Executes the given instruction and returns `Ok(true)` if execution should continue
    /// or `Ok(false)` if it should halt.
    pub fn execute(&mut self, instr: TamInstruction) -> TamResult<bool> {
        match instr.op {
            0 => self.exec_load(instr)?,
            15 => return Ok(false),
            op => return Err(TamError::UnknownOpcode(op)),
        };

        Ok(true)
    }

    fn push_data(&mut self, data: i16) -> TamResult<()> {
        if self.registers[ST] >= self.registers[HT] {
            return Err(TamError::StackOverflow);
        }

        let addr = self.registers[ST] as usize;
        self.data_store[addr] = data;
        self.registers[ST] += 1;
        Ok(())
    }

    fn pop_data(&mut self) -> TamResult<i16> {
        if self.registers[ST] == 0 {
            return Err(TamError::StackUnderflow);
        }

        self.registers[ST] -= 1;
        let addr = self.registers[ST] as usize;
        Ok(self.data_store[addr])
    }

    fn calc_address(&self, instr: TamInstruction) -> TamResult<u16> {
        match u16::checked_add_signed(self.registers[instr.r as usize], instr.d) {
            None => Err(TamError::AccessViolation),
            Some(addr) => {
                if addr >= self.registers[ST] && addr <= self.registers[HT] {
                    Err(TamError::AccessViolation)
                } else {
                    Ok(addr)
                }
            }
        }
    }

    fn exec_load(&mut self, instr: TamInstruction) -> TamResult<()> {
        let mut data = VecDeque::new();
        let base_addr = self.calc_address(instr)?;
        for i in 0..instr.n {
            let addr = base_addr + (i as u16);
            if addr >= self.registers[ST] && addr <= self.registers[HT] {
                return Err(TamError::AccessViolation);
            }
            data.push_back(self.data_store[addr as usize]);
        }

        while !data.is_empty() {
            self.push_data(data.pop_front().expect("data stack was unexpectedly empty"))?;
        }

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::*;

    #[fixture]
    fn emulator() -> TamEmulator {
        TamEmulator::new()
    }

    #[rstest]
    fn execute_unknown_op_err(mut emulator: TamEmulator) {
        let instr = TamInstruction {
            op: 28,
            r: 0,
            n: 0,
            d: 0,
        };
        let result = emulator.execute(instr);
        if let Ok(_) = result {
            panic!("should have failed to execute");
        }

        let err = result.unwrap_err();
        assert_eq!(
            TamError::UnknownOpcode(28),
            err,
            "execute returned wrong error"
        );
    }

    #[rstest]
    fn execute_load_good_address_ok(mut emulator: TamEmulator) {
        emulator.data_store[0] = 5;
        emulator.data_store[1] = 10;
        emulator.data_store[2] = 15;
        emulator.registers[ST] = 3;

        let instr = TamInstruction {
            op: 0,
            r: SB as u8,
            n: 2,
            d: 1,
        };

        let result = emulator
            .execute(instr)
            .expect("execute should not have failed");
        assert!(result, "execute should not have returned false");
        assert_eq!(10, emulator.data_store[3], "first value not pushed");
        assert_eq!(15, emulator.data_store[4], "second value not pushed");
    }

    #[rstest]
    fn execute_halt_return_false(mut emulator: TamEmulator) {
        let instr = TamInstruction {
            op: 15,
            r: 0,
            n: 0,
            d: 0,
        };
        let result = emulator.execute(instr).expect("execute returned an error");
        assert!(!result, "execute should have returned false");
    }
}
