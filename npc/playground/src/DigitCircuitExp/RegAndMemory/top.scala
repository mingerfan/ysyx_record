package RegAndMemory
import chisel3._
import chisel3.util._

class top extends Module {
    val io = IO(new Bundle {
        val addr = Input(UInt(4.W))
        val out = Output(UInt(4.W))
    })

    val regs = RegInit(VecInit(Seq.fill(16)(0.U(4.W))))
    io.out := regs(io.addr)
}