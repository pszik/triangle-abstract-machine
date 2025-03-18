/// Error types that can be returned by a running emulator.
#[derive(Copy, Clone, Debug, PartialEq, Eq)]
pub enum TamError {
    IOError,
    AccessViolation,
    StackUnderflow,
    StackOverflow,
}

/// Convenient shorthand for a [Result] returning a [TamError] as its error type.
pub type TamResult<T> = Result<T, TamError>;
