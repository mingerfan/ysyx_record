package RegAndMemory
import chisel3._
import chisel3.util._
import math._

class top extends Module {
    val io = IO(new Bundle {
        val addr = Input(UInt(4.W))
        val in = Input(UInt(4.W))
        val we = Input(Bool())
        val out = Output(UInt(4.W))
    })
    val max_num = 16
    val data = new Array[Int](max_num)
    for (i <- 0 until max_num) {
        data(i) = i.toInt
    }
    val regs = VecInit(data.map(_.U(4.W)))
    when (io.we){
        regs(io.addr) := io.in
    }
    io.out := regs(io.addr)
}