package reg2d
import chisel3._

class TwoDimRegArray extends Module {
  val io = IO(new Bundle {
    val writeEnable = Input(Bool())
    val rowSelect = Input(UInt(1.W))
    val colSelect = Input(UInt(1.W))
    val writeData = Input(UInt(8.W))
    val readData = Output(UInt(8.W))
  })

  // 创建一个2x2的寄存器数组
  val regArray = VecInit(Seq.fill(8)(VecInit(Seq.fill(8)(Reg(UInt(8.W))))))

  // 根据输入的行列选择信号，写入数据
  when(io.writeEnable) {
    regArray(io.rowSelect)(io.colSelect) := io.writeData
  }

  // 读取选定的行列数据
  io.readData := regArray(io.rowSelect)(io.colSelect)
}

object TwoDimRegArray extends App {
  val s = getVerilogString(new TwoDimRegArray)
  println(s)
}