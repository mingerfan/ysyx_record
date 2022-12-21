package HomeWorkHDL
import chisel3._
import chisel3.util._

class top extends Module {
    val io = IO(new Bundle {
        val led = Output(UInt(16.W))
    })
    val cnt = RegInit(0.U(32.W))
    cnt := cnt + 1.U
    val ledReg = RegInit("h01".U(16.W))
    when (cnt === 0.U) {
        ledReg := Cat(ledReg(14,0), ledReg(15))
    }
    io.led := ledReg
}

// object TopMain extends App {
//     emitVerilog(new top, Array("--target-dir", "generated"))
// }