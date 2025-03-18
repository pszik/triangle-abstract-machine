use tam_rs::{errors::TamResult, TamEmulator};

fn main() -> TamResult<()> {
    let mut emu = TamEmulator::new();
    loop {
        let instr = emu.fetch_decode()?;
        if !emu.execute(instr)? {
            break;
        }
    }

    Ok(())
}
