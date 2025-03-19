use tam_rs::TamEmulator;

// ; Add two numbers and print the result
// LOADL(10)
// LOADL(-20)
// CALL(8) 8[PB]
// CALL(8) 26[PB]
// CALL(8) 24[PB]
// HALT

#[test]
fn run_simple_tests() {
    let mut emulator = TamEmulator::new(false);
    if let Err(e) = emulator.set_code(&[
        0x3000000a, 0x3000ffec, 0x62080008, 0x6208001a, 0x62080008, 0xf0000000,
    ]) {
        panic!("failed to set code: {:?}", e);
    }

    let mut input = std::io::stdin().lock();
    let mut output: Vec<u8> = Vec::new();

    loop {
        let instr = emulator.fetch_decode();
        if let Err(e) = instr {
            panic!("fetch_decode failed with error: {:?}", e);
        }

        match emulator.execute(instr.unwrap(), &mut input, &mut output) {
            Ok(true) => (),
            Ok(false) => break,
            Err(e) => panic!("execution failed with error: {:?}", e),
        }
    }

    let output_txt = String::from_utf8(output).expect("output was not valid utf8");
    assert_eq!("-10\n", output_txt);
}
