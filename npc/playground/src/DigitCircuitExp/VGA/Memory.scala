package DDS_HomeWork
import chisel3._
import chisel3.util._
import math._
import chisel3.util.experimental.loadMemoryFromFileInline
import firrtl.annotations.MemoryLoadFileType
import java.io.File

// rom width 8
// DAC 0-255

class RomBundle(addr_width: Int, out_width: Int) extends Bundle {
    val addr = Input(UInt(addr_width.W))
    val out = Output(UInt(out_width.W))
}

class AbsRom(addr_width: Int, out_width: Int) extends Module {
    val io = IO(new RomBundle(addr_width, out_width))
    val max_num = pow(2, addr_width).toInt
    val DAC_max = 255
}

class WireRom(addr_width: Int, out_width: Int) extends AbsRom(addr_width, out_width) {
    val data = new Array[Int](max_num)
    for (i <- 0 until max_num) {
        data(i) = ((sin(2*Pi/max_num*i) + 1)/2 * 255).toInt
        val data1 = data(i)
        // println(s"data[i] = $data1")
    }
    val table = VecInit(data.toSeq.map(_.U(8.W)))
    io.out := RegNext(table(io.addr))
}

class MemRom(addr_width: Int, out_width: Int, init_file: String) extends AbsRom(addr_width, out_width) {
    val mem = SyncReadMem(max_num, UInt(out_width.W))
    loadMemoryFromFileInline(mem, init_file)
    // This prevents deduping this memory module
    // Ref. https://github.com/chipsalliance/firrtl/issues/2168
    val dedupBlock = WireInit(mem.hashCode.U)

    io.out := mem.read(io.addr)
}

class Ram(addr_width: Int, out_width: Int) extends Module {
    val io = IO(new Bundle {
        val readAddr = Input(UInt(addr_width.W))
        val readData = Output(UInt(out_width.W))
        val writeAddr = Input(UInt(addr_width.W))
        val writeData = Input(UInt(out_width.W))
        val writeEn = Input(Bool())
    })
    val mem = SyncReadMem(pow(2, addr_width).toInt, UInt(out_width.W))
    io.readData := mem.read(io.readAddr)

    when (io.writeEn) {
        mem.write(io.writeAddr, io.writeData)
    }
}


object wireMain extends App {
    val s = getVerilogString(new WireRom(8, 8))
    println(s)
}
