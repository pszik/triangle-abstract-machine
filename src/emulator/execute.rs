use std::io::{BufRead, Write};

use super::TamEmulator;
use crate::{consts::*, errors::*, instructions::TamInstruction};

impl TamEmulator {
    /// Executes the given instruction and returns `Ok(true)` if execution should continue
    /// or `Ok(false)` if it should halt.
    ///
    /// Parameters [reader] and [writer] are used for primitive operations which interact
    /// with input and output.
    pub fn execute(
        &mut self,
        instr: TamInstruction,
        reader: &mut impl BufRead,
        writer: &mut impl Write,
    ) -> TamResult<bool> {
        if self.trace {
            writeln!(writer, "{}", instr).map_err(|e| TamError {
                kind: TamErrorKind::IOError,
                address: Some(self.registers[CP]),
                message: Some(e.to_string()),
            })?;
        }

        match instr.op {
            0 => self.exec_load(instr)?,
            1 => self.exec_loada(instr)?,
            2 => self.exec_loadi(instr)?,
            3 => self.exec_loadl(instr)?,
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

    fn exec_loada(&mut self, instr: TamInstruction) -> TamResult<()> {
        self.calc_address(instr)
            .and_then(|addr| self.push_data(addr as i16))
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

    fn exec_loadl(&mut self, instr: TamInstruction) -> TamResult<()> {
        self.push_data(instr.d)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::*;

    #[fixture]
    fn emulator() -> TamEmulator {
        TamEmulator::new(false)
    }

    #[fixture]
    fn reader() -> impl BufRead {
        std::io::stdin().lock()
    }

    #[fixture]
    fn writer() -> impl Write {
        std::io::stdout()
    }

    #[rstest]
    fn execute_unknown_op_err(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(28, 0, 0, 0);
        let err = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("should have failed to execute");

        assert_eq!(
            TamErrorKind::UnknownOpcode(28),
            err.kind,
            "execute returned wrong error"
        );
    }

    #[rstest]
    fn execute_load_good_address_ok(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        emulator.data_store[0] = 5;
        emulator.data_store[1] = 10;
        emulator.data_store[2] = 15;
        emulator.registers[ST] = 3;

        let instr = TamInstruction::make(0, SB as u8, 2, 1);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect("execute should not have failed");

        assert!(result, "execute should not have returned false");
        assert_eq!(10, emulator.data_store[3], "first value not pushed");
        assert_eq!(15, emulator.data_store[4], "second value not pushed");
    }

    #[rstest]
    fn execute_load_bad_address_err(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(0, SB as u8, 1, 20);
        let error = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("execute should have returned an error");

        assert_eq!(TamErrorKind::AccessViolation, error.kind);
        assert_eq!(Some(0), error.address);
    }

    #[rstest]
    fn execute_loada_ok(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(1, SB as u8, 0, 3);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect("execute should not have errored");

        assert!(result, "execute should not have returned false");
        assert_eq!(3, emulator.data_store[0], "wrong value loaded");
    }

    #[rstest]
    fn execute_loada_full_stack_stack_overflow(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        emulator.registers[ST] = 1;
        emulator.registers[HT] = 1;
        let instr = TamInstruction::make(1, SB as u8, 0, 3);

        let error = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("execute should not have succeeded");
        assert_eq!(TamErrorKind::StackOverflow, error.kind);
    }

    #[rstest]
    fn execute_loadi_good_address_full_stack_ok(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        emulator.data_store[0..3].copy_from_slice(&[10, 20, 0]);
        emulator.registers[ST] = 3;

        let instr = TamInstruction::make(2, 0, 1, 0);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect("execute should not have errored");
        assert!(result, "execute should not have returned false");
        assert_eq!(10, emulator.data_store[2], "wrong value loaded");
    }

    #[rstest]
    fn execute_loadi_good_address_empty_stack_stack_underflow(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(2, 0, 1, 0);
        let error = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("execute should not have succeeded");

        assert_eq!(TamErrorKind::StackUnderflow, error.kind);
        assert_eq!(Some(0), error.address);
    }

    #[rstest]
    fn execute_loadi_bad_address_access_violation(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        emulator.data_store[0..3].copy_from_slice(&[10, 20, 30]);
        emulator.registers[ST] = 3;
        let instr = TamInstruction::make(2, 0, 1, 0);

        let error = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("execute should not have succeeded");

        assert_eq!(TamErrorKind::AccessViolation, error.kind);
        assert_eq!(Some(0), error.address);
    }

    #[rstest]
    fn execute_loadl_ok(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(3, 0, 0, -45);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect("execute should not have errored");

        assert!(result);
        assert_eq!(-45, emulator.data_store[0], "wrong value was loaded");
    }

    #[rstest]
    fn execute_loadl_full_stack_stack_overflow(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        emulator.registers[HT] = 0;
        let instr = TamInstruction::make(3, 0, 0, -45);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect_err("execute should have errored");

        assert_eq!(TamErrorKind::StackOverflow, result.kind);
    }

    #[rstest]
    fn execute_halt_return_false(
        mut emulator: TamEmulator,
        mut reader: impl BufRead,
        mut writer: impl Write,
    ) {
        let instr = TamInstruction::make(15, 0, 0, 0);
        let result = emulator
            .execute(instr, &mut reader, &mut writer)
            .expect("execute returned an error");
        assert!(!result, "execute should have returned false");
    }
}
