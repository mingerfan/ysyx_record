package DCE_Encoder
import chisel3._
import chisel3.util._

class top extends Module {
    val io = IO(new Bundle {
        val sw = Input(UInt(9.W))
        val led = Output(UInt(4.W))
        val seg = Output(UInt(7.W))
        val unused = Output(UInt(2.W))
    })

    // 为了防止verilator仿真报UNUSED，添加时序逻辑
    val un_reg = RegInit(2.U)
    un_reg := un_reg + 1.U
    io.unused := un_reg
    val seg_wire = Wire()
    val enc_in = Wire(UInt(8.W))
    enc_in := io.sw(7, 0)

    var last_mux = 0.U(3.W)
    for (i <- 0 until 8) {
        last_mux = Mux(enc_in(i)===1.U, i.U, last_mux)
    }
    val enc_result = Mux(io.sw(8), last_mux, 0.U)
    io.led := Cat(io.sw(8), enc_result)

    io.seg := seg_wire

    seg_wire := 0.U
    switch (enc_result) {
        is (0.U) { seg_wire := "b011_1111".U }
        is (1.U) { seg_wire := "b000_0011".U }
        is (2.U) { seg_wire := "b101_1011".U }
        is (3.U) { seg_wire := "b100_1111".U }
        is (4.U) { seg_wire := "b110_0110".U }
        is (5.U) { seg_wire := "b110_1101".U }
        is (6.U) { seg_wire := "b111_1101".U }
        is (7.U) { seg_wire := "b000_0111".U }
        is (8.U) { seg_wire := "b111_1111".U }
        is (9.U) { seg_wire := "b110_1111".U }
    }
}