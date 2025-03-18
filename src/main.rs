use byteorder::{ReadBytesExt, BE};
use std::{
    fs::File,
    io::{Seek, SeekFrom},
};
use tam_rs::{errors::*, TamEmulator};

fn code_from_file(filename: &str) -> std::io::Result<Vec<u32>> {
    let mut file = File::open(filename)?;

    file.seek(SeekFrom::End(0))?;
    let file_len = file.stream_position()?;
    file.rewind()?;

    let mut code = Vec::with_capacity((file_len / 4) as usize);
    file.read_u32_into::<BE>(&mut code)?;
    Ok(code)
}

fn main() -> TamResult<()> {
    let mut emu = TamEmulator::new();
    let code = code_from_file("test.tam").map_err(|_| TamError::IOError)?;
    emu.set_code(&code)?;

    loop {
        let instr = emu.fetch_decode()?;
        if !emu.execute(instr)? {
            break;
        }
    }

    Ok(())
}
