use byteorder::{ReadBytesExt, BE};
use clap::Parser;
use std::{
    fs::File,
    io::{Seek, SeekFrom},
    path::PathBuf,
};
use tam_rs::{errors::*, TamEmulator};

#[derive(Parser)]
#[command(version, about, long_about=None)]
struct CliArgs {
    /// Name of binary file to load.
    #[arg(value_name = "FILE")]
    filename: PathBuf,

    /// If set, print mnemonic of instructions as they are executed.
    #[arg(long)]
    trace: bool,
}

fn code_from_file(filename: &PathBuf) -> std::io::Result<Vec<u32>> {
    let mut file = File::open(filename)?;

    file.seek(SeekFrom::End(0))?;
    let file_len = file.stream_position()?;
    file.rewind()?;

    let mut code = Vec::with_capacity((file_len / 4) as usize);
    file.read_u32_into::<BE>(&mut code)?;
    Ok(code)
}

fn main() -> TamResult<()> {
    let cli = CliArgs::parse();

    let mut emu = TamEmulator::new(cli.trace);
    let code = code_from_file(&cli.filename).map_err(|r| TamError {
        kind: TamErrorKind::IOError,
        address: None,
        message: Some(r.to_string()),
    })?;
    emu.set_code(&code)?;

    let mut reader = std::io::stdin().lock();
    let mut writer = std::io::stdout();

    loop {
        let instr = emu.fetch_decode()?;
        if !emu.execute(instr, &mut reader, &mut writer)? {
            break;
        }
    }

    Ok(())
}
