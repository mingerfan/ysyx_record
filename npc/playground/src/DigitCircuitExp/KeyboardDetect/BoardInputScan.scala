package KeyboardDetect
import chisel3._
import chisel3.util._

class BoardInputScan extends Module {
    val io = IO(new Bundle {
        val cmdOut = new DecoupledIO(UInt(8.W))
        val ps2_clk = Input(Bool())
        val ps2_data = Input(Bool())
    })

    val ps2_clk_sync = Reg(UInt(3.W))
    ps2_clk_sync := Cat(ps2_clk_sync(1,0), io.ps2_clk)

    val buffer = RegInit(0.U(10.W))
    val rcvCnt = RegInit(0.U(4.W))

    val sampling = WireDefault(ps2_clk_sync(2) && !ps2_clk_sync(1))
    val fifo = Module(new RegFifo(UInt(8.W), 8))

    fifo.io.deq <> io.cmdOut
    fifo.io.enq.valid := false.B
    fifo.io.enq.bits := 0.U

    when (sampling) {
        when (rcvCnt < 10.U) {
            rcvCnt := rcvCnt + 1.U
            buffer := buffer | io.ps2_data << rcvCnt
        } .otherwise {
            rcvCnt := 0.U
            when (fifo.io.enq.ready && io.ps2_data && buffer(9, 1).xorR) {
                fifo.io.enq.valid := true.B
                fifo.io.enq.bits := buffer(9, 1)
            }
            buffer := 0.U
        }
    }
}