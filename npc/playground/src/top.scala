package HomeWorkHDL
import chisel3._
import chisel3.util._

class top extends Module {
    val io = IO(new Bundle {
        val led = Output(UInt(16.W))
    })

    io.led := 0.U
}

// object TopMain extends App {
//     emitVerilog(new top, Array("--target-dir", "generated"))
// }