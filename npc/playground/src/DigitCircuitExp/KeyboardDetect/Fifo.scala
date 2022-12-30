package KeyboardDetect
import chisel3._
import chisel3.util._

class FifoIO[T<:Data](gen: T) extends Bundle {
    val enq = Flipped(new DecoupledIO(gen))
    val deq = new DecoupledIO(gen)
}

abstract class Fifo[T<:Data](gen: T, depth: Int) extends Module {
    val io = IO(new FifoIO(gen))
    assert(depth > 0, "Fifo depth need to larger than 0")
}

class RegFifo[T<:Data](gen: T, depth: Int) extends Fifo(gen, depth) {
    def counter(incr: Bool) = {
        val cntReg = RegInit(0.U(log2Ceil(depth).W))
        val nextval = Mux(cntReg===(depth-1).U, 0.U, cntReg + 1.U)
        when (incr) {
            cntReg := nextval
        }
        (cntReg, nextval)
    }
    val regFile = Reg(Vec(depth, gen))

    val incrRead = WireDefault(false.B)
    val incrWrite = WireDefault(false.B)
    val isFull = RegInit(false.B)
    val isEpty = RegInit(true.B)

    val (readPtr, nextRead) = counter(incrRead)
    val (writePtr, nextWrite) = counter(incrWrite)

    when (io.enq.valid && !isFull) {
        regFile(writePtr) := io.enq.bits
        isFull := nextWrite === readPtr
        isEpty := false.B
        incrWrite := true.B
    }

    when (io.deq.ready && !isEpty) {
        isEpty := nextRead === writePtr
        isFull := false.B
        incrRead := true.B
    }

    io.deq.bits := regFile(readPtr)
    io.enq.ready := !isFull
    io.deq.valid := !isEpty
}
