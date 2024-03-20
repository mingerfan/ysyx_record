package ScProcessor.basic
import ScProcessor.topInfo
import chisel3._
import chisel3.util._

class StageConnectModule[T <: Data](gen: T) extends Module {
    val io = IO(new Bundle {
        val in = Flipped(DecoupledIO(gen.cloneType))
        val out = DecoupledIO(gen.cloneType)
    })
    MyStageConnect(io.in, io.out)
}

object MyStageConnect {
    val SC_ARCH =  topInfo.ARCH;
    def apply[T <: Data](left: DecoupledIO[T], right: DecoupledIO[T]) = {
        if (SC_ARCH == "single") { 
            right.bits := left.bits 
            right.valid := true.B
            left.ready := true.B
        }
        else if (SC_ARCH == "multi") { right <> left }
    }

    def apply[T <: Data](left: DecoupledIO[T], right: DecoupledIO[T], module_name: String) = {
        val stage_connect = Module(new StageConnectModule(left.bits))
        stage_connect.suggestName(module_name)
        stage_connect.io.in <> left
        stage_connect.io.out <> right
    }
}
