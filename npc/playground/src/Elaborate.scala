package gcd
import chisel3._

object Elaborate extends App {
  def top = new GCD()
  emitVerilog(top, Array("--target-dir", "./build"))
}
