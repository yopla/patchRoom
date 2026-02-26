// forked from rsakane's 歯車くるくる
/*
 * 鉛が詰まったときは、
 * 歯車をマウスで動かせば取り出せるかも。
 */
package
{
	import flash.display.MovieClip;
	import flash.events.Event;
	import com.actionsnippet.qbox.objects.*;
	import com.actionsnippet.qbox.*;
	
	public class Main extends MovieClip
	{
		private var sim:QuickBox2D;
		private var pos:Array = 
		[
			[1.0,   1.7, 7.5, 1.0,  3.0, 7, 0.0, 2.5, 10],
			[1.9,   6.9, 5.0, 0.4,  2.0, 5, 0.2, 2.7, 10],
			[5.85,  5.4, 4.5, 0.83, 1.3, 4, 0.2, 2.7, 10],
			[9.9,  10.5, 8.0, 1.4,  2.0, 2, 0.2, 5.4, 70]
		];
		
		private var gear:QuickObject;
		private var frame:int = 0;
		private var balls:Vector.<QuickObject> = new Vector.<QuickObject>();
		
		public function Main()
		{
			sim = new QuickBox2D(this);
			sim.setDefault({fillColor:0x393939, lineAlpha:0, radius:1.5});
			
			for (var i:int = 0; i < pos.length; i++)
			{
				gear = Shape.createGear(sim, pos[i][0], pos[i][1], pos[i][2], pos[i][3], pos[i][4], pos[i][5], pos[i][6], pos[i][7], pos[i][8]);
			}
			
			sim.mouseDrag();
			sim.start();
			
			addEventListener(Event.ENTER_FRAME, onEnterFrame);
		}
		
		public function onEnterFrame(event:Event):void
		{
			if (frame++ % 40 == 0) balls.push(sim.addCircle( { x:Math.random() * 6 + 2, y: -5, radius:0.1, density:1000 } ));
			
			for (var i:int = 0; i < balls.length; i++)
			{
				if (balls[i].body.GetPosition().y >= 18)
				{
					balls[i].destroy();
					balls.splice(i--, 1);
				}
			}
		}
	}
}

import com.actionsnippet.qbox.QuickObject;
import com.actionsnippet.qbox.QuickBox2D;

class Shape
{
	public static function createGear(sim:QuickBox2D, x:Number, y:Number, width:int, height:Number, radius:Number, num:int, angle:Number, scale:Number, density:Number):QuickObject
	{
		var gear:QuickObject = sim.addCircle( { radius:radius } );
		var group:Array = [gear];
		for (var degree:Number = 0; degree < 180; degree += 180 / num)
		{
			var rect:QuickObject = sim.addBox( { width:width, height:height, angle:degree * Math.PI / 180, density:density } );
			group.push(rect);
		}
		gear = sim.addGroup( { objects:group } );
		gear.x = x
		gear.y = y;
		gear.angle = angle;
		
		sim.addJoint( { type:"revolute", a:gear.body, b:sim.w.GetGroundBody() } );
		return gear;
	}
}