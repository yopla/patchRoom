package
{
	
	// des fantomes en pathfind / le troll
	// deplacer son panier
	// les feed popent rand
	
	// un docteur maboule spootnik decollage
	//integrer des sauteurs funs
//	des spiders
//	des ragdoll
	
	//des echelles
	//des fixe rigid bodie à tourner
	
	
	import com.actionsnippet.qbox.QuickBox2D;
	import com.actionsnippet.qbox.objects.CircleObject;
	
	import flash.display.DisplayObject;
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.net.URLRequest;
	import flash.utils.getTimer;
	
	import Box2D.Collision.b2AABB;
	import Box2D.Collision.Shapes.b2CircleDef;
	import Box2D.Collision.Shapes.b2MassData;
	import Box2D.Collision.Shapes.b2PolygonDef;
	import Box2D.Common.Math.b2Vec2;
	import Box2D.Dynamics.b2Body;
	import Box2D.Dynamics.b2BodyDef;
	import Box2D.Dynamics.b2World;
	
	//import Box2DAS.Collision.Shapes.b2MassData;
	
	public class pac extends Sprite
	{
		private var world:b2World;
		private var car:b2Body;	
		//private var carv:carV;
		private var fixed:Array; // of b2Body
		private var float:Array; // of b2Body
		private var worldView:Sprite;
		
		private static const A_FORCE:Number = 20000;
		private static const S_FORCE:Number = A_FORCE * 0.7;
		private static const B_DAMP:Number = 1.0;
		private static const NB_DAMP:Number = 0.1;
		
		private var acl:Boolean = false;
		private var brk:Boolean = false;
		private var steering:Number = 0;
		
		private var haut:Boolean = false;
		private var bas:Boolean = false;
		private var gauche:Boolean = false;
		private var droite:Boolean = false;
		private var space:Boolean = false;
		
		//static public var lesFooks:Vector.<lala> = new Vector.<lala>;
		static public var lesMurs:Vector.<mur> = new Vector.<mur>;
		//static public var lesPingous:Vector.<pingou> = new Vector.<pingou>;
		//static public var lesObjectifs:Vector.<objectif> = new Vector.<objectif>;
		
		private static const VK_LEFT:int = 37;
		private static const VK_UP:int = 38;
		private static const VK_RIGHT:int = 39;
		private static const VK_DOWN:int = 40;
		private static const VK_SPC:int = 32;
		
		
	
		private var _softbodys:Vector.<SoftBody>=new Vector.<SoftBody>;
		private var _qbox:QuickBox2D;
		
		
		
		
		public function pac()
		{
		//	var fpsDemo:FPSDemo = new FPSDemo();
		//	addChild( fpsDemo );
		//	fpsDemo.x=1700
	//		fpsDemo.y=50
				
			for( var j:int = this.numChildren - 1; j>=0; j-- ) {
				//if( this.getChildAt(j) is lala ) lesFooks.push(this.getChildAt(j));
				if( this.getChildAt(j) is mur )	lesMurs.push(this.getChildAt(j));		
				//if( this.getChildAt(j) is pingou )	lesPingous.push(this.getChildAt(j));		
				//if( this.getChildAt(j) is objectif ) lesObjectifs.push(this.getChildAt(j));	
				// les flammes murs piquants
			}
			
			var mc:MovieClip = new MovieClip();
			addChild(mc);
			_qbox = new QuickBox2D(mc);
			_qbox.gravity = new b2Vec2(0,0);
			
			_qbox.createStageWalls();
			_qbox.mouseDrag();
			_qbox.start();

			
			
			//_qbox.addBox( { x:26, y:17, width:4, height:1,density:0 } );
			_qbox.setDefault( { lineAlpha:1, fillAlpha:0 } );
			for each(var o:mur in lesMurs) {				
				_qbox.addBox( { x:o.x/30, y:o.y/30, 
					width:o.scaleX/2, height:o.scaleY/2,
					angle:o.rotation*Math.PI/180, 
					density:0 } );
			}
			
			for( var i:int = 0; i<3; i++ ) {
				//addBlob();
			}
		
			
			
			
			
			///////////////////////
			this.world = createWorld(); // ワールドを作る
			this.worldView = new Sprite();
			
			
			/*var carView:DisplayObject = createCarView();
			this.car = createCarBody(); // 車モデルを作る
			worldView.addChild(carView);
			car.SetUserData(carView);
			*/
			//this.fixed = createFixed(); // 壁を作る
			//this.float = createFloat(); // 障害物を作る
			
			//addChild(worldView);
			//createBackground();
			//createFixedView();
			//createFloatView();
					
			addEventListener(Event.ENTER_FRAME, function(e:Event):void {
				step();
			});
			
			
			
			/*keyTableDOWN[VK_UP] = function():void { acl = true; };
			keyTableDOWN[VK_DOWN] = function():void { brk = true; };
			keyTableDOWN[VK_LEFT] = function():void { steering =-Math.PI/2; };
			keyTableDOWN[VK_RIGHT] = function():void { steering = Math.PI/2; };
			keyTableDOWN[VK_SPC] = reset;			
			keyTableUP[VK_UP] = function():void { acl = false; };
			keyTableUP[VK_DOWN] = function():void { brk = false; };
			keyTableUP[VK_LEFT] = function():void { steering = 0; };
			keyTableUP[VK_RIGHT] = function():void { steering = 0; };*/
		
			var keyTableDOWN:Array = [];
			keyTableDOWN[VK_UP] = function():void { haut = true; };
			keyTableDOWN[VK_DOWN] = function():void { bas = true; };
			keyTableDOWN[VK_LEFT] = function():void { gauche = true };
			keyTableDOWN[VK_RIGHT] = function():void { droite = true};
			keyTableDOWN[VK_SPC] = function():void { space = true};
			var keyTableUP:Array = [];
			keyTableUP[VK_UP] = function():void { haut = false; };
			keyTableUP[VK_DOWN] = function():void { bas = false; };
			keyTableUP[VK_LEFT] = function():void { gauche = false };
			keyTableUP[VK_RIGHT] = function():void { droite = false};
			keyTableUP[VK_SPC] = function():void { space = false};
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, function(e:KeyboardEvent):void {
				var f:Function = keyTableDOWN[e.keyCode];
				if (f != null) {
					f();
				}
			});	
			stage.addEventListener(KeyboardEvent.KEY_UP, function(e:KeyboardEvent):void {
				addBlob()
				var f:Function = keyTableUP[e.keyCode];
				if (f != null) {
					f();
				}
			});
			
		}
		private function addBlob() {
			//var i=Math.random()*20;
			var n=new SoftBody(_qbox, { 
				x:mouseX/30,//+i*(1000/15)/30, 
				y:mouseY/30, 
				lineAlpha:.1,lineColor:0xFFFFFF, fillColor:.5+Math.random()*0xFFFFFF/*0x2485F3*/, fillAlpha:0.7, 
				radius:.45 } );
			_softbodys.push(n);
			addChild(n);
		}
		
		//////////////
		private function createWorld():b2World {
			var aabb:b2AABB = new b2AABB();
			aabb.lowerBound.Set(0, 0);
			aabb.upperBound.Set(2000, 2000);
			var gravity:b2Vec2 = new b2Vec2();
			return new b2World(aabb, gravity, true);
		}
		private function reset():void {
			car.SetXForm(new b2Vec2(500, 500+16), 0);
			car.SetLinearVelocity(new b2Vec2());
			car.SetAngularVelocity(0);
		}
		
		private function createCarBody():b2Body {
			var def:b2BodyDef = new b2BodyDef();
			def.position.Set(500, 500);
			def.angularDamping = 0.75; // 回転の減衰
			var body:b2Body = world.CreateBody(def);
			var rect:b2PolygonDef = new b2PolygonDef();
			rect.SetAsBox(16,16);//carv.width, carv.height);
			rect.density = 1//((carv.width*carv.height)/16*16)//1 pour 16*16;       // 0:固定 kg/m^2
			rect.restitution = 0.4; // 反発係数[0,1]
			rect.friction = 0.1;    // 摩擦[0,1]
			body.CreateShape(rect);
			body.SetMassFromShapes();			
			//var massData:Box2D.Collision.Shapes.b2MassData = new Box2D.Collision.Shapes.b2MassData()
			//trace(body.GetMass());
			//massData.mass = 1024.0;
			//body.SetMass(massData);
			body
			return body;
		}
		private function createCarView():DisplayObject {
			var sprite:Sprite = new Sprite();
			//carv=new carV();
			/*var image:Loader = new Loader();
			image.load(new URLRequest("http://img.f.hatena.ne.jp/images/fotolife/f/flashrod/20100121/20100121222408.png?1264080327"));
			image.x = -32/2;
			image.y = -32/2;*/
			//sprite.addChild(carv);
			return sprite;
		}
		
		private function createFixed():Array {
			var a:Array = [
				{x:388, y:120, w:100, h:8, a:0.7853981633974483},
				{x:73, y:120, w:100, h:8, a:-0.7853981633974483},
				{x:124, y:764, w:116.25, h:8, a:0},
				{x:343, y:1226, w:116.25, h:8, a:0},
				{x:119, y:1612, w:116.25, h:8, a:0},
				{x:348, y:2000, w:140, h:8, a:0.7853981633974483},
				{x:116, y:2500, w:140, h:8, a:-0.7853981633974483},
				{x:0, y:1500, w:8, h:1500, a:0},
				//{x:465, y:1500, w:8, h:1500, a:0},
				{x:232, y:3000, w:232, h:8, a:0},
			];
			var list:Array = [];
			
			for each(var o:mur in lesMurs) {				
				var def:b2BodyDef = new b2BodyDef();
				def.angle = o.rotation*Math.PI/180; // ATTENTION, le haut doit être en haut
				//def.angle = o.rotation*180/Math.PI;
				def.position.Set(o.x, o.y);
				var body:b2Body = world.CreateBody(def);
				var rect:b2PolygonDef = new b2PolygonDef();
				rect.SetAsBox(o.scaleX*8, o.scaleY*8);
				rect.restitution = 0.4;
				rect.friction = 0.1;
				body.CreateShape(rect);
				body.SetUserData({w:o.scaleX*8, h:o.scaleY*8});
				list.push(body);
			}
						
		/*	for each (var o:* in a) {
				var def:b2BodyDef = new b2BodyDef();
				def.position.Set(o.x, o.y);
				def.angle = o.a;
				var body:b2Body = world.CreateBody(def);
				var rect:b2PolygonDef = new b2PolygonDef();
				rect.SetAsBox(o.w, o.h);
				rect.restitution = 0.4;
				rect.friction = 0.1;
				body.CreateShape(rect);
				body.SetUserData({w:o.w, h:o.h});
				list.push(body);
			}*/
			return list;
		}
		
		
		private function createFloat():Array {
			var a:Array = [
				{x:283, y:341, r:8},
				{x:112, y:178, r:8},
				{x:355, y:354, r:8},
				{x:170, y:473, r:8},
			];
			var list:Array = [];
			for each (var o:* in a) {
				var def:b2BodyDef = new b2BodyDef();
				def.position.Set(o.x, o.y);
				def.linearDamping = 1.0;
				def.angularDamping = 1.0;
				var body:b2Body = world.CreateBody(def);
				var circle:b2CircleDef = new b2CircleDef();
				circle.density = 0.01;
				circle.radius = o.r;
				circle.restitution = 0.6;
				circle.friction = 0.2;
				body.CreateShape(circle);
				body.SetMassFromShapes();
				body.SetUserData({r:o.r});
				list.push(body);
			}
			return list;
		}
		private function createFixedView():void {
			for each (var b:b2Body in fixed) {
				var o:* = b.GetUserData();
				var shape:Shape = new Shape();
				shape.graphics.lineStyle(1, 0x000000);
				shape.graphics.beginFill(0xCCCCCC);
				shape.graphics.drawRect(-o.w, -o.h, 2*o.w, 2*o.h);
				shape.graphics.endFill();
				shape.alpha=.5;
				var p:b2Vec2 = b.GetPosition();
				shape.x = p.x;
				shape.y = p.y;
				shape.rotation = b.GetAngle() * (180 / Math.PI);
				o.s = shape;
				//worldView.addChild(shape);
			}
		}
		private function createFloatView():void {
			for each (var b:b2Body in float) {
				var o:* = b.GetUserData();
				var shape:Shape = new Shape();
				shape.graphics.lineStyle(1, 0x000000);
				shape.graphics.beginFill(0x44FF44);
				shape.graphics.drawCircle(0, 0, o.r);
				shape.graphics.endFill();
				var p:b2Vec2 = b.GetPosition();
				shape.x = p.x;
				shape.y = p.y;
				o.s = shape;
				o.name = "po";
				worldView.addChild(shape);
			}
		}
		private function updateFloatView():void {
			for each (var b:b2Body in float) {
				var o:* = b.GetUserData();
				var shape:Shape = o.s;
				var p:b2Vec2 = b.GetPosition();
				shape.x = p.x;
				shape.y = p.y;
				shape.rotation = b.GetAngle() * (180 / Math.PI);
			}
		}
		/*private function createBackground():void {
			for each (var j:int in [500, 100]) {
				var shape:Shape = new Shape();
				var k:int = 0;
				for (var i:int = 0; i < 1000; i += 8) {
					shape.graphics.beginFill(0xFFFFFF);
					shape.graphics.drawRect(i, k, 8, 8);
					shape.graphics.endFill();
					k = k == 0 ? 8 : 0;
				}
				shape.x = 0;
				shape.y = j;
				worldView.addChild(shape);
			}
		}*/
		private function step():void {
			
			for each(var soft:SoftBody in _softbodys) {		
				soft.update();
			}
			
		/*
			
			world.Step(1 / 9, 10);


			var p:b2Vec2 = car.GetPosition(); // 車の中心
			var t:Number = car.GetAngle(); // 0が上向き
			var vy:Number =-Math.cos(t); // (vx,vy)進行方向ベクトル
			var vx:Number = Math.sin(t);
			if (acl) {
				var rp:b2Vec2 = new b2Vec2(p.x - 16 *vx, p.y - 16 *vy);
				car.ApplyForce(new b2Vec2(A_FORCE *vx, A_FORCE *vy), rp);
			}
			
			if (haut) {				
				//car.ApplyImpulse(new b2Vec2(0, -5000), new b2Vec2(p.x, p.y) )
				car.ApplyForce(new b2Vec2(0, -10000), new b2Vec2(p.x, p.y) )
			}
			if (bas) {
				//car.ApplyImpulse(new b2Vec2(0, 5000), new b2Vec2(p.x, p.y) )
				car.ApplyForce(new b2Vec2(0, 10000), new b2Vec2(p.x, p.y) )
			}
			if (gauche) {
				//car.ApplyImpulse(new b2Vec2(-5000, 0), new b2Vec2(p.x, p.y) )
				car.ApplyForce(new b2Vec2(-10000, 0), new b2Vec2(p.x, p.y) )			
			}
			if (droite) {
				//car.ApplyImpulse(new b2Vec2(5000, 0), new b2Vec2(p.x, p.y) )
				car.ApplyForce(new b2Vec2(10000, 0), new b2Vec2(p.x, p.y) )
			}
			
			if (space) {
				for (var bb:b2Body = world.GetBodyList(); bb; bb = bb.GetNext()) {
					if ((bb.GetUserData() is Object)&&(bb.GetUserData().name=="po")) {
						var b:b2Vec2 = bb.GetPosition();
						var gap_x:Number = b.x - p.x;
						var gap_y:Number = b.y - p.y;
						var len:Number = Math.sqrt(gap_x*gap_x + gap_y*gap_y);
						var proxim=4
						var heroForce=1000*Math.pow(10,proxim);
						len=Math.pow(len,proxim)/heroForce;
						if (len<proxim*5) bb.ApplyImpulse(new b2Vec2((b.x-p.x)/len, (b.y-p.y)/len), new b2Vec2(p.x, p.y) );
					}
				}
				
			}
			
			// ブレーキ
			car.m_linearDamping = brk ? B_DAMP : NB_DAMP;
			// ステアリング
			if (steering != 0) {
				// 力を働かせる点は車の前方
				var sy:Number = S_FORCE *-Math.cos(t + steering);
				var sx:Number = S_FORCE * Math.sin(t + steering);
				var fp:b2Vec2 = new b2Vec2(p.x + 16 *vx, p.y + 16 *vy);
				car.ApplyForce(new b2Vec2(sx, sy), fp);
			}
			// モデルをビューに反映する
			var d:DisplayObject = DisplayObject(car.GetUserData());
			d.x = p.x;
			d.y = p.y;
			d.rotation = car.GetAngle() * (180 / Math.PI);
			updateFloatView();
			// スクロール
			//worldView.x = -(p.x - VW/2);
			//worldView.y = -(p.y - VH/2);
			// 時間計測
			//countup = (GOAL_Y <= p.y && p.y < START_Y);
			*/
		}
		
	}
}



