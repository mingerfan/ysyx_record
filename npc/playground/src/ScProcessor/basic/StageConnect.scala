package ScProcessor.basic
import ScProcessor.topInfo
import chisel3._
import chisel3.util._

object MyStageConnect {
    val SC_ARCH =  topInfo.ARCH;
    def apply[T <: Data](left: DecoupledIO[T], right: DecoupledIO[T]) = {
        if (SC_ARCH == "single") { 
            right.bits := left.bits 
            right.valid := true.B
        }
        else if (SC_ARCH == "multi") { right <> left }
    }
}
