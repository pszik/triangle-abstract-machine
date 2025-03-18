use tam_rs;

fn main() {
    let instr = tam_rs::instructions::TamInstruction::from(0x12345678);
    println!("{:?}", instr);
}
