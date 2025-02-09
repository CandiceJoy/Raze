class DukeTrash : DukeActor
{
	default
	{
		ScaleX 0.375;
		ScaleY 0.375;
		statnum STAT_STANDABLE;
		pic "TRASH";
	}
	
	override void Initialize()
	{
		self.angle = frandom(0, 360);
	}
	
	override void Tick()
	{
		if (self.vel.X == 0) self.vel.X = 1 / 16.;
		if (self.DoMove(CLIPMASK0))
		{
			self.makeitfall();
			if (random(0, 1)) self.vel.Z -= 1;
			if (abs(self.vel.X) < 3)
				self.vel.X += random(0, 3) / 16.;
		}
		else self.Destroy();
	}
}