import com.actionsnippet.qbox.QuickBox2D;
import com.actionsnippet.qbox.QuickObject;
import com.actionsnippet.qbox.objects.CircleObject;

import flash.display.Sprite;

import Box2D.Common.Math.b2Vec2;

class Anchor extends CircleObject
{
	public var vx:Number = 0;
	public var vy:Number = 0;
	private var spring:Number = 1.5;
	private var friction:Number = 0.1;
	private var density:Number = 0.;
	private var vec:b2Vec2;
	public var target:QuickObject;
	private var rot:Number;
	
	public var truc:QuickObject
	var trucok:Boolean=false;
	
	public function Anchor(qbox:QuickBox2D, params:Object, target:QuickObject = null, rot:Number = 0)
	{
		super(qbox, params);
		this.target = target;
		this.rot = rot;
		vec = new b2Vec2();	
		
		//truc = qbox.addCircle({x:x,y:y,radius:.2, density:.00, mass:0 })
	}
	
	public function addCirc(){
		trucok=true
		//truc = qbox.addBox({x:x,y:y,width:.63, height:.1, density:.00, mass:0 });
		//truc = qbox.addCircle({x:x,y:y,radius:.13, density:.00, mass:0 });
	}
		
	//var age:int=0
	//var rx, ry:Number=0;
	public function update():void
	{
			
		if (!target) return;
		var targetPos:b2Vec2 = target.body.GetPosition();
		var thisPos:b2Vec2 = body.GetPosition();
		var dx:Number = Math.sin(rot * Math.PI / 180)*2 + targetPos.x - thisPos.x;
		var dy:Number = Math.cos(rot * Math.PI / 180)*2 + targetPos.y - thisPos.y;
		var ax:Number = dx * spring;
		var ay:Number = dy * spring;
		
		//if (trucok){
			//truc.angle=rot/180*Math.PI;
			truc.x=Math.sin(rot * Math.PI / 180) * .35 + thisPos.x
			truc.y=Math.cos(rot * Math.PI / 180) * .35 + thisPos.y
		//truc.x = thisPos.x
		//truc.y = thisPos.y
		//}
		
		vx += ax;
		vy += ay;
		vec.x = vx;
		vec.y = vy;
		body.ApplyForce(vec, thisPos);
		vx *= friction;
		vy *= friction;	
	}
}




