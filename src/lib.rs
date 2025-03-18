pub mod emulator;
pub mod errors;
pub mod instructions;

mod consts {
    pub const CB: usize = 0;
    pub const CT: usize = 1;
    pub const PB: usize = 2;
    pub const PT: usize = 3;
    pub const SB: usize = 4;
    pub const ST: usize = 5;
    pub const HB: usize = 6;
    pub const HT: usize = 7;
    pub const LB: usize = 8;
    pub const L1: usize = 9;
    pub const L2: usize = 10;
    pub const L3: usize = 11;
    pub const L4: usize = 12;
    pub const L5: usize = 13;
    pub const L6: usize = 14;
    pub const CP: usize = 15;
}

pub use emulator::TamEmulator;
