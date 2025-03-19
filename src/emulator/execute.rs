use crate::{consts::*, errors::*, instructions::TamInstruction};
use std::collections::VecDeque;

use super::TamEmulator;

impl TamEmulator {
    /// Executes the given instruction and returns `Ok(true)` if execution should continue
    /// or `Ok(false)` if it should halt.
    pub fn execute(&mut self, instr: TamInstruction) -> TamResult<bool> {
        match instr.op {
            0 => self.exec_load(instr)?,
            2 => self.exec_loadi(instr)?,
            15 => return Ok(false),
            op => {
                return Err(TamError {
                    kind: TamErrorKind::UnknownOpcode(op),
                    address: Some(self.registers[CP]),
                    message: None,
                })
            }
        };

        Ok(true)
    }

    /// Pushes a value to the top of the stack and updates `ST`.
    ///
    /// # Returns
    ///
    /// Returns a stack overflow error if the push would expand the stack into the heap.
    fn push_data(&mut self, data: i16) -> TamResult<()> {
        if self.registers[ST] >= self.registers[HT] {
            return Err(TamError {
                kind: TamErrorKind::StackOverflow,
                address: Some(self.registers[CP]),
                message: None,
            });
        }

        let addr = self.registers[ST] as usize;
        self.data_store[addr] = data;
        self.registers[ST] += 1;
        Ok(())
    }

    /// Removes the top value of the stack and decrements `ST`.
    ///
    /// # Returns
    ///
    /// Returns the popped value, or a stack underflow error if the stack was empty.
    fn pop_data(&mut self) -> TamResult<i16> {
        if self.registers[ST] == 0 {
            return Err(TamError {
                kind: TamErrorKind::StackUnderflow,
                address: Some(self.registers[CP]),
                message: None,
            });
        }

        self.registers[ST] -= 1;
        let addr = self.registers[ST] as usize;
        Ok(self.data_store[addr])
    }

    /// Calculates an absolute address from the register value and signed argument of an
    /// instruction.
    ///
    /// # Returns
    ///
    /// The calculated address, or an access violation error of the calculation caused an integer
    /// underflow/overflow
    fn calc_address(&self, instr: TamInstruction) -> TamResult<u16> {
        match u16::checked_add_signed(self.registers[instr.r as usize], instr.d) {
            None => Err(TamError {
                kind: TamErrorKind::AccessViolation,
                address: Some(self.registers[CP]),
                message: None,
            }),
            Some(addr) => Ok(addr),
        }
    }

    fn exec_load(&mut self, instr: TamInstruction) -> TamResult<()> {
        let base_addr = self.calc_address(instr)?;
        for i in 0..instr.n {
            let addr = base_addr + (i as u16);
            if addr >= self.registers[ST] && addr <= self.registers[HT] {
                return Err(TamError {
                    kind: TamErrorKind::AccessViolation,
                    address: Some(self.registers[CP]),
                    message: None,
                });
            }
            self.push_data(self.data_store[addr as usize])?;
        }

        Ok(())
    }

    fn exec_loadi(&mut self, instr: TamInstruction) -> TamResult<()> {
        let base_addr = self.pop_data()? as u16;
        for i in 0..instr.n {
            let addr = base_addr + (i as u16);
            if addr >= self.registers[ST] && addr <= self.registers[HT] {
                return Err(TamError {
                    kind: TamErrorKind::AccessViolation,
                    address: Some(self.registers[CP]),
                    message: None,
                });
            }
            self.push_data(self.data_store[addr as usize])?;
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
            TamErrorKind::UnknownOpcode(28),
            err.kind,
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
    fn execute_load_bad_address_err(mut emulator: TamEmulator) {
        let instr = TamInstruction {
            op: 0,
            r: SB as u8,
            n: 1,
            d: 20,
        };
        let TamError {
            kind,
            address,
            message: _,
        } = emulator
            .execute(instr)
            .expect_err("execute should have returned an error");

        assert_eq!(TamErrorKind::AccessViolation, kind);
        assert_eq!(Some(0), address);
    }

    #[rstest]
    fn execute_loadi_good_address_full_stack_ok(mut emulator: TamEmulator) {
        emulator.data_store[0..3].copy_from_slice(&[10, 20, 0]);
        emulator.registers[ST] = 3;

        let instr = TamInstruction {
            op: 2,
            r: 0,
            n: 1,
            d: 0,
        };
        let result = emulator
            .execute(instr)
            .expect("execute should not have errored");
        assert!(result, "execute should not have returned false");
        assert_eq!(10, emulator.data_store[2], "wrong value loaded");
    }

    #[rstest]
    fn execute_loadi_good_address_empty_stack_stack_underflow(mut emulator: TamEmulator) {
        let instr = TamInstruction {
            op: 2,
            r: 0,
            n: 1,
            d: 0,
        };
        let TamError {
            kind,
            address,
            message: _,
        } = emulator
            .execute(instr)
            .expect_err("execute should not have succeeded");

        assert_eq!(TamErrorKind::StackUnderflow, kind);
        assert_eq!(Some(0), address);
    }

    #[rstest]
    fn execute_loadi_bad_address_access_violation(mut emulator: TamEmulator) {
        emulator.data_store[0..3].copy_from_slice(&[10, 20, 30]);
        emulator.registers[ST] = 3;
        let instr = TamInstruction {
            op: 2,
            r: 0,
            n: 1,
            d: 0,
        };

        let TamError {
            kind,
            address,
            message: _,
        } = emulator
            .execute(instr)
            .expect_err("execute should not have succeeded");

        assert_eq!(TamErrorKind::AccessViolation, kind);
        assert_eq!(Some(0), address);
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
