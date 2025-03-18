/// A single TAM instruction.
#[derive(Copy, Clone, Debug, PartialEq, Eq)]
pub struct TamInstruction {
    /// Opcode
    pub op: u8,
    /// Register
    pub r: u8,
    /// Unsigned operand
    pub n: u8,
    /// Signed operand
    pub d: i16,
}

impl From<u32> for TamInstruction {
    fn from(value: u32) -> Self {
        let op = (value & 0xf0000000) >> 28;
        let r = (value & 0x0f000000) >> 24;
        let n = (value & 0x00ff0000) >> 16;
        let d = value & 0x0000ffff;
        TamInstruction {
            op: op as u8,
            r: r as u8,
            n: n as u8,
            d: d as i16,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::*;

    #[rstest]
    fn from_u32() {
        let code = 0x38f1cf3f;
        let instr = TamInstruction::from(code);
        assert_eq!(3, instr.op);
        assert_eq!(8, instr.r);
        assert_eq!(241, instr.n);
        assert_eq!(-12481, instr.d);
    }
}
