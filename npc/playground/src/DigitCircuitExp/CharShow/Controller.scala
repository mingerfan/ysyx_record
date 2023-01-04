package CharShow
import chisel3._
import chisel3.util._
import DDS_HomeWork._

class Controller extends Module {
    val io = IO(new Bundle {
        val backspace = Input(Bool())
        val cursorIndex = Output(UInt(11.W))
        val charIndex = Input(UInt(11.W))
        val charData = Output(UInt(8.W))
        val KeyBoardIn = Input(UInt(8.W))
    })

    val row = 24
    val column = 49

    val ram = Module(new Ram(11, 8))

    val cursorIndex = RegInit(0.U(11.W))
    val counter = RegInit(0.U(11.W))
    val lineCounter = RegInit(0.U(11.W))

    ram.io.readAddr := io.charIndex
    io.charData := ram.io.readData
    ram.io.writeAddr := 1.U
    ram.io.writeData := 0.U
    ram.io.writeEn := false.B

    val lastEnterIndex = Reg(Vec(row, UInt(6.W)))

    when (cursorIndex - counter >= column.U) {
        counter := counter + column.U
    }

    when (cursorIndex - counter >= column.U && cursorIndex < (column * row).U) {
        lineCounter := lineCounter + 1.U
    } .elsewhen (cursorIndex - counter >= column.U && cursorIndex >= (column * row).U) {
        lineCounter := 0.U
    }
    
    when (io.KeyBoardIn =/= 0.U) {
        ram.io.writeEn := true.B
        when (io.KeyBoardIn === '\n'.U) {
            ram.io.writeData := '\n'.U
            cursorIndex := counter + column.U
        } .otherwise {
            ram.io.writeData := io.KeyBoardIn
            cursorIndex := cursorIndex + 1.U
        }
    }

    when (cursorIndex >= (column * row).U) {
        cursorIndex := 0.U
    }

    when (io.KeyBoardIn =/= 0.U) {
        when (io.KeyBoardIn === '\n'.U) {
            lastEnterIndex(lineCounter) := cursorIndex
        } .otherwise {
            when (cursorIndex - 1.U - counter >= column.U) {
                lastEnterIndex(lineCounter) := (column-1).U
            }
        }
    }
    
    when (io.backspace) {
        ram.io.writeEn := true.B
        ram.io.writeAddr := cursorIndex
        ram.io.writeData := 0.U
        when (cursorIndex =/= 0.U) {
            when (cursorIndex === counter) {
                counter := counter - column.U
                lastEnterIndex(lineCounter) := 0.U
                lineCounter := lineCounter - 1.U
                cursorIndex := lastEnterIndex(lineCounter-1.U)
            } .otherwise {
                cursorIndex := cursorIndex - 1.U
            }
        }
    }

    io.cursorIndex := cursorIndex
}