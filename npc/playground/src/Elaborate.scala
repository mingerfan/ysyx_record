import chisel3._

object Elaborate extends App {
    val top = new HomeWorkHDL.top()
    emitVerilog(top, Array("--target-dir", "build/top/"))
}