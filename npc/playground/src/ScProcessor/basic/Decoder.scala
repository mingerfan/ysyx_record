package ScProcessor
package basic
import chisel3._
import chisel3.util._

class BasicDecoder(in_width: Int) extends Module {
    val ports_num = 1<<in_width
    val sel = IO(Input(UInt(in_width.W)))
    val out = IO(Output(UInt(ports_num.W)))

    val vecOut = Wire(Vec(ports_num, Bool()))
    for (i <- 0 until ports_num) {
        vecOut(i) := sel === i.U
    }
    out := vecOut.asUInt
}

object DecGen extends App {
    val s = getVerilogString(new BasicDecoder(2))
    println(s)
}