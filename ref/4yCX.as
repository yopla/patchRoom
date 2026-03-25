/**
 * Copyright bibibyuu ( http://wonderfl.net/user/bibibyuu )
 * MIT License ( http://www.opensource.org/licenses/mit-license.php )
 * Downloaded from: http://wonderfl.net/c/4yCX
 */

// forked from uwi's forked from: 少し凝ったAI + SnakeGame
// forked from uwi's 少し凝ったAI + SnakeGame
// forked from bkzen's 簡易AI + SnakeGame
// forked from bkzen's SnakeGame
package  
{
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.KeyboardEvent;
    import flash.geom.Rectangle;
    import flash.text.TextField;

    // とりあえず、途中の経路で蛇が通れない箇所をつくらないように
    // つくろうとおもう
    /**
     * Snake Game とりあえず100行以内で書きたかった。
     * おバカなAI追加
     */
    [SWF (backgroundColor = "0xFFFFFF", frameRate = "60", width = "465", height = "465")]
    public class SnakeGame extends Sprite
    {
        // bmp, bmd 描画されるところ。
        private var bmp: Bitmap, bmd: BitmapData;
        // key は今から進もうと思っている方向, way は今進んでいる方向。
        private var key: uint, way: uint;
        // snake は ヘビの配列、[0] が先頭。 point はヘビが取っていくドット。
        private var snake: Array, point: Snake;
        // 描画領域
        private var viewRect: Rectangle;
        // speedK は スピード調整用の係数。 1 ～ N (小さいほど早い)、 speedC は wait 用のカウンタ(いじる必要なす)、 snakeLength は ヘビの長さ。(1～)
        private var speedK: int = 1, speedC: int = 0, snakeLength: int = 1;
       
        public function SnakeGame()
        {
            if (stage) init();
            else addEventListener(Event.ADDED_TO_STAGE, init);
        }
       
        private function init(e: Event = null): void
        {
            removeEventListener(Event.ADDED_TO_STAGE, init);
            //
            addChild(bmp = new Bitmap(bmd = new BitmapData(stageW, stageW, false, 0)));
            bmp.scaleX = bmp.scaleY = cellSize;
            bmd.fillRect(viewRect = new Rectangle(1, 1, stageW2, stageW2), 0xFFFFFF);
            snake = [new Snake(stageW >> 1, stageW >> 1)], point = new Snake();
            //stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown); // AI を作るときはこの行をコメントアウト。
            addEventListener(Event.ENTER_FRAME, loop);
            
            _tf = new TextField();
            addChild(_tf);
            _tf.textColor = 0xff0000;
            _tf.width = 200;
            _tf.height = 565;
        }
        
        private var _tf : TextField;
        
        /**
         * 壁が消えていくのに一応対応しているが、
         * すでに確定してしまったセルには戻れないため、
         * どこかで暇を潰したあと到達できるような経路が描けない
         */
        private function AStar() : Array
        {
            var map : Array = new Array(stageW * stageW);
            var i : uint, j : uint;
            // マップぬりぬり
            for(j = 0;j < stageW;j++){
                    for(i = 0;i < stageW;i++){
                        map[j*stageW+i] = new Cell(i, j);
                    }
            }
            for(i = 0;i < stageW;i++){
                    map[i].wall = -1;
                    map[i * stageW].wall = -1;
                    map[(stageW - 1) * stageW + i].wall = -1;
                    map[i * stageW + stageW - 1].wall = -1;
            }
            for(i = 1;i < snake.length;i++){
                    map[snake[i].y * stageW + snake[i].x].wall = i;
            }
            map[point.y * stageW + point.x].wall = -2;
            
            // 起点のセルをセット
            var start : uint = snake[0].y * stageW + snake[0].x;
            map[start].d = 0;
            map[start].score = 0;
            var unsolved : Object = {};
            unsolved[start] = map[start];
            _tf.appendText("" + point.x + "\t" + point.y + "\n");
            _tf.scrollV = _tf.maxScrollV; 
            
            // DPで探索
            var c : Cell;
            for(;;){
                    // スコアが最小(最もゴールに近い点)を選ぶ
                    var minScore : Number = Number.MAX_VALUE;
                    var minInd : int = -1;
                    for(var key : String in unsolved){
                        if(unsolved[key].score < minScore){
                            minScore = unsolved[key].score;
                            minInd = int(key);
                        }
                    }
                    if(minInd == -1){
                        return null;
                    }
                    delete unsolved[minInd];
                    
                    c = map[minInd];
                    if(c.wall == -2)break;
                    
                    // 直前からの移動・・曲がりの取得のためにいれたけど
                    // もう使わないかな
                    var prevDelta : int = 0;
                    if(c.prev != null){
                        prevDelta = minInd - (c.prev.y * stageW + c.prev.x);
                    }
                    
                    // 新たに調べるべきセルを追加。
                    // 選ばれたセルの上下左右について
                    for each(var delta : int in [-1, 1, -stageW, stageW]){
                        var ind : int = minInd + delta;
                        var nc : Cell = map[ind];
                        // 壁もしくは設定済みのセルならスルー
                        if(nc.wall == -1 || nc.d > -1)continue;
                        if(nc.wall > 0 && nc.wall + c.d < snake.length)continue;
                        
                        /*
                        var nwall : int = 0; 
                        for each(var dd : int in [-1, 1, -stageW, stageW, -stageW-1, -stageW+1, stageW-1, stageW+1]){
                            var indd : int = ind + dd;
                            var ncd : Cell = map[indd];
                               if(ncd.wall == -1){ nwall++; continue; }
                            if(ncd.wall > 1){nwall++; continue; }
                        }
                        */
                        // でもへんだな、それだとここにひっかかるはずなのに・・
                        // だめぽおおおおお　もうちょっとしっかり考えないとだめだ。
                        if(!isConnectedSpace(nc, c, snake, point))continue;
                        
                        var d : int = c.d + 1;
                        var score : Number = Math.sqrt((nc.x - point.x) * (nc.x - point.x) + (nc.y - point.y) * (nc.y - point.y));
//                        if(score < nc.score){
                            nc.score = score;
                            nc.d = d;
                            nc.prev = c;
//                        }
                        unsolved[ind] = nc;
                    }
            }
            
            var way : Array = [];
            for(;c.prev != null;c = c.prev){
                    way.push(c);
            }
            way.reverse();
            return way;
        }
        
            // cとそれ以前snake.length個でできる蛇と壁によって
            // できる白い空間は連結かどうか判定する
        private function isConnectedSpace(head : Cell, prev : Cell, snake : Array, goal : Snake) : Boolean
        {
                var bmd : BitmapData = new BitmapData(31, 31, false, 0x000000);
                var i : int, j : int
                var c : Cell;
                bmd.lock();
                bmd.fillRect(new Rectangle(1, 1, 31-2, 31-2), 0xffffff);
                
                bmd.setPixel(head.x, head.y, 0x000000);
                for(i = 0, c = prev;i < snake.length;i++, c = c.prev){
                    if(c == null){
                        for(j = 0;j < snake.length - i;j++){
                            bmd.setPixel(snake[j].x, snake[j].y, 0x000000);
                        }
                        break;
                    }                    
                    bmd.setPixel(c.x, c.y, 0x000000);
                }
                /*
                bmd.setPixel(goal.x, goal.y, 0x000000);
                bmd.setPixel(goal.x-1, goal.y, 0x000000);
                bmd.setPixel(goal.x+1, goal.y, 0x000000);
                bmd.setPixel(goal.x, goal.y-1, 0x000000);
                bmd.setPixel(goal.x, goal.y+1, 0x000000);
                */
                
                // 白い点を見つける
                outer:
                for(i = 1;i < 31-1;i++){
                    for(j = 1;j < 31-1;j++){
                        if(bmd.getPixel(i, j) == 0xffffff)break outer;
                    }
                }
                
                bmd.floodFill(i, j, 1); // ぬりぬり
                
                var rect : Rectangle = bmd.getColorBoundsRect(0xffffff, 0xffffff);
                bmd.unlock();
                bmd.dispose();
                return rect.isEmpty();
        }
        
        private function onKeyDown(e: KeyboardEvent ): void { changeWay(e.keyCode); }
        /**
         * 方向変更
         * @param    keyCode
         * @return    <Boolean> : 方向変更可能だったら true
         */
        private function changeWay(keyCode: uint): Boolean
        {
            var b: Boolean;
            switch (keyCode)
            {
                case UP:       if (b = (way != 1 << 1)) key = 1 << 0; break;
                case DOWN:     if (b = (way != 1 << 0)) key = 1 << 1; break;
                case LEFT:     if (b = (way != 1 << 3)) key = 1 << 2; break;
                case RIGHT:    if (b = (way != 1 << 2)) key = 1 << 3; break;
            }
            return b;
        }
        
        private var optWay : Array = null;
        private var p : uint = 0;
        
        private function loop(e: Event ): void
        {
            if (speedC++ % speedK == 0)
            {
                var s: Snake = snake[0], i: int, c: uint, tx: int, ty: int;
                var vx : int = 0, vy : int = 0;
                if(optWay != null){
                   vx = optWay[p].x - optWay[p].prev.x;
                    vy = optWay[p].y - optWay[p].prev.y;
                    p++;
                }
                tx = s.x, ty = s.y;
                s.x += vx; s.y += vy;
                c = bmd.getPixel(s.x, s.y);
                bmd.lock();
                bmd.fillRect(viewRect, 0xFFFFFF);
                bmd.setPixel(s.x, s.y, 1);
                for (i = 1; i < snakeLength; i++)
                {
                    s = snake[i];
                    vx = s.x, vy = s.y;
                    bmd.setPixel(s.x = tx, s.y = ty, 0);
                    tx = vx, ty = vy;
                } 
                bmd.setPixel(point.x, point.y, 0xFF0000);
                bmd.unlock();
                
                if(optWay == null || c == 0xff0000){
                        snakeLength = snake.push(point);
                        var pindList : Array = [];
                        for(i = 0;i < stageW * stageW;i++){
                            if(bmd.getPixel(i%stageW,i/stageW) == 0xffffff){
                                if(Math.abs(point.x - (i%stageW)) + Math.abs(point.y - uint(i/stageW)) > 1){
                                    pindList.push(i);
                                } 
                            }
                        }
                        var rind : int = pindList[int(Math.random() * pindList.length)];
                        point = new Snake(rind%stageW,rind/stageW);
                        speedK = speedK < 2 ? 1 : speedK - 1;
                        optWay = AStar();
                        if(optWay == null){
                            removeEventListener(Event.ENTER_FRAME, loop);
                            return;
                        }
                        p = 0;
                }else if (c == 0) { 
                        removeEventListener(Event.ENTER_FRAME, loop);
                        return;
                    }
            } 
        }
    }
}

const stageW:    int = 31;            // stage のサイズ
const stageW2:   int = stageW - 2;    // stage のサイズから、枠を除いた数
const cellSize:  int = 15;            // 一つ一つのセルのサイズ (px)
const UP:        uint = 38;           // AI が changeWay する時用の 上 Key のキーコード
const DOWN:      uint = 40;           // AI が changeWay する時用の 下 Key のキーコード
const LEFT:      uint = 37;           // AI が changeWay する時用の 左 Key のキーコード
const RIGHT:     uint = 39;           // AI が changeWay する時用の 右 Key のキーコード

class Snake
{
    public var x: int, y: int;
    function Snake(x_: int = 0, y_: int = 0)
    {
        x = x_ || Math.random() * stageW2 + 1, y = y_ || Math.random() * stageW2 + 1;
    }
}

class Cell
{
    public var wall:int;
    public var x:int;
    public var y:int;
    public var d:int;
    public var prev:Cell;
    public var score:Number;
    
    public function Cell(x : int, y : int)
    {
        this.x = x;
        this.y = y;
        this.d = -1;
        this.wall = 0;
        this.prev = null;
        this.score = Number.MAX_VALUE;
    }
}