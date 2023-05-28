// import Mill dependency
import mill._
import mill.scalalib._
import mill.scalalib.scalafmt.ScalafmtModule
import mill.scalalib.TestModule.ScalaTest
// support BSP
import mill.bsp._

object playground extends ScalaModule with ScalafmtModule { m =>
  override def scalaVersion = "2.13.8"
  override def scalacOptions = Seq(
    "-language:reflectiveCalls",
    "-deprecation",
    "-feature",
    "-Xcheckinit",
    "-P:chiselplugin:genBundleElements"
  )
  override def ivyDeps = Agg(
    ivy"edu.berkeley.cs::chisel3:3.5.4",
    ivy"com.sifive::chisel-circt:0.6.0",
  )
  override def scalacPluginIvyDeps = Agg(
    ivy"edu.berkeley.cs:::chisel3-plugin:3.5.4",
  )
  object tools extends ScalaModule {
    def scalaVersion = "2.13.8"
  }
  object test extends Tests with ScalaTest {
    override def ivyDeps = m.ivyDeps() ++ Agg(
      ivy"org.scalatest::scalatest:3.1.4",
      ivy"edu.berkeley.cs::chiseltest:0.5.4",
    )
  }
}
