import circt.stage._

object Elaborate extends App {
    def top = new ScProcessor.top
    val generator = Seq(chisel3.stage.ChiselGeneratorAnnotation(() => top))
    (new ChiselStage).execute(args, generator)
}