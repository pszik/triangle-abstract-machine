use byteorder::{ReadBytesExt, BE};
use std::{fs::File, io::BufReader};
use tam_rs::{errors::*, TamEmulator};

fn code_from_file(filename: &str) -> std::io::Result<Vec<u32>> {
    let file = File::open(filename)?;
    let mut file_reader = BufReader::new(file);

    let mut code = Vec::new();
    loop {
        match file_reader.read_u32::<BE>() {
            Ok(c) => code.push(c),
            Err(_) => break,
        }
    }

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
