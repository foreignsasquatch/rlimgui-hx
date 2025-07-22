import raylib.Types.RenderTextureImpl;
import raylib.Types.RectangleImpl;
import cpp.ConstStar;
import raylib.Types.TextureImpl;

@:buildXml('<include name="${haxelib:rlimgui-hx}/build.xml" />')
@:include("rlImGui.h")
extern class RlImGui {
    @:native("rlImGuiSetup") static function rlImGuiSetup(darkTheme:Bool):Void;
    @:native("rlImGuiBegin") static function rlImGuiBegin():Void;
    @:native("rlImGuiEnd") static function rlImGuiEnd():Void;
    @:native("rlImGuiShutdown") static function rlImGuiShutdown():Void;

    @:native("rlImGuiBeginInitImGui") static function rlImGuiBeginInitImGui():Void;
    @:native("rlImGuiEndInitImGui") static function rlImGuiEndInitImGui():Void;
    @:native("rlImGuiReloadFonts") static function rlImGuiReloadFonts():Void;
    @:native("rlSetupFontAwesome") static function rlSetupFontAwesome():Void;

    @:native("rlImGuiImage") static function rlImGuiImage(image:cpp.ConstStar<TextureImpl>):Void;
    @:native("rlImGuiImageButton") static function rlImGuiImageButton(image:ConstStar<TextureImpl>):Bool;
    @:native("rlImGuiImageSize") static function rlImGuiImageSize(image:ConstStar<TextureImpl>, width:Int, height:Int):Void;
    @:native("rlImGuiImageRect") static function rlImGuiImageRect(image:ConstStar<TextureImpl>, destWidth:Int, destHeight:Int, sourceRect:RectangleImpl):Bool;
    @:native("rlImGuiImageRenderTexture") static function rlImGuiImageRenderTexture(image:ConstStar<RenderTextureImpl>):Void;
    @:native("rlImGuiImageRenderTextureFit") static function rlImGuiImageRenderTextureFit(image:ConstStar<RenderTextureImpl>, center:Bool):Void;
}