// psyark's life - interactive now :)
package {
    import flash.display.*;
    import flash.events.*;
    import flash.filters.ConvolutionFilter;
    import flash.geom.Rectangle;
    [SWF(width=465,height=465,frameRate=120,backgroundColor=0x000000)]
    public class ParallelLife extends Sprite {
        private var rrect:Rectangle;
        private var field:BitmapData;
        public function ParallelLife() {
            rrect = new Rectangle;
            field = new BitmapData(465, 465, false, 0);
            var step:ConvolutionFilter = new ConvolutionFilter(3, 3, [1, 1, 1, 1, 9, 1, 1, 1, 1], 0xFF, 0);
            var ruleR:Array = [0, 0, 0, 0xFF0000, 0, 0, 0, 0, 0, 0, 0, 0xFF0000, 0xFF0000];
            var ruleG:Array = [0, 0, 0, 0x00FF00, 0, 0, 0, 0, 0, 0, 0, 0x00FF00, 0x00FF00];
            var ruleB:Array = [0, 0, 0, 0x0000FF, 0, 0, 0, 0, 0, 0, 0, 0x0000FF, 0x0000FF];
            // field.noise(Math.random() * 0x100, 0, 255, 7);
            addChild(new Bitmap(field));
            addEventListener(Event.ENTER_FRAME, function (event:Event):void {
                field.applyFilter(field, field.rect, field.rect.topLeft, step);
                field.paletteMap(field, field.rect, field.rect.topLeft, ruleR, ruleG, ruleB, null);
            });

            stage.addEventListener(MouseEvent.MOUSE_MOVE, function (e:MouseEvent):void {
                if (e.buttonDown) {
                    // draw (shift: erase) random rectangle
                    var side:Number = e.shiftKey ? 50 : 10;
                    rrect.x = e.stageX - side * Math.random ();
                    rrect.y = e.stageY - side * Math.random ();
                    rrect.width  = side * (1 + Math.random ());
                    rrect.height = side * (1 + Math.random ());
                    field.fillRect (rrect,
                        (e.shiftKey) ? 0 : int (0xffffff * Math.random ()));
                }
            });
        }
    }
}
