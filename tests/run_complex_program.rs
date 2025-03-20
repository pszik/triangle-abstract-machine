use tam_rs::TamEmulator;

// ; Read two numbers from stdin and print their gcd
// PUSH(2)
// LOADA     0[SB]
// CALL(LB)  getint
// LOADA     1[SB]
// CALL(LB)  getint
// LOAD(1)   1[SB]
// JUMPIF(0) 14[CB]
// LOAD(1)   1[SB]
// LOAD(1)   1[SB]
// LOAD(1)   0[SB]
// CALL(LB)  mod
// STORE(1)  1[SB]
// STORE(1)  0[SB]
// JUMP      5[SB]
// LOAD(1)   0[SB]
// CALL(LB)  putint
// CALL(LB)  puteol
// HALT

#[test]
fn run_simple_tests() {
    let mut emulator = TamEmulator::new(false);
    if let Err(e) = emulator.set_code(&[
        0xa0000002, 0x14000000, 0x62080019, 0x14000001, 0x62080090, 0x04010001, 0xe000000e,
        0x04010001, 0x04010001, 0x04010000, 0x6208000c, 0x44010001, 0x44010000, 0xc4000005,
        0x04010000, 0x6208001a, 0x62080018, 0xf0000000,
    ]) {
        panic!("failed to set code: {:?}", e);
    }

    let mut input = std::io::Cursor::new("38\n18\n");
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
    assert_eq!("6\n", output_txt);
}
