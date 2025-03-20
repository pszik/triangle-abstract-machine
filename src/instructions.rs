use std::fmt::Display;

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

impl TamInstruction {
    pub fn make(op: u8, r: u8, n: u8, d: i16) -> TamInstruction {
        TamInstruction { op, r, n, d }
    }
}

impl From<u32> for TamInstruction {
    fn from(value: u32) -> Self {
        let op = (value & 0xf0000000) >> 28;
        let r = (value & 0x0f000000) >> 24;
        let n = (value & 0x00ff0000) >> 16;
        let d = value & 0x0000ffff;
        TamInstruction::make(op as u8, r as u8, n as u8, d as i16)
    }
}

impl Display for TamInstruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self.op {
            0 => write!(f, "LOAD({}) {}[{}]", self.n, self.d, self.r),
            1 => write!(f, "LOADA {}[{}]", self.d, self.r),
            2 => write!(f, "LOADI ({})", self.n),
            3 => write!(f, "LOADL {}", self.d),
            4 => write!(f, "STORE({}) {}[{}]", self.n, self.d, self.r),
            5 => write!(f, "STOREI({})", self.n),
            6 => write!(f, "CALL({}) {}[{}]", self.n, self.d, self.r),
            7 => write!(f, "CALLI"),
            8 => write!(f, "RETURN({}) {}", self.n, self.d),
            10 => write!(f, "PUSH {}", self.d),
            11 => write!(f, "POP({}) {}", self.n, self.d),
            12 => write!(f, "JUMP {}[{}]", self.d, self.r),
            13 => write!(f, "JUMPI"),
            14 => write!(f, "JUMPIF({}) {}[{}]", self.n, self.d, self.r),
            15 => write!(f, "HALT"),
            _ => write!(f, ""),
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
