/**
 * Copyright mutantleg ( http://wonderfl.net/user/mutantleg )
 * MIT License ( http://www.opensource.org/licenses/mit-license.php )
 * Downloaded from: http://wonderfl.net/c/4JAS
 */

package {
    import flash.events.MouseEvent;
    import flash.events.Event;
    import flash.display.Sprite;
    import flash.display.DisplayObject;
    import flash.geom.Point;
  
    public class FlashTest extends Sprite {
        public var lesMurs:Vector.<truc> = new Vector.<truc>
         public var lesSrc:Vector.<srcSpawn> = new Vector.<srcSpawn>
        public var lesScores:Vector.<sc> = new Vector.<sc>
        public function FlashTest() {

         ///var swap:DisplayObject
                for( var j:int = this.numChildren - 1; j>=0; j-- ) {
                    //trace ("popo")
                    if( this.getChildAt(j) is truc ) {
                        lesMurs.push(this.getChildAt(j));
                    }
                     if( this.getChildAt(j) is sc ) {
                        //trace (this.getChildAt(j).name)
                        lesScores.push(this.getChildAt(j));
                    }
                    if( this.getChildAt(j) is srcSpawn ) {
                        //trace (this.getChildAt(j).name)
                        lesSrc.push(this.getChildAt(j));
                        removeChild(getChildAt(j));

                    }
                      if( this.getChildAt(j) is bg ) {
                        //swap = this.getChildAt(j)
                        //trace (this.getChildAt(j).name)
                       // lesSrc.push(this.getChildAt(j));
                       // removeChild(getChildAt(j));
                    }

                    //parentObj.parentObj.removeChild(parentObj.parentObj.getChildAt(j))
                }
                trace (lesMurs.length+ " murs")
                trace (lesScores.length+ " score")
                trace (lesSrc.length+ " sources")


            stage.quality = "low";
            stage.addEventListener(MouseEvent.MOUSE_DOWN, onMdown);
            stage.addEventListener(MouseEvent.MOUSE_UP, onMup); 
            stage.addEventListener(Event.ENTER_FRAME, onEnter);
            bt_maison.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
            bt_redo.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)


 bt_R.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
  bt_J.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
   bt_M.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
    bt_V.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
     bt_C.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)
      bt_B.addEventListener(MouseEvent.MOUSE_UP, clickBTBAse)


            compt3.text="0"
            compt4.text="100000"
            comptAVic.text="0";  comptBVic.text="0" ;  comptCVic.text="0";
            comptDVic.text="0";  comptEVic.text="0" ;  comptFVic.text="0";
            comptATt.text="0";  comptBTt.text="0" ;  comptCTt.text="0";
            comptDTt.text="0";  comptETt.text="0" ;  comptFTt.text="0";

            comptAEff.text="0"
            //swapChildren(swap, this);
            reInit();
        }




    
    var countInit:int=0;
    public var maxi:int = 512;//1280 //128
    var nbBase:int=maxi/50//6;
    public var vecColor:Vector.<uint> = Vector.<uint>([0xFFFFFF, 0xFF0000]);//,0xFF,0x808000,0x008000,0xF000F0]); 
    public function reInit() {
    
        var i:int;

        if (parseInt(compt3.text)<gt) compt3.text = gt.toString();//best time
        if (parseInt(compt4.text)>gt && gt != 0) compt4.text = gt.toString();//petit temps
        //calme = true;
        //durecalme=0;
        gt=0;
        comptAEff.text = (parseInt(comptAEff.text)+1).toString();
       
        vecUnit = new Vector.<xUnit>(0,false);
        for(i=0;i<maxi;i++) {vecUnit.push(new xUnit()); }

        //vecColor = Vector.<uint>([]);//0xFFFF00, 0xFF0000])
        vecColor = Vector.<uint>([0xFFFF00, 0xFF0000,0xFF00FF,0x00FF00,0x00FFFF, 0x0000FF ])

        for (i = 0; i < 16; i++) { 
            var red : int = Math.floor(Math.random()*255)+64;
            var green : int = Math.floor(Math.random()*255)+64;
            var blue : int = Math.floor(Math.random()*255)+64;
            var color : int = red << 16 | green << 8 | blue;
            vecColor.push(color);
            //vecColor.push(Math.random()*0xFFffFFff); 
        }

        comptA.textColor = vecColor[1]; comptA.text="0";
        comptB.textColor = vecColor[0];; comptB.text="0";
        comptC.textColor = vecColor[2];; comptC.text="0";
        comptD.textColor = vecColor[3];; comptD.text="0";
        comptE.textColor = vecColor[4];; comptE.text="0";
        comptF.textColor = vecColor[5];; comptF.text="0";

        comptAPt.textColor = vecColor[1]; comptAPt.text="0";
        comptBPt.textColor = vecColor[0];; comptBPt.text="0";
        comptCPt.textColor = vecColor[2];; comptCPt.text="0";
        comptDPt.textColor = vecColor[3];; comptDPt.text="0";
        comptEPt.textColor = vecColor[4];; comptEPt.text="0";
        comptFPt.textColor = vecColor[5];; comptFPt.text="0";

        countInit=0;

        var letters:Array = new Array();
        trace (nbBase+" chacun")
        for ( i = 0; i < nbBase; i++)
        {
            letters.push(0)
            letters.push(1)
            letters.push(2)
            letters.push(3)
            letters.push(4)
            //if (i%2==0) 
            letters.push(5) //village cité
        }

        var shuffledLetters:Array = new Array(letters.length);
        var randomPos:Number = 0;
        for ( i = 0; i < shuffledLetters.length; i++)
        {
            randomPos = int(Math.random() * letters.length);
            shuffledLetters[i] = letters.splice(randomPos, 1)[0];   //since splice() returns an Array, we have to specify that we want the first (only) element 
        }
        //trace (shuffledLetters)
        for ( i = 0; i < shuffledLetters.length; i++)
        {
            var a:xUnit;
            if (shuffledLetters[i]==1) { a = addUnit(Math.random()*100+32,Math.random()*100+32); }
            if (shuffledLetters[i]==2) { a = addUnit(Math.random()*100+332,Math.random()*100+332);  }
            if (shuffledLetters[i]==3) { a = addUnit(Math.random()*100+332,Math.random()*100+32); }
            if (shuffledLetters[i]==4) { a = addUnit(Math.random()*100+32,Math.random()*100+332);}  
           //citadelle
            if (shuffledLetters[i]==0) { a = addUnit(232,232);}
            //village
            if (shuffledLetters[i]==5) { a = addUnit(200+Math.random()*64,200+Math.random()*64);shuffledLetters[i]=0}
           
            //a  = addUnit(Math.random()*400+32,Math.random()*400+32); 
            //a  = addUnit(Math.random()*stage.width/2+50 ,Math.random()*stage.height/2); 
            a.team = shuffledLetters[i]; a.hp=4; a.spec=1; a.rad = 8;
            a.gx = a.cx +Math.random()*32-16; a.gy = a.cy+Math.random()*32-16;
           // countInit++
        }

        //garde kong citadelle
           for ( i = 0; i < nbBase*maxi/100; i++) // 3 pour 100, 1max pour 1000
        {  
            //var rx=Math.random()*stage.width/2+50;
            //var ry=Math.random()*stage.height/2+50;
           // a = addUnit(Math.random()*stage.width/2+50,Math.random()*stage.height/2+50);
            a  = addUnit(200+Math.random()*64,200+Math.random()*64); 
            a.gx = a.cx+Math.random()*16-8; 
            a.gy  = a.cy+Math.random()*16-8; 
            a.mode = 1;  
            a.team = 0; //a.hp=4; a.spec=0; a.rad = 4;
            //a.gx = a.cx +Math.random()*32-16; a.gy = a.cy+Math.random()*32-16; 
        }
        
        
        ///locals
        var shuffledSrc:Vector.<srcSpawn> = new Vector.<srcSpawn>(lesSrc.length)//.<srcSpawn>
        var tempSrc:Vector.<srcSpawn>=new Vector.<srcSpawn>
        for ( i = 0; i < lesSrc.length; i++){
            tempSrc.push(lesSrc[i])
        }
         //trace (tempSrc)
        for ( i = 0; i < shuffledSrc.length; i++)
        {
            randomPos = int(Math.random() * tempSrc.length);
            shuffledSrc[i] = tempSrc.splice(randomPos, 1)[0];   //since splice() returns an Array, we have to specify that we want the first (only) element 
        }

        for ( i = 0; i < nbBase*5; i++)
        {
            var ra=32; 
            a  = addUnit(shuffledSrc[i%lesSrc.length].x + Math.random()*ra-ra/2, shuffledSrc[i%lesSrc.length].y+ Math.random()*ra-ra/2)
            //a  = addUnit(Math.random()*stage.width/2+50 ,Math.random()*stage.height/2+50); 
            a.team = 5; a.hp=4; a.spec=1; a.rad = 8;
            a.gx = a.cx +Math.random()*32-16; a.gy = a.cy+Math.random()*32-16;  
        }

        //trolls
       // var rx=Math.random()*stage.width/2+50;
        //var ry=Math.random()*stage.height/2+50;
        for ( i = 0; i < nbBase*13; i++)
        {  
            var rx=Math.random()*464//-256// addUnit(232,232)//stage.width/2+115;
            var ry=Math.random()*464//-256//stage.height/2+35;
           // a = addUnit(Math.random()*stage.width/2+50,Math.random()*stage.height/2+50);
            a  = addUnit(rx,ry); 
            a.gx = rx+Math.random()*16-8; 
            a.gy  = ry+Math.random()*16-8; 
            a.mode = 1;  
            a.team = 5; //a.hp=4; a.spec=0; a.rad = 4;
            //a.gx = a.cx +Math.random()*32-16; a.gy = a.cy+Math.random()*32-16; 
        }
        

        trace (countInit+" bases");       
    }

        public var paused:Boolean=false;

        public function clickBTBAse(e:Event):void {
            var btn:SimpleButton = e.currentTarget as SimpleButton
            if (btn == bt_maison) {
                //selTeam = parseInt(comptAEff.text);
                adesel=false;
                //curSel = null;
                for (var i:int = 0; i < vecUnit.length; i++) {vecUnit[i].sel=false;} 
                if (paused) paused=false else paused=true;
            }
            if (btn == bt_redo) {   reInit()}
             
            if (btn == bt_R){selTeam=1}
            if (btn == bt_J){selTeam=0}
            if (btn == bt_M){selTeam=2}
            if (btn == bt_V){selTeam=3}
            if (btn == bt_C){selTeam=4}
            if (btn == bt_B){selTeam=5}

        }

        public var gt:int = 0; 
        public var numAlive:int =0;
        public var vecUnit:Vector.<xUnit>;
        public var mdown:int = 0;
        public function onMdown(e:MouseEvent):void { mdown = 1; }
        public function onMup(e:MouseEvent):void { mdown = 0; } 

        public var mx:Number = 0;
        public var my:Number = 0;             
        public var wmx:Number = 0;
        public var wmy:Number = 0;             
         var selTeam:int=0

       public function onEnter(e:Event):void {
            mx = stage.mouseX;   my = stage.mouseY;
            wmx = mx; wmy = my;          
            graphics.clear(); 
            graphics.beginFill(0x003020,0.4);
            //graphics.drawRect(0,0,465,465);
            graphics.endFill();

            if (!pretadepla)  graphics.lineStyle(4,0);
            else graphics.lineStyle(2,0); // pret à deplacer des unités deja selectionnées ? changer le graphisme
            
            graphics.drawRect(8,8,465-16,465-16);
            graphics.lineStyle(2,0);
            update();

            //selection equipe
           
            if (mdown > 0 &&  !pretadepla){ getOver(mx, my, 16, selTeam) }
            handSel();               
            if (!paused) gt += 1;
       }
       
        var adesel:Boolean=false;
        var pretadepla:Boolean=false;
        public function handSel():void {
            graphics.lineStyle(2,0x000000,.75);
            var a:xUnit;
            var c:uint; 
            if (paused){
               var num:int = vecUnit.length;
               for (var i:int = 0; i < num; i++){
                    a = vecUnit[i]
                    c = vecColor[a.team];
                    if (a.team !=1 && a.spec == 1) {
                        graphics.lineStyle(4,c,.15);
                        graphics.drawCircle(a.gx, a.gy, 2);//draw zone spawn adverses
                        graphics.moveTo(a.cx,a.cy);graphics.lineTo(a.gx,a.gy);                        
                    }
                    if (a.hp>0 && a.spec == 1 &&  a.team ==selTeam ) { // draw zone spawn soi
                        graphics.lineStyle(6,c,.25);
                        graphics.drawCircle(a.gx, a.gy, 4);
                        graphics.moveTo(a.cx,a.cy); graphics.lineTo(a.gx,a.gy);                      
                    }      
                    if (a.hp>0 && a.mode > 0 && a.spec==0 && a.team ==selTeam ) { //draw trajet de ses unités à soi
                        graphics.lineStyle(2,c,1);
                        graphics.moveTo(a.cx,a.cy); graphics.lineTo(a.gx,a.gy);       
                  }
               }
            }
   
            for (i = 0; i < vecUnit.length; i++) { // draw selection multi
                a=vecUnit[i]
                if (a.sel==true){  
                    graphics.lineStyle(2,0xFFFFFF,.75);
                    graphics.drawRect(a.cx-a.rad,a.cy-a.rad,a.rad*2,a.rad*2); }          
             }

             if (mdown == 0 ){ // fin de la selection
                if (adesel==true) { pretadepla=true;                 
                }else{pretadepla=false  }
             }

            if (mdown >0 ){ // GO deplacement
                if (pretadepla && wmy<500){  // clic dans la scène         
                    for (i = 0; i < vecUnit.length; i++) {
                         a=vecUnit[i];
                         if (a.sel==true){ 
                            a = vecUnit[i]
                            a.gx = wmx+Math.random()*16-8; 
                            a.gy = wmy+Math.random()*16-8; 
                            a.mode = 1; 
                            a.targ=null;
                            a.sel=false;
                        }
                    }
                    adesel=false;                   
                }
            }
       }


       public function getOver(ax:Number, ay:Number, r:Number, ofteam:int){//:xUnit
            var i:int; var num:int; var a:xUnit; var d:Number;
            num = vecUnit.length;
            for (i = 0; i < num; i++){
                a = vecUnit[i]; 
                if (a.hp <=0) {continue;}
                if (a.team != ofteam) { continue;}
                if (getMag(a.cx-ax,a.cy-ay) > (a.rad+r)) { continue; }
                if (a.sel==false){
                    adesel=true;
                    a.sel=true;
                }
           }     
       }
       
       public function addUnit(ax:Number, ay:Number):xUnit {
           var a:xUnit;  a = getFree();
           a.hp=8;    a.cx=ax; a.cy=ay;
           a.vx=0; a.vy=0;  a.rad = 4;
           //a.range = 32;  
           a.targ = null;
           countInit++;
           return a;
       }
       
       public function getFree():xUnit{
           var i:int; var num:int; var a:xUnit;
           num = vecUnit.length;
           for (i = 0; i < num; i++){
                a = vecUnit[i]; if (a.hp>0){continue;} 
                return a; 
            }
           return a; //return last if reached limit           
       }
       
       public function getClose(m:xUnit, maxd:Number=800):xUnit {
            var i:int; var num:int; var a:xUnit;
            var d:Number; var ret:xUnit;
            ret = null;
            maxd *= maxd;//squared
            num = vecUnit.length;
            for (i = 0; i < num; i++){
                a = vecUnit[i]; 
                if (a.hp <=0) {continue;}
                //if (isCroiseUnNode(a.cx,a.cy,m.cx,m.cy)) {continue;}
                //if (IsIntersecting(a,m,new Point(250,0), new Point(250,500))) {continue}
                if (a.team==m.team){continue;}  
                if (a == m) {continue;}
                d = (a.cx-m.cx)*(a.cx-m.cx) + (a.cy-m.cy)*(a.cy-m.cy);
                if (d > maxd) {continue;}
                ret = a; maxd = d;
            }      
           return ret;
       }

       


        public function IsIntersecting(p1:xUnit, p2:xUnit, p3:Point, p4:Point):Boolean {
            var x1:Number = p1.cx, x2:Number = p2.cx, x3:Number = p3.x, x4:Number = p4.x;
            var y1:Number = p1.cy, y2:Number = p2.cy, y3:Number = p3.y, y4:Number = p4.y;
            var z1:Number= (x1 -x2), z2:Number = (x3 - x4), z3:Number = (y1 - y2), z4:Number = (y3 - y4);
            var d:Number = z1 * z4 - z3 * z2;
            
            // If d is zero, there is no intersection
            if (d == 0) return false;
            
            // Get the x and y
            var pre:Number = (x1*y2 - y1*x2), post:Number = (x3*y4 - y3*x4);
            var x:Number = ( pre * z2 - z1 * post ) / d;
            var y:Number = ( pre * z4 - z3 * post ) / d;
            
            // Check if the x and y coordinates are within both lines
            if ( x < Math.min(x1, x2) || x > Math.max(x1, x2) ||
                x < Math.min(x3, x4) || x > Math.max(x3, x4) ) return false;
            if ( y < Math.min(y1, y2) || y > Math.max(y1, y2) ||
                y < Math.min(y3, y4) || y > Math.max(y3, y4) ) return false;
            
            // if Return the point of intersection : c'est new Point(x, y);
            return true;
        }
        
       //var calme:Boolean=true;
       //var durecalme:int;
       public function update():void  {      
            var na:int; na = 0;//unité sur le terrain
            var i:int; var num:int; var a:xUnit;
            var b:xUnit;
            var d:Number;
            num = vecUnit.length;
            compt.text=numAlive.toString();
            compt2.text=gt.toString();
           // comptBEff.text = (parseInt(comptAEff.text)+gt).toString();
            var divers:Boolean=false;
          //  var winner:int=-1;
           

            for (i = 0; i < num; i++) { 
                a = vecUnit[i]; 
                if (a.hp<=0){continue;}
                if (a.team != vecUnit[0].team) divers=true;
                //winner=vecUnit[0].team
                na +=1;

                //base SPAWN
                if (a.spec == 1){
                    var c:uint;
                    c = vecColor[a.team];
                    if (a.dmgTime > gt) { c=0xFFffFFff; }                
                    graphics.beginFill(c,0.85);
                    graphics.drawCircle(a.cx, a.cy, a.rad);
                    graphics.endFill(); 
                    graphics.drawRect(a.cx-a.rad,a.cy-a.rad,a.rad*2,a.rad*2);    
                    
                    if (numAlive < maxi-maxi/16)//Math.random()*20+4)//1 à 12 selon basard//96// 1280 max d'unité sur le terrain SPAWN 1200 decoupe des frontières et 1280 fait plus de dawa
                   // if ((gt % num) == num);//(i%128))
                    if (a.reload < gt) {                      
                           a.reload = gt + a.spawn+Math.random()*a.RNDspawn; //durée spawn
                           //if (a.team==1) a.reload = gt + 75
                           b = addUnit(a.cx+Math.random()*16-8,a.cy+Math.random()*16-8);
                           b.team = a.team; 
                           if (b.team==1) {
                                //b.hp=1; //2 plus hp des bases au init : il etait à 40
                                //b.spd=4;//2.5
                                //b.spawn=2//75//1+1
                                //b.dmg=4;//1 one shot pas signifnat avec 2 pv +1
                                //b.range=40;//32 +1 
                                //b.recharge=10//20//4 +1
                                
                            }

                            //test secu : ne plus être ciblé quand il spawn
                            //for (var j:int = 0; j < num; j++) if (b == vecUnit[j].targ) vecUnit[j].targ=null;
                                          
                           // direcion du nouveau
                            b.gx = a.gx+Math.random()*16-8; 
                            b. gy  = a.gy+Math.random()*16-8; 
                            b.mode = 1;    
                           
                           //cibler une nouvelle zone de span (pour les adverses surtout ?)
                           if (a.team > 0) {
                                 a.ang = Math.random()*6.28;  
                                 d = Math.random()*32 + a.rad;
                                 a.gx = a.cx + Math.cos(a.ang) * d;
                                 a.gy = a.cy+ Math.sin(a.ang) * d;  
                           }
                                               
                    } 
                    continue;
                }             
            
            
                //regular unit
                if (a.targ == null)
                
                    //donne plus ou moins vite de cible 
                    //16 cte bidouille pour opti. toute les 64 frames, faire 2 fois dans la boucle
                    // if ((gt % 128) == (i%128)){ 
                    if ((gt % 8) == (i%8)){ 
                        a.targ = getClose(a, a.sight);
                    }
                
               // if (a.targ == null)    {  // pourquoi il était commenté ??
                   if (a.mode == 1) { //va dans sa propre direction
                        a.ang = Math.atan2(a.gy-a.cy,a.gx-a.cx);
                        a.vx = Math.cos(a.ang);
                        a.vy = Math.sin(a.ang);
                    if (getMag(a.gx-a.cx,a.gy-a.cy) < a.rad) {
                        a.vx =0; a.vy =0; a.mode = 0; }    
                   }                   
               // }//endif
               
               
               if (a.targ != null)  {
                    d = getMag(a.cx-a.targ.cx, a.cy-a.targ.cy);
                    if (d > a.sight || a.targ.hp<=0 || a.targ.team == a.team) { 
                        a.targ = null;                   
                    }else { 
                        if (d < a.range) { //porté de tir
                          if (a.mode != 1) { a.vx=0;a.vy=0; }//s'arrete avant de tirer
                          if (a.reload < gt) {
                             a.reload = gt+a.recharge;// 20; //recharge
                             a.fire =gt+a.recharge/4; //durée du tir
                             a.targ.hp -= a.dmg;
                             a.dmgTime = gt+4;//duré du skin quand il tir
                            //calme = false;

                                if (a.targ.hp <= 0 && a.targ.spec == 1) {
                                    a.targ.spec=0;//tuer une base 
                                    
                                    
                                    //durecalme=0; 
                                    //trace (a.targ.team, a.team)
                                    if (a.targ.team == 1) comptA.text = (parseInt(comptA.text) -1).toString();  
                                    if (a.targ.team == 0) comptB.text = (parseInt(comptB.text) -1).toString();
                                    if (a.targ.team == 2) comptC.text = (parseInt(comptC.text) -1).toString();  
                                    if (a.targ.team == 3) comptD.text = (parseInt(comptD.text) -1).toString(); 
                                    if (a.targ.team == 4) comptE.text = (parseInt(comptE.text) -1).toString(); 
                                    if (a.targ.team == 5) comptF.text = (parseInt(comptF.text) -1).toString(); 
                                   
                                   if (a.team==1) comptAPt.text = (parseInt(comptAPt.text) +1).toString();
                                    if (a.team==0) comptBPt.text = (parseInt(comptBPt.text) +1).toString();
                                     if (a.team==2) comptCPt.text = (parseInt(comptCPt.text) +1).toString();
                                      if (a.team==3) comptDPt.text = (parseInt(comptDPt.text) +1).toString();
                                       if (a.team==4) comptEPt.text = (parseInt(comptEPt.text) +1).toString();
                                        if (a.team==5) comptFPt.text = (parseInt(comptFPt.text) +1).toString();

                                    comptATt2.text = (parseInt(comptAPt.text) + parseInt(comptA.text)  ).toString();  
                                    comptBTt2.text = (parseInt(comptBPt.text) + parseInt(comptB.text)  ).toString();  
                                    comptCTt2.text = (parseInt(comptCPt.text) + parseInt(comptC.text)  ).toString();  
                                    comptDTt2.text = (parseInt(comptDPt.text) + parseInt(comptD.text)  ).toString();  
                                    comptETt2.text = (parseInt(comptEPt.text) + parseInt(comptE.text)  ).toString();  
                                    comptFTt2.text = (parseInt(comptFPt.text) + parseInt(comptF.text)  ).toString();  

                                    for (var i3 = 0; i3 < lesScores.length; i3++) {
                                        //trace (lesScores[i3].name)
                                        if (lesScores[i3].name == "scRouge") lesScores[i3].scaleX = nbBase+1+parseInt(comptATt2.text)*1;
                                        if (lesScores[i3].name == "scJaune") lesScores[i3].scaleX = nbBase*2+1+parseInt(comptBTt2.text)*1;
                                        if (lesScores[i3].name == "scMag")   lesScores[i3].scaleX = nbBase+1+parseInt(comptCTt2.text)*1;
                                        if (lesScores[i3].name == "scVert") lesScores[i3].scaleX = nbBase+1+parseInt(comptDTt2.text)*1;
                                        if (lesScores[i3].name == "scCyan") lesScores[i3].scaleX = nbBase+1+parseInt(comptETt2.text)*1;
                                        if (lesScores[i3].name == "scBleu") lesScores[i3].scaleX = nbBase*5+parseInt(comptFTt2.text)*1;

                                    }
                                    //lesScores[0].scaleX = parseInt(comptATt2.text)*1;
                                    
                                   // lesScores[2].scaleX = parseInt(comptCTt2.text)*1;

                                    var n:xUnit
                                    //n  = addUnit(Math.random()*stage.width/2+50 ,Math.random()*stage.height/2); 
                                    n  = addUnit(a.targ.cx,a.targ.cy); 
                                    n.team = a.team; n.hp=4; n.spec=1; n.rad = 8;
                                    n.gx = n.cx +Math.random()*32-16; n.gy = n.cy+Math.random()*32-16;
                                    
                                   
                                   //if (a.team)  (parseInt(comptA.text)== (parseInt(comptA.text)+(parseInt(comptB.text)
                                }else{
                                    //durecalme++
                                }
                             
                             if (a.targ.targ == null){a.targ.targ = a;} //riposte
                             if (a.targ.hp <=0) {a.targ=null; 
                                //trace ("lk"+gt);
                                //calme = false
                             }
                              // essai pour empehcer de viser un respawn : ne plus cibler apres avoir tirer
                             //a.targ=null
                        }
                     } else if (a.mode != 1) {//va vers sa target si hors de portée
                       // a.fire=0;
                        a.ang = Math.atan2(a.targ.cy-a.cy,a.targ.cx-a.cx);
                        a.vx = Math.cos(a.ang);
                        a.vy = Math.sin(a.ang);   
                   }  
                 }
               }
            //if (a.targ!=null && a.targ.hp<=0)
            //if (getMag(a.targ.cx-a.cx,a.targ.cy-a.cy) < a.range ) a.fire=0;
                //rebondi
                 
                    var col=0;
                    if (Math.random()>.9) col=1 
                   for (var i2 = 0; i2 < lesMurs.length; i2++) {
                       var dx:int = lesMurs[i2].x
                       var dx2:int = lesMurs[i2].width+lesMurs[i2].x
                       var dy:int = lesMurs[i2].y
                       var dy2:int = lesMurs[i2].height+lesMurs[i2].y

                   

                         if ( (a.vx < 0) && (a.cx < dx2) && (a.cx > dx) ){ //vers la gauche
                           if ( (a.cy < dy2) && (a.cy > dy) )
                                (a.vx *=-col) 
                         }
                            
                         else if ( (a.vx > 0) && (a.cx > dx) && (a.cx < dx2) ) // vers la droite
                         {
                            if ( (a.cy > dy) && (a.cy < dy2) ) 
                                (a.vx *=-col)
                         }


                        if ( (a.vy < 0) && (a.cy < dy2) && (a.cy > dy) ) { //monte
                            if ( (a.cx < dx2) && (a.cx > dx) )
                                (a.vy *=-col)
                        }
                         else if ( (a.vy > 0) && (a.cy > dy) && (a.cy < dy2) ) {//descend
                            if ( (a.cx < dx2) && (a.cx > dx) )
                                (a.vy *=-col)
                        }
                }
        

                if (!paused) { 
                   a.cx += a.vx*a.spd;   a.cy += a.vy*a.spd;               
                   if (a.vx >0 && a.cx > 465) {a.vx*=-1;}
                   else  if (a.vx<0 && a.cx < 0) {a.vx*=-1;}
                   if (a.vy >0 && a.cy > 465) {a.vy*=-1;}
                   else  if (a.vy<0 && a.cy < 0) {a.vy*=-1;} 

                }

               
                
                c = vecColor[a.team];
                // skin de l'event quand il tir
                if (a.dmgTime > gt) {  c=0xFFffFFff; }
             
                //draw unit
                graphics.beginFill(c,0.85);
                graphics.drawCircle(a.cx, a.cy, a.rad);
                graphics.endFill();    
            
            } 
                 
            

            //dessin tir     
            for (i = 0; i < num; i++) {
                a = vecUnit[i]; 
                if (a.hp<=0){continue;}//  
                if (a.fire > gt)  
                if (a.targ != null)
              // if (getMag(a.targ.cx-a.cx,a.targ.cy-a.cy) <= a.range )     // oula
                if (a.targ.hp > 0) { 
                    comptBEff.text =  gt.toString();
                    c=vecColor[a.team]
                    graphics.lineStyle(2,c,.95);
                    graphics.moveTo(a.cx,a.cy); graphics.lineTo(a.targ.cx, a.targ.cy); 
                    graphics.drawCircle(a.targ.cx, a.targ.cy, 2);  
                }
                //secu
                //if (Math.random()*100 > 85) 
                //a.targ=null
           }
          numAlive = na; 
          // comptBEff.text =  durecalme.toString();
          /*if (calme) {
           
            durecalme++;
            }*/
        if (!divers || (gt>parseInt(comptBEff.text)+300) ) {

            comptATt.text = (parseInt(comptAPt.text)+ parseInt(comptATt.text) ).toString();
            comptBTt.text = (parseInt(comptBPt.text)+ parseInt(comptBTt.text) ).toString();
            comptCTt.text = (parseInt(comptCPt.text)+ parseInt(comptCTt.text) ).toString();
            comptDTt.text = (parseInt(comptDPt.text)+ parseInt(comptDTt.text) ).toString();
            comptETt.text = (parseInt(comptEPt.text)+ parseInt(comptETt.text) ).toString();
            comptFTt.text = (parseInt(comptFPt.text)+ parseInt(comptFTt.text) ).toString();


            if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 1)  comptAVic.text = (parseInt(comptAVic.text)+1).toString();
             if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 0)  comptBVic.text = (parseInt(comptBVic.text)+1).toString();
             if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 2)  comptCVic.text = (parseInt(comptCVic.text)+1).toString();
             if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 3)  comptDVic.text = (parseInt(comptDVic.text)+1).toString();
             if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 4)  comptEVic.text = (parseInt(comptEVic.text)+1).toString();
             if (vecUnit[Math.floor(Math.random()*vecUnit.length)].team == 5)  comptFVic.text = (parseInt(comptFVic.text)+1).toString();
            reInit();
        }   
        //if (durecalme>1000) reInit()
       // if reInit();
       }
       
      // Math.random()*vecUnit.length

        var yaqq1:Array;

       public function getMag(ax:Number,ay:Number):Number{ return Math.sqrt(ax*ax+ay*ay);}

    }//classend
}


internal class xUnit {
    public var sel:Boolean=false;
    public var spd:Number = 2.5;//speed
    public var spawn:Number = 1//90//recharge base    //1spaxwn 2 rnd stable  // 2spawn 1rnd chaostique
    public var RNDspawn:Number = 2//20;//
    public var recharge:Number = 4//20;//recharge tir
    public var sight:Number = 256;//porté vue
    public var range:Number = 16+Math.random()*42;//64;//porté tir
    public var dmg:Number = 1; //degat
   
    public var team:int = 0;
    public var rad:Number = 0;   
    public var spec:int = 0; // 1 est une base  
    public var cx:Number = 0;//position
    public var cy:Number = 0;
    public var vx:Number = 0;//vitess
    public var vy:Number = 0;
    public var gx:Number = 0;//goal
    public var gy:Number = 0;
    public var mode:int = 0; // en depla
    public var ang:Number =0;
    public var hp:Number = 0;

    public var targ:xUnit = null; //i shouldnt do this and use some id instead
    
    
    public var fire:int = 0;// timer duré du tir graphique
    public var dmgTime:int = 0; //timer temps de skin tir
    public var reload:int = 0;  //timer recherche de tir
   
}//xunit