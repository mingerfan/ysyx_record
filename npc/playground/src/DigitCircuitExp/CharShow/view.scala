package CharShow
import chisel3._
import chisel3.util._
import DDS_HomeWork._

class View extends Module {
    val io = IO(new Bundle {
        val hAddr = Input(UInt(10.W))
        val vAddr = Input(UInt(10.W))
        val cursorIndex = Input(UInt(11.W))
        val charIndex = Output(UInt(11.W))
        val charData = Input(UInt(8.W))
        val vgaData = Output(UInt(24.W))
    })

    val curPosX = RegInit(0.U(10.W))
    val curPosY = RegInit(0.U(10.W))


    val hGap = 4
    val vGap = 4

    val xWidth = hGap + 9 // 13 now
    val yLength = vGap + 16 // 20 now

    // 49 in x, 24 in y
    val curIndex = RegInit(0.U(11.W))
    val curAscii = RegInit(0.U(8.W))
    val curCharRow = RegInit(0.U(4.W))
    val curCharColumn = RegInit(0.U(4.W))

    val row = 24
    val column = 49

    val max_x = column * xWidth
    val max_y = row * yLength

    val nextPosX = WireDefault(Mux(curPosX + xWidth.U >= max_x.U, 0.U, curPosX + xWidth.U))
    val nextPosY = WireDefault(Mux(curPosY + yLength.U >= max_y.U, 0.U, curPosY + yLength.U))

    when (io.hAddr === nextPosX) {
        curPosX := nextPosX
    }
    when (io.vAddr === nextPosY) {
        curPosY := nextPosY
    }

    when (io.hAddr === nextPosX && curIndex >= (row * column - 1).U) {
        curIndex := 0.U
    } .elsewhen (io.hAddr === nextPosX) {
        curIndex := curIndex + 1.U
    }
    io.charIndex := curIndex + 1.U
    
    when (io.hAddr === nextPosX) {
        curAscii := io.charData
    }
    
    when (io.vAddr >= curPosY + vGap.U) {
        curCharRow := curCharRow + 1.U
    } .otherwise {
        curCharRow := 0.U
    }

    when (io.hAddr >= curPosX + hGap.U) {
        curCharColumn := curCharColumn + 1.U
    } .otherwise {
        curCharColumn := 0.U
    }

    val mAddrStart = WireDefault(curAscii << 4.U) // ascii * 16
    val mAddr = WireDefault(mAddrStart + curCharRow)
    val mData = Wire(UInt(9.W))

    val ModMem = Module(new MemRom(13, 12, "/home/xs/ysyx/ysyx-workbench/npc/playground/resource/vga_font.hex"))
    ModMem.io.addr := mAddr
    mData := ModMem.io.out


    when (io.hAddr >= curPosX + hGap.U) {
        io.vgaData := "hFFFFFF".U
        // when (curIndex === io.cursorIndex) {
        //     io.vgaData := "hFFFFFF".U
        // } .otherwise {
        //     io.vgaData := mData(curCharColumn)
        // }
    } .otherwise {
        io.vgaData := 0.U
    }
}