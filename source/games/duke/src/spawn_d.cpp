//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment
Copyright (C) 2000, 2003 - Matt Saettler (EDuke Enhancements)
Copyright (C) 2020 - Christoph Oelckers

This file is part of Enhanced Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms

EDuke enhancements integrated: 04/13/2003 - Matt Saettler

Note: EDuke source was in transition.  Changes are in-progress in the
source as it is released.

*/
//-------------------------------------------------------------------------
#include <utility>
#include "ns.h"
#include "global.h"
#include "sounds.h"
#include "names_d.h"
#include "dukeactor.h"

BEGIN_DUKE_NS


DDukeActor* spawninit_d(DDukeActor* actj, DDukeActor* act, TArray<DDukeActor*>* actors)
{
	if (act->GetClass() != RUNTIME_CLASS(DDukeActor))
	{
		CallInitialize(act);
		return act;
	}
	auto sectp = act->sector();

	if (isWorldTour())
	{
		switch (act->spr.picnum)
		{
		case BOSS2STAYPUT:
		case BOSS3STAYPUT:
		case BOSS5STAYPUT:
			act->actorstayput = act->sector();
			[[fallthrough]];
		case FIREFLY:
		case BOSS5:
			if (act->spr.picnum != FIREFLY)
			{
				if (actj && isrespawncontroller(actj))
					act->spr.pal = actj->spr.pal;
				if (act->spr.pal != 0)
				{
					act->clipdist = 20;
					act->spr.scale = DVector2(0.625, 0.625);
				}
				else
				{
					act->spr.scale = DVector2(1.25, 1.25);
					act->clipdist = 41;
				}
			}
			else
			{
				act->spr.scale = DVector2(0.625, 0.625);
				act->clipdist = 20;
			}

			if (actj)
				act->spr.lotag = 0;

			if ((act->spr.lotag > ud.player_skill) || ud.monsters_off)
			{
				act->spr.scale = DVector2(0, 0);
				ChangeActorStat(act, STAT_MISC);
				break;
			}
			else
			{
				makeitfall(act);

				act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
				ps[connecthead].max_actors_killed++;

				if (actj) {
					act->timetosleep = 0;
					check_fta_sounds_d(act);
					ChangeActorStat(act, STAT_ACTOR);
				}
				else
					ChangeActorStat(act, STAT_ZOMBIEACTOR);
			}
			return act;
		case FIREFLYFLYINGEFFECT:
			act->SetOwner(actj);
			ChangeActorStat(act, STAT_MISC);
			act->spr.scale = DVector2(0.25, 0.25);
			return act;
		case LAVAPOOLBUBBLE:
			if (actj->spr.scale.X < 0.46875)
				return act;
			act->SetOwner(actj);
			ChangeActorStat(act, STAT_MISC);
			act->spr.pos.X += krandf(32) - 16;
			act->spr.pos.Y += krandf(32) - 16;
			act->spr.scale = DVector2(0.25, 0.25);
			return act;
		case WHISPYSMOKE:
			ChangeActorStat(act, STAT_MISC);
			act->spr.pos.X += krandf(16) - 8;
			act->spr.pos.Y += krandf(16) - 8;
			act->spr.scale = DVector2(0.3125, 0.3125);
			return act;
		case SERIOUSSAM:
			ChangeActorStat(act, STAT_ZOMBIEACTOR);
			act->spr.cstat = CSTAT_SPRITE_BLOCK_ALL;
			act->spr.extra = 150;
			return act;
		}
	}

	switch (act->spr.picnum)
	{
	default:
		spawninitdefault(actj, act);
		break;
	case FOF:
		act->spr.scale = DVector2(0, 0);
		ChangeActorStat(act, STAT_MISC);
		break;
	case WATERSPLASH2:
		if (actj)
		{
			SetActor(act, actj->spr.pos);
			double s = 0.125 + (krand() & 7) * REPEAT_SCALE;
			act->spr.scale = DVector2(s, s);
		}
		else
		{
			double s = 0.25 + (krand() & 15) * REPEAT_SCALE;
			act->spr.scale = DVector2(s, s);
		}

		act->spr.shade = -16;
		act->spr.cstat |= CSTAT_SPRITE_YCENTER;
		if (actj)
		{
			if (actj->sector()->lotag == 2)
			{
				act->spr.pos.Z = getceilzofslopeptr(act->sector(), act->spr.pos) + 16;
				act->spr.cstat |= CSTAT_SPRITE_YFLIP;
			}
			else if (actj->sector()->lotag == 1)
				act->spr.pos.Z = getflorzofslopeptr(act->sector(), act->spr.pos);
		}

		if (sectp->floorpicnum == FLOORSLIME ||
			sectp->ceilingpicnum == FLOORSLIME)
			act->spr.pal = 7;
		[[fallthrough]];
	case NEON1:
	case NEON2:
	case NEON3:
	case NEON4:
	case NEON5:
	case NEON6:
	case DOMELITE:
		if (act->spr.picnum != WATERSPLASH2)
			act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		[[fallthrough]];
	case NUKEBUTTON:
		if (act->spr.picnum == DOMELITE)
			act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		ChangeActorStat(act, STAT_MISC);
		break;
	case NATURALLIGHTNING:
		act->spr.cstat &= ~CSTAT_SPRITE_BLOCK_ALL;
		act->spr.cstat |= CSTAT_SPRITE_INVISIBLE;
		break;
	case TRANSPORTERSTAR:
	case TRANSPORTERBEAM:
		spawntransporter(actj, act, act->spr.picnum == TRANSPORTERBEAM);
		break;

	case FRAMEEFFECT1:
		if (actj)
		{
			act->spr.scale = actj->spr.scale;
			act->temp_data[1] = actj->spr.picnum;
		}
		else act->spr.scale = DVector2(0, 0);

		ChangeActorStat(act, STAT_MISC);

		break;

	case FORCESPHERE:
		if (!actj)
		{
			act->spr.cstat = CSTAT_SPRITE_INVISIBLE;
			ChangeActorStat(act, STAT_ZOMBIEACTOR);
		}
		else
		{
			act->spr.scale = DVector2(REPEAT_SCALE, REPEAT_SCALE);
			ChangeActorStat(act, STAT_MISC);
		}
		break;

	case BLOOD:
		act->spr.scale = DVector2(0.25, 0.25);
		act->spr.pos.Z -= 26;
		if (actj && actj->spr.pal == 6)
			act->spr.pal = 6;
		ChangeActorStat(act, STAT_MISC);
		break;
	case LAVAPOOL:
		if (!isWorldTour()) // Twentieth Anniversary World Tour
			return act;
		[[fallthrough]];

	case BLOODPOOL:
	case PUKE:
		if (spawnbloodpoolpart1(act)) break;

		if (actj && act->spr.picnum != PUKE)
		{
			if (actj->spr.pal == 1)
				act->spr.pal = 1;
			else if (actj->spr.pal != 6 && actj->spr.picnum != NUKEBARREL)
			{
				if (actj->spr.picnum == FECES)
					act->spr.pal = 7; // Brown
				else act->spr.pal = 2; // Red
			}
			else act->spr.pal = 0;  // green
		}
		act->spr.cstat |= CSTAT_SPRITE_ALIGNMENT_FLOOR;
		if (act->spr.picnum == LAVAPOOL)  // Twentieth Anniversary World Tour
		{
			act->spr.pos.Z = getflorzofslopeptr(act->sector(), act->spr.pos) - 0.78125;
		}
		[[fallthrough]];

	case FECES:
		if (actj)
			act->spr.scale = DVector2(REPEAT_SCALE, REPEAT_SCALE);
		ChangeActorStat(act, STAT_MISC);
		break;

	case BLOODSPLAT1:
	case BLOODSPLAT2:
	case BLOODSPLAT3:
	case BLOODSPLAT4:
		act->spr.cstat |= CSTAT_SPRITE_ALIGNMENT_WALL;
		act->spr.scale.X = (0.109375 + (krand() & 7) * REPEAT_SCALE);
		act->spr.scale.Y = (0.109375 + (krand() & 7) * REPEAT_SCALE);
		act->spr.pos.Z -= 16;
		if (actj && actj->spr.pal == 6)
			act->spr.pal = 6;
		insertspriteq(act);
		ChangeActorStat(act, STAT_MISC);
		break;

	case SPACEMARINE:
		act->spr.extra = 20;
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		ChangeActorStat(act, STAT_ZOMBIEACTOR);
		break;

	case HYDRENT:
	case SATELITE:
	case FUELPOD:
	case SOLARPANNEL:
	case ANTENNA:
	case BOTTLE1:
	case BOTTLE2:
	case BOTTLE3:
	case BOTTLE4:
	case BOTTLE5:
	case BOTTLE6:
	case BOTTLE7:
	case BOTTLE8:
	case BOTTLE10:
	case BOTTLE11:
	case BOTTLE12:
	case BOTTLE13:
	case BOTTLE14:
	case BOTTLE15:
	case BOTTLE16:
	case BOTTLE17:
	case BOTTLE18:
	case BOTTLE19:
	case OCEANSPRITE1:
	case OCEANSPRITE2:
	case OCEANSPRITE3:
	case OCEANSPRITE5:
	case MONK:
	case INDY:
	case LUKE:
	case JURYGUY:
	case FANSPRITE:
	case HANGLIGHT:
	case FETUS:
	case FETUSBROKE:
	case SUSHIPLATE1:
	case SUSHIPLATE2:
	case SUSHIPLATE3:
	case SUSHIPLATE4:
	case SUSHIPLATE5:
	case WAITTOBESEATED:
	case VASE:
	case PIPE1:
	case PIPE2:
	case PIPE3:
	case PIPE4:
	case PIPE5:
	case PIPE6:
		act->clipdist = 8;
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		[[fallthrough]];
	case OCEANSPRITE4:
		ChangeActorStat(act, 0);
		break;
	case FEMMAG1:
	case FEMMAG2:
		act->spr.cstat &= ~CSTAT_SPRITE_BLOCK_ALL;
		ChangeActorStat(act, 0);
		break;
	case DUKETAG:
	case SIGN1:
	case SIGN2:
		if (ud.multimode < 2 && act->spr.pal)
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
		}
		else act->spr.pal = 0;
		break;
	case MASKWALL1:
	case MASKWALL2:
	case MASKWALL3:
	case MASKWALL4:
	case MASKWALL5:
	case MASKWALL6:
	case MASKWALL7:
	case MASKWALL8:
	case MASKWALL9:
	case MASKWALL10:
	case MASKWALL11:
	case MASKWALL12:
	case MASKWALL13:
	case MASKWALL14:
	case MASKWALL15:
	{
		auto j = act->spr.cstat & (CSTAT_SPRITE_ALIGNMENT_MASK | CSTAT_SPRITE_XFLIP | CSTAT_SPRITE_YFLIP);
		act->spr.cstat = j | CSTAT_SPRITE_BLOCK;
		ChangeActorStat(act, 0);
		break;
	}
	case FOOTPRINTS:
	case FOOTPRINTS2:
	case FOOTPRINTS3:
	case FOOTPRINTS4:
		initfootprint(actj, act);
		break;

	case FEM1:
	case FEM2:
	case FEM3:
	case FEM4:
	case FEM5:
	case FEM6:
	case FEM7:
	case FEM8:
	case FEM9:
	case FEM10:
	case PODFEM1:
	case NAKED1:
	case STATUE:
	case TOUGHGAL:
		act->spr.yint = act->spr.hitag;
		act->spr.hitag = -1;
		if (act->spr.picnum == PODFEM1) act->spr.extra <<= 1;
		[[fallthrough]];

	case BLOODYPOLE:
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		act->clipdist = 8;
		ChangeActorStat(act, STAT_ZOMBIEACTOR);
		break;

	case DUKELYINGDEAD:
		if (actj && actj->isPlayer())
		{
			act->spr.scale = actj->spr.scale;
			act->spr.shade = actj->spr.shade;
			act->spr.pal = ps[actj->PlayerIndex()].palookup;
		}
		[[fallthrough]];
	case DUKECAR:
	case HELECOPT:
		act->spr.cstat = 0;
		act->spr.extra = 1;
		act->vel.X = 292 / 16.;
		act->vel.Z = 360 / 256.;
		[[fallthrough]];
	case BLIMP:
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		act->clipdist = 32;
		[[fallthrough]];
	case MIKE:
		if (act->spr.picnum == MIKE)
			act->spr.yint = act->spr.hitag;
		[[fallthrough]];
	case WEATHERWARN:
		ChangeActorStat(act, STAT_ACTOR);
		break;

	case SPOTLITE:
		break;
	case BULLETHOLE:
		act->spr.scale = DVector2(0.046875, 0.046875);
		act->spr.cstat = CSTAT_SPRITE_ALIGNMENT_WALL | randomFlip();
		insertspriteq(act);
		[[fallthrough]];
	case MONEY:
	case MAIL:
	case PAPER:
		if (act->spr.picnum == MONEY || act->spr.picnum == MAIL || act->spr.picnum == PAPER)
		{
			act->temp_data[0] = krand() & 2047;
			act->spr.cstat = randomFlip();
			act->spr.scale = DVector2(0.125, 0.125);
			act->spr.Angles.Yaw = randomAngle();
		}
		ChangeActorStat(act, STAT_MISC);
		break;

	case SHELL: //From the player
	case SHOTGUNSHELL:
		initshell(actj, act, act->spr.picnum == SHELL);
		break;

	case ONFIRE:
		// Twentieth Anniversary World Tour
		if (!isWorldTour())
			break;
		[[fallthrough]];
	case EXPLOSION2:
	case EXPLOSION2BOT:
	case BURNING:
	case BURNING2:
	case SMALLSMOKE:
	case SHRINKEREXPLOSION:
	case COOLEXPLOSION1:

		if (actj)
		{
			act->spr.Angles.Yaw = actj->spr.Angles.Yaw;
			act->spr.shade = -64;
			act->spr.cstat = CSTAT_SPRITE_YCENTER | randomXFlip();
		}

		if (act->spr.picnum == EXPLOSION2 || act->spr.picnum == EXPLOSION2BOT)
		{
			act->spr.scale = DVector2(0.75, 0.75);
			act->spr.shade = -127;
			act->spr.cstat |= CSTAT_SPRITE_YCENTER;
		}
		else if (act->spr.picnum == SHRINKEREXPLOSION)
		{
			act->spr.scale = DVector2(0.5, 0.5);
		}
		else if (act->spr.picnum == SMALLSMOKE || act->spr.picnum == ONFIRE)
		{
			// 64 "money"
			act->spr.scale = DVector2(0.375, 0.375);
		}
		else if (act->spr.picnum == BURNING || act->spr.picnum == BURNING2)
		{
			act->spr.scale = DVector2(0.0625, 0.0625);
		}

		if (actj)
		{
			double x = getflorzofslopeptr(act->sector(), act->spr.pos);
			if (act->spr.pos.Z > x - 12)
				act->spr.pos.Z = x - 12;
		}

		if (act->spr.picnum == ONFIRE)
		{
			act->spr.pos.X += krandf(32) - 16;
			act->spr.pos.Y += krandf(32) - 16;
			act->spr.pos.Z -= krandf(40);
			act->spr.cstat |= CSTAT_SPRITE_YCENTER;
		}

		ChangeActorStat(act, STAT_MISC);

		break;

	case PLAYERONWATER:
		if (actj)
		{
			act->spr.scale = actj->spr.scale;
			act->vel.Z = 0.5;
			if (act->sector()->lotag != 2)
				act->spr.cstat |= CSTAT_SPRITE_INVISIBLE;
		}
		ChangeActorStat(act, STAT_DUMMYPLAYER);
		break;

	case APLAYER:
	{
		act->spr.scale = DVector2(0, 0);
		int j = ud.coop;
		if (j == 2) j = 0;

		if (ud.multimode < 2 || (ud.multimode > 1 && j != act->spr.lotag))
			ChangeActorStat(act, STAT_MISC);
		else
			ChangeActorStat(act, STAT_PLAYER);
		break;
	}
	case WATERBUBBLE:
		if (actj && actj->isPlayer())
			act->spr.pos.Z -= 16;
		if (act->spr.picnum == WATERBUBBLE)
		{
			if (actj)
				act->spr.Angles.Yaw = actj->spr.Angles.Yaw;
			act->spr.scale = DVector2(0.0625, 0.0625);
		}
		else act->spr.scale = DVector2(0.5, 0.5);

		ChangeActorStat(act, STAT_MISC);
		break;

	case WATERDRIPSPLASH:
		act->spr.scale = DVector2(0.375, 0.375);
		ChangeActorStat(act, STAT_STANDABLE);
		break;

	case PLUG:
		act->spr.lotag = 9999;
		ChangeActorStat(act, STAT_STANDABLE);
		break;
	case WATERBUBBLEMAKER:
		if (act->spr.hitag && act->spr.picnum == WATERBUBBLEMAKER)
		{	// JBF 20030913: Pisses off move(), eg. in bobsp2
			Printf(TEXTCOLOR_YELLOW "WARNING: WATERBUBBLEMAKER %d @ %d,%d with hitag!=0. Applying fixup.\n", act->GetIndex(), int(act->spr.pos.X), int(act->spr.pos.Y));
			act->spr.hitag = 0;
		}
		act->spr.cstat |= CSTAT_SPRITE_INVISIBLE;
		ChangeActorStat(act, STAT_STANDABLE);
		break;
	case TARGET:
	case DUCK:
	case LETTER:
		act->spr.extra = 1;
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		ChangeActorStat(act, STAT_ACTOR);
		break;
	case OCTABRAINSTAYPUT:
	case LIZTROOPSTAYPUT:
	case PIGCOPSTAYPUT:
	case LIZMANSTAYPUT:
	case BOSS1STAYPUT:
	case PIGCOPDIVE:
	case COMMANDERSTAYPUT:
	case BOSS4STAYPUT:
		act->actorstayput = act->sector();
		[[fallthrough]];
	case BOSS1:
	case BOSS2:
	case BOSS3:
	case BOSS4:
	case ROTATEGUN:
	case GREENSLIME:
		if (act->spr.picnum == GREENSLIME)
			act->spr.extra = 1;
		[[fallthrough]];
	case DRONE:
	case LIZTROOPONTOILET:
	case LIZTROOPJUSTSIT:
	case LIZTROOPSHOOT:
	case LIZTROOPJETPACK:
	case LIZTROOPDUCKING:
	case LIZTROOPRUNNING:
	case LIZTROOP:
	case OCTABRAIN:
	case COMMANDER:
	case PIGCOP:
	case LIZMAN:
	case LIZMANSPITTING:
	case LIZMANFEEDING:
	case LIZMANJUMP:
	case ORGANTIC:
	case SHARK:

		if (act->spr.pal == 0)
		{
			switch (act->spr.picnum)
			{
			case LIZTROOPONTOILET:
			case LIZTROOPSHOOT:
			case LIZTROOPJETPACK:
			case LIZTROOPDUCKING:
			case LIZTROOPRUNNING:
			case LIZTROOPSTAYPUT:
			case LIZTROOPJUSTSIT:
			case LIZTROOP:
				act->spr.pal = 22;
				break;
			}
		}

		if (bossguy(act))
		{
			if (actj && isrespawncontroller(actj))
				act->spr.pal = actj->spr.pal;
			if (act->spr.pal && (!isWorldTour() || !(currentLevel->flags & LEVEL_WT_BOSSSPAWN) || act->spr.pal != 22))
			{
				act->clipdist = 20;
				act->spr.scale = DVector2(0.625, 0.625);
			}
			else
			{
				act->spr.scale = DVector2(1.25, 1.25);
				act->clipdist = 41;
			}
		}
		else
		{
			if (act->spr.picnum != SHARK)
			{
				act->spr.scale = DVector2(0.625, 0.625);
				act->clipdist = 20;
			}
			else
			{
				act->spr.scale = DVector2(0.9375, 0.9375);
				act->clipdist = 10;
			}
		}

		if (actj) act->spr.lotag = 0;

		if ((act->spr.lotag > ud.player_skill) || ud.monsters_off == 1)
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
			break;
		}
		else
		{
			makeitfall(act);

			act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
			if (act->spr.picnum != SHARK)
				ps[myconnectindex].max_actors_killed++;

			if (act->spr.picnum == ORGANTIC) act->spr.cstat |= CSTAT_SPRITE_YCENTER;

			if (actj)
			{
				act->timetosleep = 0;
				check_fta_sounds_d(act);
				ChangeActorStat(act, STAT_ACTOR);
			}
			else ChangeActorStat(act, STAT_ZOMBIEACTOR);
		}

		if (act->spr.picnum == ROTATEGUN)
			act->vel.Z = 0;

		break;

	case OOZ:
	case OOZ2:
	{
		act->spr.shade = -12;

		if (actj)
		{
			if (actj->spr.picnum == NUKEBARREL)
				act->spr.pal = 8;
			insertspriteq(act);
		}

		ChangeActorStat(act, STAT_ACTOR);

		getglobalz(act);

		double j = ((act->floorz - act->ceilingz) / 128.);

		act->spr.scale = DVector2(max(0., 0.390625 - j * 0.5), j);
		if (krand() & 4) act->spr.cstat |= CSTAT_SPRITE_XFLIP;

		break;
	}
	case HEAVYHBOMB:
		if (actj) act->SetOwner(actj);
		else act->SetOwner(act);

		act->spr.scale = DVector2(0.140625, 0.140625);
		act->spr.yint = 4;
		[[fallthrough]];
	case REACTOR2:
	case REACTOR:
	case RECON:
		if (initreactor(actj, act, act->spr.picnum == RECON)) return act;
		break;

	case FLAMETHROWERSPRITE:
	case FLAMETHROWERAMMO: // Twentieth Anniversary World Tour
		if (!isWorldTour())
			break;
		[[fallthrough]];

	case ATOMICHEALTH:
	case STEROIDS:
	case HEATSENSOR:
	case SHIELD:
	case AIRTANK:
	case TRIPBOMBSPRITE:
	case JETPACK:
	case HOLODUKE:

	case FIRSTGUNSPRITE:
	case CHAINGUNSPRITE:
	case SHOTGUNSPRITE:
	case RPGSPRITE:
	case SHRINKERSPRITE:
	case FREEZESPRITE:
	case DEVISTATORSPRITE:

	case SHOTGUNAMMO:
	case FREEZEAMMO:
	case HBOMBAMMO:
	case CRYSTALAMMO:
	case GROWAMMO:
	case BATTERYAMMO:
	case DEVISTATORAMMO:
	case RPGAMMO:
	case BOOTS:
	case AMMO:
	case AMMOLOTS:
	case COLA:
	case FIRSTAID:
	case SIXPAK:
		if (actj)
		{
			act->spr.lotag = 0;
			act->spr.pos.Z -= 32;
			act->vel.Z = -4;
			ssp(act, CLIPMASK0);
			if (krand() & 4) act->spr.cstat |= CSTAT_SPRITE_XFLIP;
		}
		else
		{
			act->SetOwner(act);
			act->spr.cstat = 0;
		}

		if ((ud.multimode < 2 && act->spr.pal != 0) || (act->spr.lotag > ud.player_skill))
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
			break;
		}

		act->spr.pal = 0;
		[[fallthrough]];

	case ACCESSCARD:

		if (act->spr.picnum == ATOMICHEALTH)
			act->spr.cstat |= CSTAT_SPRITE_YCENTER;

		if (ud.multimode > 1 && ud.coop != 1 && act->spr.picnum == ACCESSCARD)
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
			break;
		}
		else
		{
			if (act->spr.picnum == AMMO)
				act->spr.scale = DVector2(0.25, 0.25);
			else act->spr.scale = DVector2(0.5, 0.5);
		}

		act->spr.shade = -17;

		if (actj) ChangeActorStat(act, STAT_ACTOR);
		else
		{
			ChangeActorStat(act, STAT_ZOMBIEACTOR);
			makeitfall(act);
		}
		break;

	case FLOORFLAME:
		act->spr.shade = -127;
		ChangeActorStat(act, STAT_STANDABLE);
		break;

	case BOUNCEMINE:
		act->SetOwner(act);
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL; //Make it hitable
		act->spr.scale = DVector2(0.375, 0.375);
		act->spr.shade = -127;
		act->spr.extra = gs.impact_damage << 2;
		ChangeActorStat(act, STAT_ZOMBIEACTOR);
		break;

	case CAMERAPOLE:
		act->spr.extra = 1;

		if (gs.camerashitable) act->spr.cstat = CSTAT_SPRITE_BLOCK_ALL;
		else act->spr.cstat = 0;
		[[fallthrough]];

	case GENERICPOLE:

		if (ud.multimode < 2 && act->spr.pal != 0)
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
			break;
		}
		else act->spr.pal = 0;
		break;
	case STEAM:
		if (actj)
		{
			act->spr.Angles.Yaw = actj->spr.Angles.Yaw;
			act->spr.cstat = CSTAT_SPRITE_ALIGNMENT_WALL | CSTAT_SPRITE_YCENTER | CSTAT_SPRITE_TRANSLUCENT;
			act->spr.scale = DVector2(REPEAT_SCALE, REPEAT_SCALE);
			act->vel.X = -0.5;
			ssp(act, CLIPMASK0);
		}
		[[fallthrough]];
	case CEILINGSTEAM:
		ChangeActorStat(act, STAT_STANDABLE);
		break;

	case SECTOREFFECTOR:
		spawneffector(act, actors);
		break;

	case TOILET:
	case STALL:
		act->spr.lotag = 1;
		act->spr.cstat |= CSTAT_SPRITE_BLOCK_ALL;
		act->clipdist = 2;
		act->SetOwner(act);
		break;
	case RUBBERCAN:
		act->spr.extra = 0;
		[[fallthrough]];
	case EXPLODINGBARREL:
	case HORSEONSIDE:
	case FIREBARREL:
	case NUKEBARREL:
	case FIREVASE:
	case NUKEBARRELDENTED:
	case NUKEBARRELLEAKED:
	case WOODENHORSE:

		if (actj)
			act->spr.scale = DVector2(0.5, 0.5);
		act->clipdist = 18;
		makeitfall(act);
		if (actj) act->SetOwner(actj);
		else act->SetOwner(act);
		[[fallthrough]];

	case EGG:
		if (ud.monsters_off == 1 && act->spr.picnum == EGG)
		{
			act->spr.scale = DVector2(0, 0);
			ChangeActorStat(act, STAT_MISC);
		}
		else
		{
			if (act->spr.picnum == EGG)
			{
				act->clipdist = 6;
				ps[connecthead].max_actors_killed++;
			}
			act->spr.cstat = CSTAT_SPRITE_BLOCK_ALL | randomXFlip();
			ChangeActorStat(act, STAT_ZOMBIEACTOR);
		}
		break;
	case TOILETWATER:
		act->spr.shade = -16;
		ChangeActorStat(act, STAT_STANDABLE);
		break;
	}
	return act;
}

END_DUKE_NS
