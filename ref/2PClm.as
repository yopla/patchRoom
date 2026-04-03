/**
 * Copyright phi16 ( http://wonderfl.net/user/phi16 )
 * MIT License ( http://www.opensource.org/licenses/mit-license.php )
 * Downloaded from: http://wonderfl.net/c/2PClm
 */

// forked from phi16's Template
package {
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.MouseEvent;
    import flash.events.KeyboardEvent;
    import flash.ui.Keyboard;
    public class LifeGame extends Sprite {
        public var spr:Sprite=new Sprite();
        public var table:Array=new Array(),stp:Array=new Array();
        public var sz:Number=40;
        public var w:Number=465/sz;
        public var dels:Array=new Array();
        public var vx:Array=new Array(-1,0,1,-1,1,-1,0,1);
        public var vy:Array=new Array(-1,-1,-1,0,0,1,1,1);
        public var gliderGun:Array=new Array(
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,
            0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        public var i:int,j:int,tim:int;
        public var dragging:Boolean=false,pushing:Boolean=false,drCol:Boolean;
        public var period:int=10;
        public function LifeGame(){
            this.addChild(spr);
            stage.addEventListener(KeyboardEvent.KEY_DOWN,kDown);
            stage.addEventListener(KeyboardEvent.KEY_UP,kUp);
            this.addEventListener(MouseEvent.MOUSE_DOWN,mDown);
            this.addEventListener(MouseEvent.MOUSE_UP,mUp);
            this.addEventListener(Event.ENTER_FRAME,frame);
            for(var i:int=0;i<sz;i++){
                table[i]=new Array();
                stp[i]=new Array();
                for(j=0;j<sz;j++){
                    if(Math.random()<0.5)table[i][j]=new Cell(i,j,w);
                }
            }
            tim=0;
        }
        public function mDown(e:MouseEvent):void{
            dragging=true;
            var x:int=mouseX/w,y:int=mouseY/w;
            if(x>=0 && x<sz && y>=0 && y<sz){
                drCol=table[x][y]==null;
            }else drCol=true;
        }
        public function mUp(e:MouseEvent):void{
            dragging=false;
        }
        public function kDown(e:KeyboardEvent):void{
            var c:int=e.keyCode;
            if(c==Keyboard.SPACE)pushing=true;
            if(c==Keyboard.ESCAPE){
                for(i=0;i<sz;i++){
                    for(j=0;j<sz;j++){
                        if(table[i][j]!=null){
                            table[i][j].del();
                            dels.push(table[i][j]);
                            table[i][j]=null; 
                        }
                    }
                }
            }
            if(c==Keyboard.ENTER){
                for(i=0;i<sz;i++){
                    for(j=0;j<sz;j++){
                        if(table[i][j]!=null){
                            table[i][j].del();
                            dels.push(table[i][j]);
                            table[i][j]=null;
                        }
                    }
                }
                for(i=0;i<sz;i++){
                    for(j=0;j<11;j++){
                        if(gliderGun[i+sz*j]==1){
                            table[i][j+5]=new Cell(i,j+5,w);
                        }
                    }
                }
            }
        }
        public function kUp(e:KeyboardEvent):void{
            var c:int=e.keyCode;
            if(c==Keyboard.SPACE)pushing=false;
        }
        public function frame(e:Event):void{
            spr.graphics.clear();
            if(pushing || dragging)spr.graphics.beginFill(0x333333);
            else spr.graphics.beginFill(0);
            spr.graphics.drawRect(0,0,465,465);
            for(i=0;i<sz;i++){
                for(j=0;j<sz;j++){
                    if(table[i][j]!=null){
                        spr.graphics.beginFill(0xffffff);
                        table[i][j].draw(spr);
                        spr.graphics.endFill();
                        table[i][j].step();
                    }
                }
            }
            for(i=0;i<dels.length;i++){
                spr.graphics.beginFill(0xffffff);
                dels[i].draw(spr);
                spr.graphics.endFill();
                dels[i].step();
                if(dels[i].vanished){
                    dels.splice(i,1);
                    i--;
                }
            }
            if(dragging){
                var x:int=mouseX/w,y:int=mouseY/w;
                if(x>=0 && x<sz && y>=0 && y<sz){
                    if(drCol){
                        if(table[x][y]==null){
                            table[x][y]=new Cell(x,y,w);
                        }
                    }else{
                        if(table[x][y]!=null){
                            table[x][y].del();
                            dels.push(table[x][y]);
                            table[x][y]=null;
                        }
                    }
                }       
            }

            if(tim>period && !pushing && !dragging){
                tim=0;
                for(i=0;i<sz;i++){
                    for(j=0;j<sz;j++){
                        stp[i][j]=0;
                    }
                }
                for(i=0;i<sz;i++){
                    for(j=0;j<sz;j++){
                        if(table[i][j]!=null){
                            for(var k:int=0;k<8;k++){
                                if(i+vx[k]>=0 && i+vx[k]<sz && j+vy[k]>=0 && j+vy[k]<sz){                             
                                    stp[i+vx[k]][j+vy[k]]++;
                                }
                            }
                        }
                    }
                }
                var adj:Array=new Array();
                for(i=0;i<sz*sz*2;i++){
                    adj[i]=new Array();
                }
                var next:Array=new Array();
                for(i=0;i<sz;i++){
                    next[i]=new Array();
                    for(j=0;j<sz;j++){
                        if(table[i][j]!=null){
                            if(stp[i][j]!=2 && stp[i][j]!=3){
                                for(k=0;k<8;k++){
                                    var p:int=i+vx[k],q:int=j+vy[k];
                                    if(p>=0 && p<sz && q>=0 && q<sz){
                                        if(table[p][q]==null){
                                            if(stp[p][q]==3){
                                                adj[i*sz+j].push(p*sz+q+sz*sz);
                                                adj[p*sz+q+sz*sz].push(i*sz+j);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                var match:Array=maxFlow(adj,sz*sz);
                for(i=0;i<sz;i++){
                    for(j=0;j<sz;j++){
                        if(table[i][j]!=null && (stp[i][j]==2 || stp[i][j]==3)){
                            next[i][j]=table[i][j];
                        }else{    
                            if(match[i*sz+j]!=null){
                                var eu:int=match[i*sz+j]-sz*sz,ex:int=eu/sz,ey:int=eu%sz;
                                next[ex][ey]=new Cell(0,0,0);
                                next[ex][ey]=table[i][j];
                                next[ex][ey].move(ex,ey);
                            }else if(table[i][j]!=null){
                                dels.push(table[i][j]);
                                dels[dels.length-1].del();
                            }
                            if(table[i][j]!=null){
                                if(stp[i][j]==2 || stp[i][j]==3){
                                    if(match[i*sz+j+sz*sz]==null){
                                        next[i][j]=new Cell(i,j,w);
                                    }
                                }
                            }else{
                                if(stp[i][j]==3){
                                    if(match[i*sz+j+sz*sz]==null){
                                       next[i][j]=new Cell(i,j,w);
                                    }
                                }
                            }
                        }
                    }
                }
                table=next;
            }
            tim++;
        }
    }
}

import flash.display.Sprite;

class Cell {
    public var x:Number,y:Number,size:Number,width:Number;
    public var xTo:Number,yTo:Number,sTo:Number;
    public var vaCnt:int=0,vanishing:Boolean,vanished:Boolean;
    public function Cell(x_:Number,y_:Number,w:Number){
        x=x_,y=y_,size=0,width=w;
        xTo=x_,yTo=y_,sTo=w;
        vanishing=false;
    }
    public function draw(spr:Sprite):void{
        spr.graphics.drawRoundRect(x*width+1+(width-size)/2,y*width+1+(width-size)/2,size-2,size-2,3);
    }
    public function step():void{
        x+=(xTo-x)/3.0;
        y+=(yTo-y)/3.0;
        size+=(sTo-size)/4.0;
        if(vanishing){
            vaCnt++;
            if(size<1)vanished=true;
        }
    }
    public function del():void{
        vanishing=true;
        sTo=0;
    }
    public function move(x_:Number,y_:Number):void{
        xTo=x_,yTo=y_;
    }
}

function dfs(adj:Array,idx:int,match:Array,used:Array,dif:int):Boolean{
    used[idx]=true;
    var i:int,u:int,w:int;
    for(i=0;i<adj[idx].length;i++){
        u=adj[idx][i],w=match[u];
        if(u==idx+dif){        
            if(match[u]==null || used[w]==null && dfs(adj,w,match,used,dif)){
                match[u]=idx;
                match[idx]=u;
                return true;
            }
        }
    }
    for(i=0;i<adj[idx].length;i++){
        u=adj[idx][i],w=match[u];
        if(u!=idx+dif){
            if(match[u]==null || used[w]==null && dfs(adj,w,match,used,dif)){
                match[u]=idx;
                match[idx]=u;
                return true;
            }
        }
    }
    return false;
}

function maxFlow(adj:Array,lu:int):Array{
    var match:Array=new Array();
    for(var v:int=0;v<lu;v++){
        if(match[v]==null){
            var u:Array=new Array();
            dfs(adj,v,match,u,lu);
        }
    }
    return match;
}