class SoftBody extends Sprite
{
	public var params:Object;
	private var _anchor:Array = [];//Anchor
	private var qbox:QuickBox2D;
	private var centrum:QuickObject;
	public var d1, d2, d3,d4
	
	
	public function SoftBody(qbox:QuickBox2D, params:Object)
	{
		d1 = .35*Math.random()*10-2;
		d2 = .35*Math.random()*10-3;
		d3 = .35*Math.random()*10-2;
		d4 = .35*Math.random()*10-3;

		this.params = params;
		this.qbox = qbox;
		if (this.params.lineColor == null) this.params.lineColor = 1;
		if (this.params.lineAlpha == null) this.params.lineAlpha = 1;
		if (this.params.fillColor == null) this.params.fillColor = 0;
		if (this.params.fillAlpha == null) this.params.fillAlpha = 1;
		if (this.params.radius == null) this.params.radius = 1;
		
		create();
		//centrum = qbox.addCircle({x:_anchor[0].x,y:_anchor[0].y,radius:.255,density:.00, mass:0.0 });
	}
	
	private function create():void
	{
		
		var num:int=18
		var rot:Number = 0;
		for (var i:int = 0; i < num; i++) 
		{
			rot+=360/num//20 pur 18;
			qbox.setDefault( { lineAlpha:0, fillAlpha:0 } );
			_anchor[i] = new Anchor(qbox, { 
				x:Math.sin(rot * Math.PI / 180) * params.radius + params.x, 
				y:Math.cos(rot * Math.PI / 180) * params.radius + params.y, 
				radius:0.1 }, 
				_anchor[i - 1], 
				rot);
			
			_anchor[i].truc = qbox.addCircle({x:x,y:y,radius:.2, density:.00, mass:0 })
			//if (i%4==1)_anchor[i].addCirc();
			
			_anchor[i].shape.m_radius = 0.01 * params.radius;
			if (i != 0) qbox.addJoint( { type:QuickBox2D.DISTANCE, a:_anchor[i-1].body, b:_anchor[i].body } );
			
		}
		
		qbox.addJoint( { type:QuickBox2D.DISTANCE, a:_anchor[0].body, b:_anchor[_anchor.length-1].body } );

	}
	
	
	var age:int=0
	private var rx:Number=rx = Math.random()*2-1
	private var	ry:Number=rx = Math.random()*2-1
	var eyeScale:Number=.5
	public function update():void 
	{
		var len:uint = _anchor.length;
		age++
			
		if (age%10==1){
			if (Math.random()>.5){
				rx = Math.random()*2-1
				ry = Math.random()*2-1
				for (var i:int = 0; i < len; i++) {
					//_anchor[i].vx += rx*4;
					//_anchor[i].vy += ry*4;
				}
			}
			
		}
				
		
		for ( i = 0; i < len; i++) {
			_anchor[i].update();
			_anchor[i].vx *= 0.75;
			_anchor[i].vy *= 0.75;
				
			_anchor[i].vx += rx/4;
			_anchor[i].vy += ry/4;
			_anchor[i].vx += .048
			_anchor[i].vy += .173;
						
		}
		
		
		graphics.clear();
		graphics.beginFill(params.fillColor, params.fillAlpha);
		graphics.lineStyle(1, params.lineColor, params.lineAlpha);
		var mx:Number = (_anchor[0].x + _anchor[1].x) * 15;
		var my:Number = (_anchor[0].y + _anchor[1].y) * 15;
		//centrum.x = mx;
		//centrum.y = my;
		//centrum.x = _anchor[1].x;
		//centrum.y = _anchor[1].y+0.2;
		
		graphics.moveTo(mx, my);
		for (i = 1; i < len - 1; i++) 
		{
			var r1:int=Math.floor(Math.random()*len)
			//var r2:int=Math.random()*len
			


			mx = (_anchor[i].x + _anchor[i + 1].x) * 15;
			my = (_anchor[i].y + _anchor[i + 1].y) * 15;
			graphics.curveTo(_anchor[i].x * 30, _anchor[i].y * 30, mx, my);
			//graphics.lineTo(_anchor[i].x * 30, _anchor[i].y * 30);
		}
		mx = (_anchor[0].x + _anchor[1].x) * 15;
		my = (_anchor[0].y + _anchor[1].y) * 15;
		graphics.curveTo(_anchor[len - 1].x * 30, _anchor[len - 1].y * 30, mx, my);
		//graphics.lineTo(_anchor[1].x * 30, _anchor[1].y * 30);
		
		/*
		if (i==r1){
			mx = (_anchor[i].x + _anchor[i+3].x) * 15;
			my = (_anchor[i].y + _anchor[i+3].y) * 15;
			graphics.beginFill(0xFFFFFF, 0.9);
			graphics.drawCircle(mx, my, 10*eyeScale);
			graphics.beginFill(0x0, 0.9);
			graphics.drawCircle(mx + 1, my, 5*eyeScale);
		}*/

		if (Math.random()<.95){
			mx = (_anchor[1].x + _anchor[11].x) * 15;
			my = (_anchor[1].y + _anchor[11].y) * 15;
			graphics.beginFill(0xFFFFFF, 0.9);
			graphics.drawCircle(mx + d1, my+d2, 10*eyeScale);
			graphics.beginFill(0x0, 0.9);
			graphics.drawCircle(mx + 1, my, 5*eyeScale);
		}
		if (Math.random()<.95){
			mx = (_anchor[2].x + _anchor[8].x) * 15;
			my = (_anchor[2].y + _anchor[8].y) * 15;
			graphics.beginFill(0xFFFFFF, 0.9);
			graphics.drawCircle(mx+d3, my+d4, 8*eyeScale);
			graphics.beginFill(0x0, 0.9);
			graphics.drawCircle(mx - 1, my, 4*eyeScale);
		}
		
	}
}
