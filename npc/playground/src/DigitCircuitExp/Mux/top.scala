package DCE_Mux
import chisel3._
import chisel3.util._

class MuxAbs[T<:Data](dt: T, key_num: Int) extends Module {
    val io = IO(new Bundle {
        val in = Input(Vec(key_num, dt))
        val key = Input(UInt(log2Ceil(key_num).W))
        val out = Output(dt)
    })
    io.out := io.in(io.key)
}

class top extends Module {
    val io = IO(new Bundle {
        val sw = Input(Vec(10, Bool()))
        val led = Output(UInt(2.W))
        val unused = Output(UInt(2.W))
    })
    // 为了防止verilator仿真报UNUSED，添加时序逻辑
    val un_reg = RegInit(2.U)
    un_reg := un_reg + 1.U
    io.unused := un_reg

    val Mux4_1 = Module(new MuxAbs(UInt(2.W), 4))
    Mux4_1.io.key := Cat(io.sw(1), io.sw(0))
    for (i <- 0 until 4) {
        Mux4_1.io.in(i) := Cat(io.sw(i*2+3), io.sw(i*2+2))
    }
    io.led := Mux4_1.io.out
}