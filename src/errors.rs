/// Error types that can be returned by a running emulator.
#[derive(Copy, Clone, Debug, PartialEq, Eq)]
pub enum TamErrorKind {
    IOError,
    AccessViolation,
    StackUnderflow,
    StackOverflow,
    UnknownOpcode(u8),
    UnknownError,
}

#[derive(Clone, Debug, PartialEq, Eq)]
pub struct TamError {
    pub kind: TamErrorKind,
    pub address: Option<u16>,
    pub message: Option<String>,
}

impl Default for TamError {
    fn default() -> Self {
        TamError {
            kind: TamErrorKind::UnknownError,
            address: None,
            message: None,
        }
    }
}

/// Convenient shorthand for a [Result] returning a [TamError] as its error type.
pub type TamResult<T> = Result<T, TamError>;
