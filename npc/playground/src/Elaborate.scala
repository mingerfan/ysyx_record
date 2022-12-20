import chisel3._

object Elaborate extends App {
  def top = new HomeWorkHDL.HWTop()
  emitVerilog(top, Array("--target-dir", "./build"))
}
