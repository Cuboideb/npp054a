/*
 * File: attack.c
 * Purpose: Attacking (both throwing and melee) code
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *                    Jeff Greene, Diego Gonzalez
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */
#include "angband.h"

#include "game-cmd.h"
#include "cmds.h"

/*
 * Determine if the player "hits" a monster.
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	/* Penalize invisible targets */
	if (!vis) chance = chance / 2;

	/* Power competes against armor */
	if ((chance > 0) && (randint0(chance) >= (ac * 3 / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}




/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
static int tot_dam_aux(const object_type *o_ptr, int tdam, const monster_type *m_ptr, bool is_weapon)
{
	int mult = 1;

	int y = m_ptr->fy;
	int x = m_ptr->fx;

	int terrain_flag = 0;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	u32b f1, f2, f3, fn;

	/* Get the feature */
	u32b element = cave_ff3_match(y, x, TERRAIN_MASK);

	/* Find out if monster is native to terrain */
	bool is_native = is_monster_native(y, x, r_ptr);

	/* Find out if monster is flying over terrain */
	bool is_flying = (m_ptr->mflag & (MFLAG_FLYING)) != 0;

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3, &fn);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			/* Slay Animal */
			if ((f1 & (TR1_SLAY_ANIMAL)) &&
			    (r_ptr->flags3 & (RF3_ANIMAL)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_ANIMAL);
				}

				if (mult < 2) mult = 2;
			}

			/* Slay Evil */
			if ((f1 & (TR1_SLAY_EVIL)) &&
			    (r_ptr->flags3 & (RF3_EVIL)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_EVIL);
				}

				if (mult < 2) mult = 2;
			}

			/* Slay Undead */
			if ((f1 & (TR1_SLAY_UNDEAD)) &&
			    (r_ptr->flags3 & (RF3_UNDEAD)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_UNDEAD);
				}

				if (mult < 3) mult = 3;
			}

			/* Slay Demon */
			if ((f1 & (TR1_SLAY_DEMON)) &&
			    (r_ptr->flags3 & (RF3_DEMON)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_DEMON);
				}

				if (mult < 3) mult = 3;
			}

			/* Slay Orc */
			if ((f1 & (TR1_SLAY_ORC)) &&
			    (r_ptr->flags3 & (RF3_ORC)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_ORC);
				}

				if (mult < 3) mult = 3;
			}

			/* Slay Troll */
			if ((f1 & (TR1_SLAY_TROLL)) &&
			    (r_ptr->flags3 & (RF3_TROLL)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_TROLL);
				}

				if (mult < 3) mult = 3;
			}

			/* Slay Giant */
			if ((f1 & (TR1_SLAY_GIANT)) &&
			    (r_ptr->flags3 & (RF3_GIANT)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_GIANT);
				}

				if (mult < 3) mult = 3;
			}

			/* Slay Dragon */
			if ((f1 & (TR1_SLAY_DRAGON)) &&
			    (r_ptr->flags3 & (RF3_DRAGON)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_DRAGON);
				}

				if (mult < 3) mult = 3;
			}

			/* Execute Dragon */
			if ((f1 & (TR1_KILL_DRAGON)) &&
			    (r_ptr->flags3 & (RF3_DRAGON)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_DRAGON);
				}

				if (mult < 5) mult = 5;
			}

			/* Execute demon */
			if ((f1 & (TR1_KILL_DEMON)) &&
			    (r_ptr->flags3 & (RF3_DEMON)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_DEMON);
				}

				if (mult < 5) mult = 5;
			}

			/* Execute undead */
			if ((f1 & (TR1_KILL_UNDEAD)) &&
			    (r_ptr->flags3 & (RF3_UNDEAD)))
			{
				if (m_ptr->ml)
				{
					l_ptr->r_l_flags3 |= (RF3_UNDEAD);
				}

				if (mult < 5) mult = 5;
			}

			if ((p_ptr->timed[TMD_SLAY_ELEM]) && (is_weapon))
			{
				/*First, Mark all resists in the lore if applicable*/
				if (r_ptr->flags3 & (RF3_IM_ELEM))
				{
					if (m_ptr->ml)
					{
						u32b flags = r_ptr->flags3;

						/*Just the elemental flags*/
						flags &= RF3_IM_ELEM;

						l_ptr->r_l_flags3 |= flags;
					}
				}

				/*Now increase the damage, but only by two if they don't resist any of the elements.*/
				if ((r_ptr->flags3 & (RF3_IM_ELEM)) != (RF3_IM_ELEM))
				{
					if (mult < 2 ) mult = 2;
				}
			}

			/* Brand (Poison) */
			if (f1 & (TR1_BRAND_POIS))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & (RF3_IM_POIS))
				{
					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_IM_POIS);
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 3) mult = 3;
				}
			}

			/* Brand (Acid) */
			if (f1 & (TR1_BRAND_ACID))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & (RF3_IM_ACID))
				{
					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_IM_ACID);
					}
				}
				/* Otherwise, take the damage */
				/* Increase damage when standing in acid */
				else if (!is_native && !is_flying && (element == ELEMENT_ACID))
				{
					/* A deep feature increases damage even more */
					if (cave_ff2_match(y, x, FF2_DEEP))
					{
						if (mult < 5)
						{
							mult = 5;
							terrain_flag = 1;
						}
					}
					else
					{
						if (mult < 4)
						{
						      	mult = 4;
							terrain_flag = 1;
						}
					}
				}
				else
				{
					if (mult < 3) mult = 3;
				}
			}

			/* Brand (Elec) */
			if (f1 & (TR1_BRAND_ELEC))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & (RF3_IM_ELEC))
				{
					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_IM_ELEC);
					}
				}
				/* Otherwise, take the damage */
				/* Water increases damage */
				else if (!is_native && !is_flying &&
					((element == ELEMENT_WATER) || (element == ELEMENT_BWATER)))
				{
					/* A deep feature increases damage even more */
					if (cave_ff2_match(y, x, FF2_DEEP))
					{
						if (mult < 5)
						{
						      	mult = 5;
							terrain_flag = 1;
						}
					}
					else
					{
						if (mult < 4)
						{
						       	mult = 4;
							terrain_flag = 1;
						}
					}
				}
				else
				{
					if (mult < 3) mult = 3;
				}
			}

			/* Brand (Fire) */
			if (f1 & (TR1_BRAND_FIRE))
			{
				int my_mult = 0;
				int my_terrain_flag = 0;

				/* Notice immunity */
				if (r_ptr->flags3 & (RF3_IM_FIRE))
				{
					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_IM_FIRE);
					}
				}
				/* Flying monsters don't take damage from terrain */
				else if (is_flying)
				{
					my_mult = 3;
				}
				/* Otherwise, take the damage */
				/* Lava increases damage a lot */
				else if (!is_native && (element == ELEMENT_LAVA))
				{
					my_mult = 5;
					my_terrain_flag = 1;
				}
				/* Heat increases damage */
				else if (!is_native &&
					((element == ELEMENT_FIRE) || (element == ELEMENT_BMUD) ||
						(element == ELEMENT_BWATER)))
				{
					my_mult = 4;
					my_terrain_flag = 1;
				}
				/* Water decreases damage */
				else if (element == ELEMENT_WATER)
				{
					my_mult = 2;
					my_terrain_flag = -1;
				}
				/* Default damage increase */
				else
				{
					my_mult = 3;
				}

				/* Succeptable */
				if (r_ptr->flags3 & (RF3_HURT_FIRE))
				{
					/* Extra Damage */
					if (my_mult > 0) ++my_mult;

					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_HURT_FIRE);
					}
				}

				/* Override multiplier if brand damage is bigger */
				if (my_mult > mult)
				{
					mult = my_mult;
					terrain_flag = my_terrain_flag;
				}
			}

			/* Brand (Cold) */
			if (f1 & (TR1_BRAND_COLD))
			{
				int my_mult = 0;
				int my_terrain_flag = 0;

				/* Notice immunity */
				if (r_ptr->flags3 & (RF3_IM_COLD))
				{
					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_IM_COLD);
					}
				}
				/* Flying monsters don't take damage from terrain */
				else if (is_flying)
				{
					my_mult = 3;
				}
				/* Otherwise, take the damage */
				/* Ice increases damage a lot */
				else if (!is_native && (element == ELEMENT_ICE))
				{
					my_mult = 5;
					my_terrain_flag = 1;
				}
				/* Water increases damage */
				else if (!is_native && (element == ELEMENT_WATER))
				{
					my_mult = 4;
					my_terrain_flag = 1;
				}
				/* Lava nullifies cold brand  */
				else if (element == ELEMENT_LAVA)
				{
					my_terrain_flag = -1;
				}
				/* Heat decreases damage */
				else if ((element == ELEMENT_FIRE) ||
					(element == ELEMENT_BMUD) ||
					(element == ELEMENT_BWATER))
				{
					my_mult = 2;
					my_terrain_flag = -1;
				}
				/* Default damage increase */
				else
				{
					my_mult = 3;
				}

				/* Succeptable */
				if (r_ptr->flags3 & (RF3_HURT_COLD))
				{
					/* Extra damage */
					if (my_mult > 0) ++my_mult;

					if (m_ptr->ml)
					{
						l_ptr->r_l_flags3 |= (RF3_HURT_COLD);
					}
				}

				/* Override multiplier if brand damage is bigger */
				if (my_mult > mult)
				{
				       	mult = my_mult;
					terrain_flag = my_terrain_flag;
				}
			}

			break;
		}
	}

	/* To emulate fractional multiplier due to brigand combat */
	mult *= 100;

	/* A brigand can poison shots sometimes, if monster is not immune */
	if ((o_ptr->tval == TV_SHOT) &&
		(cp_ptr->flags & (CF_BRIGAND_COMBAT)) &&
		!(r_ptr->flags3 & (RF3_IM_POIS)))
	{
		int m = 0;
		char *message = "Your killer arts allowed you to create a poisoned shot!";

		/* Bigger chance to get a poisoned shot if the monster is sleeping */
		if (m_ptr->m_timed[MON_TMD_SLEEP])
		{
			/* Chance and multiplier vary with player level */
			if (rand_int(100) < (50 + 25 * p_ptr->lev / 25))
			{
				if (p_ptr->lev >= 50) m = 350;
				else if (p_ptr->lev >= 47) m = 300;
				else if (p_ptr->lev >= 40) m = 250;
				else if (p_ptr->lev >= 25) m = 200;
				else m = 150;

				/* message = "Poison damage (SLEEP)!"; */
			}
		}
		/* Monster is awake */
		else if (rand_int(100) < p_ptr->lev)
		{
			/* Chance and multiplier vary with player level */
			if (p_ptr->lev >= 35) m = 160;
			else m = 140;
		}

		/* Multiplier is bigger than the current */
		if ((m > 0) && (mult < m))
		{
			/* Save multiplier */
			mult = m;
			/* Cancel terrain bonuses */
			terrain_flag = 0;
			/* Message */
			msg_print(message);
			/* msg_format("%d %d.", tdam, (tdam * mult) / 100); */
		}
	}

	/* Show a message if necessary */
	if ((terrain_flag != 0) && player_can_see_bold(y, x) && !p_ptr->timed[TMD_BLIND])
	{
		char name[80];

		/* Get the feature's name */
		feature_desc(name, sizeof(name), cave_feat[y][x], FALSE, TRUE);

		/* Damage was increased */
		if (terrain_flag > 0)
		{
			msg_format("The %s increased the damage of the weapon!", name);
		}
		/* Damage was decreased */
		else
		{
			msg_format("The %s reduced the damage of the weapon!", name);
		}
	}

	/* Return the total damage */
	return ((tdam * mult) / 100);
}
/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level, bow skill.
 */
static int critical_shot(int weight, int plus, int dam, bool throw)
{
	int i, k;

	int bonus = (throw ? p_ptr->state.skills[SKILL_TO_HIT_THROW] : p_ptr->state.skills[SKILL_TO_HIT_BOW]);

	/* Extract "shot" power */
	i = (weight + ((p_ptr->state.to_h + plus) * 4) + (p_ptr->lev * 2));
	i += bonus;

	/* Critical hit */
	if (randint(5000) <= i)
	{
		k = weight + randint(500 + bonus);

		if (k < 500)
		{
			msg_print("It was a good hit!");
			dam = 2 * dam + 5;
		}
		else if (k < 1000)
		{
			msg_print("It was a great hit!");
			dam = 2 * dam + 10;
		}
		else
		{
			msg_print("It was a superb hit!");
			dam = 3 * dam + 15;
		}
	}

	return (dam);
}




/*
 * Determines the odds of an object breaking when thrown at a monster
 *
 * Note that artifacts never break, see the "drop_near()" function.
 */
static int breakage_chance(const object_type *o_ptr)
{
	/* Examine the item type */
	switch (o_ptr->tval)
	{
		/* Always break */
		case TV_FLASK:
		case TV_POTION:
		case TV_BOTTLE:
		case TV_FOOD:
		case TV_JUNK:
		{
			return (100);
		}

		/* Often break */
		case TV_LIGHT:
		case TV_SCROLL:
		case TV_SKELETON:
		{
			return (50);
		}

		/* Sometimes break */
		case TV_ARROW:
		{
			return (35);
		}

		/* Sometimes break */
		case TV_WAND:
		case TV_SHOT:
		case TV_BOLT:
		case TV_SPIKE:
		{
			return (25);
		}
	}

	/* Rarely break */
	return (10);
}

/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
static bool critical_norm(int weight, int plus, int *dd, int *ds)
{
	int i, k;

	/* Extract "blow" power */
	i = weight + ((p_ptr->state.to_h + plus) * 5);
	i += p_ptr->state.skills[SKILL_TO_HIT_MELEE];

	/* Chance */
	if (randint(5000) <= i)
	{
		k = weight + randint((450 + p_ptr->state.skills[SKILL_TO_HIT_MELEE]));

		if (k < 400)
		{
			sound(MSG_HIT_GOOD);
			msg_print("It was a good hit!");
			*dd *= 2;
			*ds += 2;
		}
		else if (k < 700)
		{
			sound(MSG_HIT_GREAT);
			msg_print("It was a great hit!");
			*dd *= 2;
			*ds += 4;
		}
		else if (k < 900)
		{
			sound(MSG_HIT_SUPERB);
			msg_print("It was a superb hit!");
			*dd *= 3;
			*ds += 6;
		}
		else if (k < 1300)
		{
			sound(MSG_HIT_HI_GREAT);
			msg_print("It was a *GREAT* hit!");
			*dd *= 3;
			*ds += 8;
		}
		else
		{
			sound(MSG_HIT_HI_SUPERB);
			msg_print("It was a *SUPERB* hit!");
			*dd *= 7;
			*ds += (*dd % 2);
			*dd /= 2;
			*ds += 10;
		}
	}

	else
	{
		sound(MSG_HIT);
		return (FALSE);
	}

	return (TRUE);
}



/*
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
bool check_hit(int power)
{
	return test_hit(power, p_ptr->state.ac + p_ptr->state.to_a, TRUE);
}



/*
 * Attack the monster at the given location
 *
 * If no "weapon" is available, then "punch" the monster one time.
 * Note p_ptr->base_energy_use needs to be declared before this function is called,
 * so energy use can be reduced if the player doesn't use full energy.
 */
void py_attack(int y, int x)
{
	int num = 0, k, bonus, chance;
	int hits = 0;

	int sleeping_bonus = 0;

	monster_type *m_ptr;
	monster_race *r_ptr;
	monster_lore *l_ptr;

	object_type *o_ptr;

	char m_name[80];

	bool fear = FALSE;

	bool do_quake = FALSE;

	bool was_asleep = FALSE;

	/* Get the monster */
	m_ptr = &mon_list[cave_m_idx[y][x]];
	r_ptr = &r_info[m_ptr->r_idx];
	l_ptr = &l_list[m_ptr->r_idx];

	/* Reveal minics and hidden creatures (note: mimics cannot be sneak-attacked) */
	if (((m_ptr->mimic_k_idx) && (m_ptr->ml)) || (m_ptr->mflag & (MFLAG_HIDE)))
	{
		/* Mimic no longer acts as a detected object */
		m_ptr->mflag &= ~(MFLAG_MIMIC | MFLAG_HIDE);

		/*no longer a mimic*/
		m_ptr->mimic_k_idx = 0;

		/* Get monster name ("a kobold") */
		monster_desc(m_name, sizeof(m_name), m_ptr, 0x88);

		/* And reveal */
		update_mon(cave_m_idx[m_ptr->fy][m_ptr->fx], FALSE);

		/* Redraw */
		light_spot(m_ptr->fy, m_ptr->fx);

		/* Message */
		if (!p_ptr->timed[TMD_AFRAID])
		{
			msg_format("You find yourself fighting %s!", m_name);

		}
		else
		{

			msg_format("%^s appears, but you are too frightened to fight it!",
				m_name);
			p_ptr->p_energy_use = 0;

			return;
		}

	}

	/*record if monster was sleeping before waking*/
	if (m_ptr->m_timed[MON_TMD_SLEEP]) was_asleep = TRUE;

	/* Disturb the monster */
	mon_clear_timed(get_mon_idx(m_ptr), MON_TMD_SLEEP, MON_TMD_FLG_NOMESSAGE);

	/*possibly update the monster health bar*/
	if (p_ptr->health_who == cave_m_idx[y][x]) p_ptr->redraw |= (PR_HEALTH);

	/* Disturb the player */
	disturb(0, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, sizeof(m_name), m_ptr, 0);

	/* Auto-Recall if possible and visible */
	if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

	/* Track a new monster */
	if (m_ptr->ml) health_track(cave_m_idx[y][x]);

	/* Handle player fear */
	if (p_ptr->timed[TMD_AFRAID])
	{
		/* Message */
		msg_format("You are too afraid to attack %s!", m_name);
		p_ptr->p_energy_use = 0;
		/* Done */
		return;
	}

	/* Get the weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Calculate the "attack quality" */
	bonus = p_ptr->state.to_h + o_ptr->to_h;

	/*
	 * If the monster is sleeping and visible, it can be hit more easily.
	 * Especially by Rogues
	 */

	if ((was_asleep) && (m_ptr->ml))
	{
		sleeping_bonus =  5 + p_ptr->lev / 5;

		if (cp_ptr->flags & CF_ROGUE_COMBAT)
		{
			/*50 % increase*/
			sleeping_bonus *= 3;
			sleeping_bonus /= 2;
		}
	}
	chance = (p_ptr->state.skills[SKILL_TO_HIT_MELEE] + (bonus * BTH_PLUS_ADJ) + sleeping_bonus);

	/*Mark the monster as attacked*/
	m_ptr->mflag |= (MFLAG_HIT_BY_MELEE);

	/* Attack once for each legal blow */
	while (num++ < p_ptr->state.num_blow)
	{
		int mon_ac = r_ptr->ac;

		/* Monster is different if desperate */
		if (m_ptr->mflag & (MFLAG_DESPERATE))
		{
			if (mon_ac < 10) mon_ac = 0;
			else mon_ac -= 10;
		}

		/*Adjust for player terrain*/
		chance = feat_adjust_combat_for_player(chance, FALSE);

		/*Adjust for monster terrain*/
		chance = feat_adjust_combat_for_monster(m_ptr, chance, TRUE);

		/* Some monsters are great at dodging  -EZ- */
		if ((r_ptr->flags2 & (RF2_EVASIVE)) && (!was_asleep) &&
			(!m_ptr->m_timed[MON_TMD_STUN]) && (!m_ptr->m_timed[MON_TMD_CONF]) &&
			(!m_ptr->m_timed[MON_TMD_FEAR]) && (one_in_(2)))
		{
			message_format(MSG_MISS, 0, "%^s evades your blow!",
				m_name);

			/* Learn that monster can dodge */
			l_ptr->r_l_flags2 |= (RF2_EVASIVE);

			continue;
		}

		/* Test for hit */
		else if (test_hit(chance, mon_ac, m_ptr->ml))
		{

			if (was_asleep)
			{
				if (cp_ptr->flags & CF_ROGUE_COMBAT)
				{
					message_format(MSG_GENERIC, m_ptr->r_idx,
						"You ruthlessly sneak attack %s!", m_name);
				}
				else
				{
					message_format(MSG_GENERIC, m_ptr->r_idx,
						"You sneak attack %s!", m_name);
				}
			}

			else
			{
				/* Message */
				message_format(MSG_GENERIC, m_ptr->r_idx, "You hit %s.", m_name);
			}

			/* If this was the first hit, make some noise */
			hits++;
			if (hits == 1) add_wakeup_chance += p_ptr->base_wakeup_chance;

			/* Hack -- bare hands do one damage */
			k = 1;

			/* Handle normal weapon */
			if (o_ptr->k_idx)
			{
				int dd = o_ptr->dd;
				int ds = o_ptr->ds;

				bool reroll = critical_norm(o_ptr->weight, o_ptr->to_h, &dd, &ds);

				k = damroll(dd, ds);

				if (reroll)
				{
					int k2 = damroll(dd, ds);
					if (k2 > k) k = k2;
				}
				k = tot_dam_aux(o_ptr, k, m_ptr, TRUE);
				if (p_ptr->state.impact && (k > 50)) do_quake = TRUE;
				k += o_ptr->to_d;
			}

			/* Apply the player damage bonuses */
			k += p_ptr->state.to_d;

			/* No negative damage */
			if (k < 0) k = 0;

			/* Complex message */
			if (p_ptr->wizard)
			{
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
			}

			/* Damage, check for fear and death */
			if (mon_take_hit(cave_m_idx[y][x], k, &fear, NULL, SOURCE_PLAYER))
			{
				/*return energy from unused attacks*/
				if (num < p_ptr->state.num_blow)
				{
					p_ptr->p_energy_use -= (((p_ptr->state.num_blow - (num)) * BASE_ENERGY_MOVE ) /
							p_ptr->state.num_blow);
				}
				break;
			}

			/* Confusion attack */
			if (p_ptr->confusing)
			{
				/* Cancel glowing hands */
				p_ptr->confusing = FALSE;
						/* Message */
				msg_print("Your hands stop glowing.");
						/* Confuse the monster */
				mon_inc_timed(get_mon_idx(m_ptr), MON_TMD_CONF,
						(10 + rand_int(p_ptr->lev) / 10), (MON_TMD_FLG_NOTIFY));
			}
		}

		/* Player misses */
		else
		{
			/* Message */
			message_format(MSG_MISS, m_ptr->r_idx, "You miss %s.", m_name);
		}
	}

	/* Mega-Hack -- apply earthquake brand */
	if (do_quake) earthquake(p_ptr->py, p_ptr->px, 10, TRUE);

	return;
}




/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * You must use slings + pebbles/shots, bows + arrows, xbows + bolts.
 *
 * See "calc_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a monster.
 *
 * Ranger  s (with Bows) and Anyone (with "Extra Shots") get extra shots.
 *
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 *
 * Note that Bows of "Extra Shots" give an extra shot.
 */
void do_cmd_fire(cmd_code code, cmd_arg args[])
{
	int dir, item;
	int i, j, y, x, ty, tx;
	int tdam, tdis, thits, tmul;
	int bonus, chance;

	object_type *o_ptr;
	object_type *j_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	bool hit_body = FALSE;

	byte missile_attr;
	char missile_char;

	char o_name[80];

	int msec = op_ptr->delay_factor * op_ptr->delay_factor;

	int path_n;
	u16b path_g[PATH_SIZE];
	u16b path_gx[PATH_SIZE];

	/* Get the "bow" (if any) */
	j_ptr = &inventory[INVEN_BOW];

	/* Require a usable launcher */
	if (!j_ptr->tval || !p_ptr->state.ammo_tval)
	{
		msg_print("You have nothing to fire with.");
		return;
	}

	/* Get item to fire and direction to fire in. */
	item = args[0].item;
	dir = args[1].direction;

	/* Check the item being fired is usable by the player. */
	if (!item_is_available(item, NULL, (USE_EQUIP | USE_INVEN | USE_FLOOR | USE_QUIVER | QUIVER_FIRST)))
	{
		msg_format("That item is not within your reach.");
		return;
	}

	/* Get the object for the ammo */
	o_ptr = object_from_item_idx(item);

	/* A cursed quiver disables the use of non-cursed ammo */
	if (IS_QUIVER_SLOT(item) && p_ptr->cursed_quiver && !cursed_p(o_ptr))
	{
		msg_print("Your quiver is cursed!");
		return;
	}

	/* Check the ammo can be used with the launcher */
	if (o_ptr->tval != p_ptr->state.ammo_tval)
	{
		msg_format("That ammo cannot be fired by your current weapon.");
		return;
	}

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain a local object */
	object_copy(i_ptr, o_ptr);

	/* Single object, not marked */
	i_ptr->number = 1;
	i_ptr->obj_in_use = FALSE;

	if (IS_QUIVER_SLOT(item))
	{
		/*Mark it to go in the quiver */
		i_ptr->ident |= (IDENT_QUIVER);
	}

	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}


	/* Sound */
	sound(MSG_SHOOT);

	/* Describe the object */
	object_desc(o_name, sizeof(o_name), i_ptr, ODESC_FULL | ODESC_SINGULAR);

	/* Cursed ammunition can hurt the player sometimes */
	if (IS_QUIVER_SLOT(item) && cursed_p(i_ptr) && (rand_int(100) < 70))
	{
		/* Get amount of damage */
		int dam = damroll(i_ptr->dd, i_ptr->ds) + ABS(i_ptr->to_d) + ABS(j_ptr->to_d) + ABS(p_ptr->state.to_d);

		/* Message */
		msg_format("The %s releases its curse on you!", o_name);

		/* Hurt the player */
		project_p(SOURCE_OTHER, p_ptr->py, p_ptr->px, dam, GF_NETHER, "firing a cursed projectile");

		return;
	}

	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(i_ptr);
	missile_char = object_char(i_ptr);

	/* Use the proper number of shots */
	thits = p_ptr->state.num_fire;

	/* Base damage from thrown object plus launcher bonus */
	tdam = damroll(i_ptr->dd, i_ptr->ds) + i_ptr->to_d + j_ptr->to_d;

	/* Actually "fire" the object */
	bonus = (p_ptr->state.to_h + i_ptr->to_h + j_ptr->to_h);
	chance = (p_ptr->state.skills[SKILL_TO_HIT_BOW] + (bonus * BTH_PLUS_ADJ));

	/* Assume a base multiplier */
	tmul = p_ptr->state.ammo_mult;

	/* Boost the damage */
	tdam *= tmul;

	/* Base range XXX XXX */
	tdis = 10 + 5 * tmul;

	/* Take a (partial) turn */
	p_ptr->p_energy_use = (BASE_ENERGY_MOVE / thits);

	/* Start at the player */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Predict the "target" location */
	ty = p_ptr->py + 99 * ddy[dir];
	tx = p_ptr->px + 99 * ddx[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay())
	{
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;
	}

	/* Calculate the path */
	path_n = project_path(path_g, path_gx, tdis, p_ptr->py, p_ptr->px, &ty, &tx, 0);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		/* Hack -- Stop before hitting walls */
		if (!cave_project_bold(ny, nx) && !cave_passable_bold(ny, nx)) break;

		/* Advance */
		x = nx;
		y = ny;

		/* Only do visuals if the player can "see" the missile */
		if (player_can_see_bold(y, x))
		{
			/* Hack, get the appropriate arrow graphics for david gervais and adam bolt's graphics*/
			if (((i_ptr->tval == TV_ARROW) || (i_ptr->tval == TV_BOLT)) &&
				 (use_graphics) && ((arg_graphics == GRAPHICS_DAVID_GERVAIS) || (arg_graphics == GRAPHICS_ADAM_BOLT)))
			{
				int yy, xx;
				u16b pict;

				if (!i)
				{
					yy = p_ptr->py;
					xx = p_ptr->px;
				}
				else
				{
					yy = GRID_Y(path_g[i-1]);
					xx = GRID_X(path_g[i-1]);
				}

				pict = bolt_pict(yy, xx, ny, nx, GF_ARROW, PROJECT_AMMO);

				missile_attr = PICT_A(pict);
				missile_char = PICT_C(pict);

				/* Use the other DVG set for bolts */
				if (i_ptr->tval == TV_BOLT) missile_char += 8;
			}

			/* Visual effects */
			print_rel(missile_char, missile_attr, y, x);
			move_cursor_relative(y, x);
			Term_fresh();
			handle_stuff();

			Term_xtra(TERM_XTRA_DELAY, msec);
			light_spot(y, x);

			Term_fresh();
			handle_stuff();
		}

		/* Delay anyway for consistency */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Handle monster */
		if ((cave_m_idx[y][x] > 0) && !(mon_list[cave_m_idx[y][x]].mflag & (MFLAG_HIDE)))
		{
			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_lore *l_ptr = &l_list[m_ptr->r_idx];

			int chance2;

			int visible = m_ptr->ml;


			/*Adjust for player terrain*/
			chance = feat_adjust_combat_for_player(chance, FALSE);

			/*Adjust for monster terrain*/
			chance = feat_adjust_combat_for_monster(m_ptr, chance, TRUE);

			chance2 = chance - distance(p_ptr->py, p_ptr->px, y, x);

			/* Note the collision */
			hit_body = TRUE;

			/* Some monsters are great at dodging  -EZ- */
			if ((r_ptr->flags2 & (RF2_EVASIVE)) && (!m_ptr->m_timed[MON_TMD_SLEEP]) &&
				(!m_ptr->m_timed[MON_TMD_STUN]) && (!m_ptr->m_timed[MON_TMD_CONF]) &&
				(!m_ptr->m_timed[MON_TMD_FEAR]) && (rand_int(5 + m_ptr->cdis) >= 3))
			{
				if (visible)
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					message_format(MSG_MISS, 0, "%^s dodges!", m_name);

					/* Learn that monster can dodge */
					l_ptr->r_l_flags2 |= (RF2_EVASIVE);
				}

				continue;
			}

			/* Did we hit it (penalize distance travelled) */
			else if (test_hit(chance2, r_ptr->ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/*Mark the monster as attacked by the player*/
				m_ptr->mflag |= (MFLAG_HIT_BY_RANGED);

				if (monster_nonliving(r_ptr))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}

				/* Make some noise */
				add_wakeup_chance += p_ptr->base_wakeup_chance / 2;

				/* Reveal fully visible mimics */
				if ((m_ptr->mimic_k_idx) && (m_ptr->ml))
				{
					/* Reveal it */
					reveal_mimic(m_ptr->fy, m_ptr->fx, TRUE);

					/*We can not see it*/
					visible = TRUE;

				}

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					message_format(MSG_SHOOT_HIT, 0, "The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Message */
					message_format(MSG_SHOOT_HIT, 0, "The %s hits %s.", o_name, m_name);

					/* Hack -- Track this monster race */
					if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (m_ptr->ml) health_track(cave_m_idx[y][x]);
				}

				/* Apply special damage XXX XXX XXX */
				tdam = tot_dam_aux(i_ptr, tdam, m_ptr, FALSE);
				tdam = critical_shot(i_ptr->weight, i_ptr->to_h, tdam, FALSE);

				/*rogues are deadly with slings*/
				if ((cp_ptr->flags & CF_ROGUE_COMBAT) && (p_ptr->state.ammo_tval == TV_SHOT))
				{

					tdam += p_ptr->lev * 2 / 3;
				}

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (p_ptr->wizard)
				{
					msg_format("You do %d (out of %d) damage.",
					           tdam, m_ptr->hp);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(cave_m_idx[y][x], tdam, &fear, note_dies, SOURCE_PLAYER))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					/* Message */
					message_pain(cave_m_idx[y][x], tdam);

				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(i_ptr) : 0);

	/* Drop (or break) near that location */
	drop_near(i_ptr, j, y, x);

}



void textui_cmd_fire(void)
{
	object_type *j_ptr;
	int item;
	int dir;
	cptr q = "Fire which item? ";
	cptr s = "You have nothing to fire.";

	/* Get the "bow" (if any) */
	j_ptr = &inventory[INVEN_BOW];

	/* Require a usable launcher */
	if (!j_ptr->tval || !p_ptr->state.ammo_tval)
	{
		msg_print("You have nothing to fire with.");
		return;
	}

	/* Require proper missile; prefer the quiver */
	item_tester_tval = p_ptr->state.ammo_tval;
	p_ptr->command_wrk = USE_EQUIP;

	/* Get an item */
	if (!get_item(&item, q, s, (USE_INVEN | USE_QUIVER | QUIVER_FIRST | USE_FLOOR))) return;

	/* Get a direction (or cancel) */
	if (!get_aim_dir(&dir, FALSE)) return;

	cmd_insert(CMD_FIRE, item, dir);
}

void textui_cmd_fire_at_nearest(void)
{

	/* the direction '5' means 'use the target' */
	int i, dir = 5, item = -1;

	/* Require a usable launcher */
	if (!inventory[INVEN_BOW].tval || !p_ptr->state.ammo_tval)
	{
		msg_print("You have nothing to fire with.");
		return;
	}

	/* Find first eligible ammo in the quiver */
	for (i=QUIVER_START; i < QUIVER_END; i++)
	{
		object_type *o_ptr = & inventory [i];

		if (!ammo_can_fire(o_ptr, i)) continue;

		item = i;
		break;
	}

	/* Next, try the backpack if necessary*/
	if (item < 0)
	{
		for (i = 0; i < INVEN_PACK; i++)
		{
			object_type *o_ptr = & inventory [i];

			if (!ammo_can_fire(o_ptr, i)) continue;

			item = i;
			break;
		}
	}

	/* Require usable ammo */
	if (item < 0)
	{
		msg_print("You have no ammunition in the quiver to fire");
		return;
	}

	/* Require foe */
	if (!target_set_closest(TARGET_KILL | TARGET_QUIET))
		return;

	/* Check for confusion */
	if (p_ptr->timed[TMD_CONFUSED])
	{
		msg_print("You are confused.");
		dir = ddd[randint0(8)];
	}

	/* Fire! */
	cmd_insert(CMD_FIRE, item, dir);
}



/*
 * Flavor. Some objects cause random effects when thrown.
 * Returns TRUE if the object affected the dungeon in some way.
 */
static bool do_flavor_breakage(const object_type *o_ptr, int y, int x)
{
	/* Get the feature */
	feature_type *f_ptr = &f_info[cave_feat[y][x]];

	/* Analyze object */
	switch (o_ptr->tval)
	{
		/* Flasks of oil can create oil patches */
		case TV_FLASK:
		{
			/* Hack -- Check if oil exists */
			if (!f_info[FEAT_OIL].name) break;

			/* Hack -- Graphics (don't hurt anything) */
			project(SOURCE_OTHER, 1, y, x, y, x, 0, GF_SMOKE, PROJECT_BOOM, 0, 0);

			/* Message. Flasks of oil are always broken now */
			msg_print("The flask is broken!");

			/* Check if the grid can be transformed to oil */
			if (!_feat_ff3_match(f_ptr, TERRAIN_MASK) && cave_passable_bold(y, x) &&
					!_feat_ff1_match(f_ptr, FF1_PERMANENT | FF1_STAIRS | FF1_DOOR) &&
					(_feat_ff1_match(f_ptr, FF1_LOS | FF1_PROJECT) ==
					feat_ff1_match(FEAT_OIL, FF1_LOS | FF1_PROJECT)) &&
					one_in_(4))
			{
				/* Create oil */
				cave_set_feat(y, x, FEAT_OIL);

				/* Message */
				msg_print("There is oil on the floor.");
			}

			/* Success */
			return (TRUE);
		}
		/* Torches can ignite oil and forests */
		case TV_LIGHT:
		{
			/* Check if the lite is a torch and if the grid can be burned */
			if ((o_ptr->sval == SV_LIGHT_TORCH) && _feat_ff3_match(f_ptr, FF3_OIL | FF3_FOREST) &&
				_feat_ff2_match(f_ptr, FF2_HURT_FIRE))
			{
				/* Create fire */
				cave_alter_feat(y, x, FS_HURT_FIRE);

				/* Message */
				msg_print("The torch sets the dungeon on fire!");

				/* Get the fire feature */
				f_ptr = &f_info[cave_feat[y][x]];

				/* Hurt objects sensitive to fire on the floor */
				if (_feat_ff3_match(f_ptr, FF3_FIRE) && (f_ptr->dam_non_native > 0))
				{
					/* Hurt only items */
					u32b flags = PROJECT_ITEM | PROJECT_HIDE;

					/* Burn objects */
					project(SOURCE_OTHER, 0, y, x, y, x, f_ptr->dam_non_native, GF_FIRE, flags, 0, 0);
				}

				/* Success */
				return (TRUE);
			}

			break;
		}
	}

	/* Failure */
	return (FALSE);
}




/*handle special effects of throwing certain potions*/
static bool thrown_potion_effects(object_type *o_ptr, bool *is_dead, bool *fear, int m_idx)
{
	monster_type *m_ptr = &mon_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	/*Assume it hurts everything*/
	u32b flag = (PROJECT_KILL | PROJECT_PLAY);

	int y = m_ptr->fy;
	int x = m_ptr->fx;

	bool ident = FALSE;

	bool do_stun = FALSE;
	bool un_confuse = FALSE;
	bool un_stun = FALSE;
	bool un_fear = FALSE;

	bool used_potion = TRUE;

	/* Hold the monster name */
	char m_name[80];
	char m_poss[80];

	/* Get the monster name*/
	monster_desc(m_name, sizeof(m_name), m_ptr, 0);

	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, sizeof(m_name), m_ptr, 0x22);

	/* Analyze the potion */
	switch (o_ptr->sval)
	{

		case SV_POTION_SLOWNESS:
		{
			/*slowness explosion at the site, radius 1*/
			ident = explosion(SOURCE_PLAYER, 1, y, x, damroll (2, p_ptr->lev), GF_OLD_SLOW, flag);
			break;
		}

		case SV_POTION_CONFUSION:
		{
			/*confusion explosion at the site, radius 1*/
			ident = explosion(SOURCE_PLAYER, 1, y, x, damroll (2, p_ptr->lev), GF_OLD_CONF, flag);
			break;

		}

		case SV_POTION_SLEEP:
		{
			/*sleep explosion at the site, radius 1*/
			ident = explosion(SOURCE_PLAYER, 1, y, x, damroll(2, p_ptr->lev), GF_OLD_SLEEP, flag);
			break;
		}

		case SV_POTION_LOSE_MEMORIES:
		{

			if (m_ptr->smart)
			{

				/*erase monster memory of player*/
				m_ptr->smart = 0L;

				if (m_ptr->ml)
				{
					ident = TRUE;

					/*monster forgets player history*/
					msg_format("%^s forgets all %s knows about you!", m_name, m_poss);
				}
			}
			/*monster forgets player history*/
			else used_potion = FALSE;

			break;
		}

		case SV_POTION_DRAIN_MANA:
		{
			if (m_ptr->mana)
			{
				if (m_ptr->ml)
				{
					ident = TRUE;

					/*monster forgets player history*/
					msg_format("%^s loses some of %s mana!", m_name, m_poss);
				}

				/*reduce mana by about 11%*/
				m_ptr->mana = m_ptr->mana * 9 / 10;

			}

			/*monster forgets player history*/
			else used_potion = FALSE;

			break;
		}

		case SV_POTION_RUINATION:
		{
			ident = TRUE;

			/*slight damage to monster*/
			mon_take_hit(cave_m_idx[y][x], damroll(10, 10), fear, NULL, SOURCE_PLAYER);

			break;
		}

		case SV_POTION_DETONATIONS:
		{

			ident = TRUE;

			/*slight damage to monster*/
			mon_take_hit(cave_m_idx[y][x], damroll(25, 25), fear, NULL, SOURCE_PLAYER);

			/*set the stun counter*/
			do_stun = TRUE;

			break;
		}

		case SV_POTION_DEATH:
		{
			/*drain life explosion at the site, radius 1*/
			ident = explosion(SOURCE_PLAYER, 1, y, x, damroll(30, 30), GF_LIFE_DRAIN, flag);
			break;
		}

		case SV_POTION_DETECT_INVIS:
		{
			if ((!m_ptr->ml)&& (r_ptr->flags2 & (RF2_INVISIBLE)))
			{
				/* Mark as visible */
				m_ptr->ml = TRUE;

				/*re-draw the spot*/
				light_spot(y, x);

				/* Update the monster name*/
				monster_desc(m_name, sizeof(m_name), m_ptr, 0);

				/*monster forgets player history*/
				msg_format("%^s appears for an instant!", m_name);

				/*update the lore*/
				l_ptr->r_l_flags2 |= (RF2_INVISIBLE);

				ident = TRUE;
			}

			/* Potion isn't idntified */
			else used_potion = FALSE;

			break;
		}

		case SV_POTION_BOLDNESS:
		{

			un_fear = TRUE;

			break;
		}

		case SV_POTION_SPEED:
		{

			/*speed explosion at the site, radius 1*/
			ident = explosion(SOURCE_PLAYER, 1, y, x, 20 + rand_int(20), GF_OLD_SPEED, flag);
			break;
		}

		case SV_POTION_HEROISM:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, 10, GF_OLD_HEAL, flag)) ident = TRUE;

			un_fear = TRUE;

			break;
		}

		case SV_POTION_BERSERK_STRENGTH:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, 10, GF_OLD_HEAL, flag)) ident = TRUE;

			un_fear = TRUE;

			break;
		}

		case SV_POTION_CURE_LIGHT:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, damroll(3, 8), GF_OLD_HEAL, flag)) ident = TRUE;

			break;
		}

		case SV_POTION_CURE_SERIOUS:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, damroll(5, 10), GF_OLD_HEAL, flag)) ident = TRUE;

			un_confuse = TRUE;

			break;
		}

		case SV_POTION_CURE_CRITICAL:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, damroll(8, 10), GF_OLD_HEAL, flag)) ident = TRUE;

			un_confuse = TRUE;
			un_stun = TRUE;

			break;
		}

		case SV_POTION_HEALING:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, 325, GF_OLD_HEAL, flag)) ident = TRUE;

			un_confuse = TRUE;
			un_stun = TRUE;

			break;
		}

		case SV_POTION_STAR_HEALING:
		{
			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, 1500, GF_OLD_HEAL, flag)) ident = TRUE;

			un_confuse = TRUE;
			un_stun = TRUE;

			break;
		}

		case SV_POTION_LIFE:
		{
			/*only for the living*/
			if monster_nonliving(r_ptr)
			{
				used_potion = FALSE;

				break;
			}

			/*healing explosion at the site, radius 1*/
			if (explosion(SOURCE_PLAYER, 1, y, x, 5000, GF_OLD_HEAL, flag)) ident = TRUE;

			un_confuse = TRUE;
			un_stun = TRUE;

			break;
		}

		case SV_POTION_RESTORE_MANA:
		{

			if (r_ptr->mana > m_ptr->mana)
			{
				if (m_ptr->ml)
				{
					ident = TRUE;

					/*monster forgets player history*/
					msg_format("%^s gains back all %s mana!", m_name, m_poss);
				}

				/*restore mana%*/
				m_ptr->mana = r_ptr->mana;
			}

			/*monster forgets player history*/
			else used_potion = FALSE;

			break;
		}

		/*potion just gets thrown as normal object*/
		default:
		{
			used_potion = FALSE;

			break;
		}
	}

	/*monster is now dead, skip messages below*/
	if (cave_m_idx[y][x] == 0)
	{
		do_stun = FALSE;
		un_confuse = FALSE;
		un_stun = FALSE;
		un_fear = FALSE;
		*is_dead = TRUE;
	}

	if (un_confuse)
	{

		mon_clear_timed(m_idx, MON_TMD_CONF, MON_TMD_FLG_NOTIFY);

		/* Dump a message */
		if (m_ptr->ml) ident = TRUE;

	}

	if (un_stun)
	{
		mon_clear_timed(m_idx, MON_TMD_STUN, MON_TMD_FLG_NOTIFY);

		/* Dump a message */
		if (m_ptr->ml) ident = TRUE;
	}

	if (un_fear)
	{
		mon_clear_timed(m_idx, MON_TMD_FEAR, MON_TMD_FLG_NOTIFY);

		/* Dump a message */
		if (m_ptr->ml) ident = TRUE;
	}

	/* Sound and Impact breathers never stun */
	if (do_stun)
	{
		mon_inc_timed(m_idx, MON_TMD_STUN, 15, MON_TMD_FLG_NOTIFY);

		ident = TRUE;

	}

	/*inform them of the potion, mark it as known*/
	if ((ident) && (!(k_info[o_ptr->k_idx].aware)))
	{

		char o_name[80];

		/* Identify it fully */
		object_aware(o_ptr);
		object_known(o_ptr);

		/* Description */
		object_desc(o_name, sizeof(o_name), o_ptr, ODESC_FULL | ODESC_SINGULAR);

		/* Describe the potion */
		msg_format("You threw %s.", o_name);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->redraw |= (PR_INVEN | PR_EQUIP);

	}

	/* Redraw if necessary*/
	if (used_potion) p_ptr->redraw |= (PR_HEALTH | PR_MON_MANA);

	/* Handle stuff */
	handle_stuff();

	return (used_potion);

}



/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
void do_cmd_throw(cmd_code code, cmd_arg args[])
{
	int dir, item;
	int i, j, y, x, ty, tx;
	int chance, tdam, tdis;
	int mul, divider;
	u32b f1, f2, f3, fn;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	bool hit_body = FALSE;

	int path_n;
	u16b path_g[PATH_SIZE];
	u16b path_gx[PATH_SIZE];

	byte missile_attr;
	char missile_char;

	char o_name[80];

	int msec = op_ptr->delay_factor * op_ptr->delay_factor;

	/* Get item to throw and direction in which to throw it. */
	item = args[0].item;
	dir = args[1].direction;

	/* Make sure the player isn't throwing wielded items */
	if (item >= INVEN_WIELD && item < QUIVER_START)
	{
		msg_print("You have cannot throw wielded items.");
		return;
	}

	/* Get the object */
	o_ptr = object_from_item_idx(item);

	/* A cursed quiver disables the use of non-cursed ammo */
	if (IS_QUIVER_SLOT(item) && p_ptr->cursed_quiver && !cursed_p(o_ptr))
	{
		msg_print("Your quiver is cursed!");
		return;
	}

	/* Check the item being thrown is usable by the player. */
	if (!item_is_available(item, NULL, (USE_EQUIP | USE_INVEN | USE_FLOOR | USE_QUIVER)))
	{
		msg_format("That item is not within your reach.");
		return;
	}

	/* Examine the item */
	object_flags(o_ptr, &f1, &f2, &f3, &fn);

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain a local object */
	object_copy(i_ptr, o_ptr);

	/* Distribute the charges of rods/wands between the stacks */
	distribute_charges(o_ptr, i_ptr, 1);

	/* Single object */
	i_ptr->number = 1;
	i_ptr->obj_in_use = FALSE;

	if (IS_QUIVER_SLOT(item))
	{
		/*Mark it to go in the quiver */
		i_ptr->ident |= (IDENT_QUIVER);
	}

	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Reduce and describe floor item */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}


	/* Description */
	object_desc(o_name, sizeof(o_name), i_ptr, ODESC_FULL);

	/* Cursed throwing weapons can hurt the player sometimes */
	if (IS_QUIVER_SLOT(item) && cursed_p(i_ptr) && (rand_int(100) < 70))
	{
		/* Get amount of damage */
		int dam = damroll(i_ptr->dd, i_ptr->ds) + ABS(i_ptr->to_d);

		/* Message */
		msg_format("The %s releases its curse on you!", o_name);

		/* Hurt the player */
		project_p(SOURCE_OTHER, p_ptr->py, p_ptr->px, dam, GF_NETHER, "throwing a cursed weapon");
	}


	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(i_ptr);
	missile_char = object_char(i_ptr);


	/* Extract a "distance multiplier" */
	mul = 10;

	/* Enforce a minimum "weight" of one pound */
	divider = ((i_ptr->weight > 10) ? i_ptr->weight : 10);

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->state.stat_ind[A_STR]] + 20) * mul / divider;

	/* Max distance of 10 */
	if (tdis > 10) tdis = 10;

	/* Hack -- Base damage from thrown object */
	tdam = damroll(i_ptr->dd, i_ptr->ds) + i_ptr->to_d;

	/* Chance of hitting */
	if (f3 & (TR3_THROWING))
	{
		chance = p_ptr->state.skills[SKILL_TO_HIT_THROW] + BTH_PLUS_ADJ * (p_ptr->state.to_h + i_ptr->to_h);
	}
	else
	{
		chance = (3 * p_ptr->state.skills[SKILL_TO_HIT_THROW] / 2) + (BTH_PLUS_ADJ * i_ptr->to_h);
	}

	/* Take a turn */
	p_ptr->p_energy_use = BASE_ENERGY_MOVE;


	/* Start at the player */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Predict the "target" location */
	ty = p_ptr->py + 99 * ddy[dir];
	tx = p_ptr->px + 99 * ddx[dir];

	/* Check for "target request" */
	if ((dir == 5) && target_okay())
	{
		tx = p_ptr->target_col;
		ty = p_ptr->target_row;
	}

	/* Calculate the path */
	path_n = project_path(path_g, path_gx, tdis, p_ptr->py, p_ptr->px, &ty, &tx, 0);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		/* Hack -- Stop before hitting walls */
		if (!cave_project_bold(ny, nx) && !cave_passable_bold(ny, nx)) break;

		/* Advance */
		x = nx;
		y = ny;

		/* Only do visuals if the player can "see" the missile */
		if (player_can_see_bold(y, x))
		{
			/* Visual effects */
			print_rel(missile_char, missile_attr, y, x);
			move_cursor_relative(y, x);
			Term_fresh();
			handle_stuff();

			Term_xtra(TERM_XTRA_DELAY, msec);
			light_spot(y, x);

			Term_fresh();
			handle_stuff();
		}

		/* Delay anyway for consistency */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Handle monster */
		if ((cave_m_idx[y][x] > 0) && !(mon_list[cave_m_idx[y][x]].mflag & (MFLAG_HIDE)))
		{
			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_lore *l_ptr = &l_list[m_ptr->r_idx];

			int chance2;

			int visible = m_ptr->ml;

			int sleeping_bonus = 0;

			bool potion_effect = FALSE;
			int pdam = 0;
			bool is_dead = FALSE;


			/*Adjust for player terrain*/
			chance = feat_adjust_combat_for_player(chance, FALSE);

			/*Adjust for monster terrain*/
			chance = feat_adjust_combat_for_monster(m_ptr, chance, TRUE);

			chance2 = chance - distance(p_ptr->py, p_ptr->px, y, x);

			/* Note the collision */
			hit_body = TRUE;

			/* Rogues Get extra to-hit from throwing weapons*/

			if ((cp_ptr->flags & CF_ROGUE_COMBAT)
				&& (m_ptr->ml) && (f3 & (TR3_THROWING)))
			{
				sleeping_bonus = 30 + p_ptr->lev / 2;
			}

			/* Some monsters are great at dodging  -EZ- */
			if ((r_ptr->flags2 & (RF2_EVASIVE)) && (!m_ptr->m_timed[MON_TMD_SLEEP]) &&
					(!m_ptr->m_timed[MON_TMD_STUN]) && (!m_ptr->m_timed[MON_TMD_CONF]) &&
					(!m_ptr->m_timed[MON_TMD_FEAR]) && (rand_int(5 + m_ptr->cdis) >= 3))
			{
				if (visible)
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					message_format(MSG_MISS, 0, "%^s dodges!", m_name);

					/* Learn that monster can dodge */
					l_ptr->r_l_flags2 |= (RF2_EVASIVE);
				}

				continue;
			}

			/* Did we hit it (penalize range) */
			else if (test_hit((chance2 + sleeping_bonus), r_ptr->ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/*Mark the monster as attacked by the player*/
				m_ptr->mflag |= (MFLAG_HIT_BY_RANGED);

				/* Some monsters get "destroyed" */
				if (monster_nonliving(r_ptr))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}

				/* Make some noise */
				add_wakeup_chance += p_ptr->base_wakeup_chance / 2;

				/* Reveal fully visible mimics */
				if ((m_ptr->mimic_k_idx) && (m_ptr->ml))
				{
					/* Reveal it */
					reveal_mimic(m_ptr->fy, m_ptr->fx, TRUE);

					/*We can not see it*/
					visible = TRUE;

				}

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					msg_format("The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					if (f3 & (TR3_THROWING))
					{
						/* Message */
						msg_format("The %s hits %s with great accuracy!.", o_name, m_name);
					}
					else
					{
					/* Message */
						msg_format("The %s hits %s.", o_name, m_name);
					}
					/* Hack -- Track this monster race */
					if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (m_ptr->ml) health_track(cave_m_idx[y][x]);

				}

				/*special effects sometimes reveal the kind of potion*/
				if (i_ptr->tval == TV_POTION)
				{
					/*record monster hit points*/
					pdam = m_ptr->hp;

					/*returns true if the damage has already been handled*/
					potion_effect = (thrown_potion_effects(i_ptr, &is_dead, &fear, cave_m_idx[y][x]));

					/*check the change in monster hp*/
					pdam -= m_ptr->hp;

					/*monster could have been healed*/
					if (pdam < 0) pdam = 0;

				}

				/* Apply special damage XXX XXX XXX */
				if (!potion_effect) tdam = tot_dam_aux(i_ptr, tdam, m_ptr, FALSE);

				/* Object is a throwing weapon. */
				if (f3 & (TR3_THROWING))
				{
					/* Perfectly balanced weapons do even more damage. */
					if (i_ptr->ident & IDENT_PERFECT_BALANCE) tdam *= 2;

					/* Critical hits may add damage dice. */
					tdam = critical_shot(i_ptr->weight, i_ptr->to_h, tdam, TRUE);

					/*
					 * Double the damage for throwing weapons
					 */
					tdam *= 2;
				}

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (p_ptr->wizard)
				{

					msg_format("You do %d (out of %d) damage.",
					           (potion_effect ? pdam : tdam), m_ptr->hp);
				}

				/* Hit the monster, unless a potion effect has already been done */
				if (!potion_effect)
				{
					 is_dead = (mon_take_hit(cave_m_idx[y][x], tdam, &fear, note_dies, SOURCE_PLAYER));
				}

				/* Still alive */
				if (!is_dead)
				{
					/* Message if applicable*/
					if ((!potion_effect) || (pdam > 0))
						message_pain(cave_m_idx[y][x],  (pdam ? pdam : tdam));
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Check for special breakage */
	if (!hit_body && do_flavor_breakage(i_ptr, y, x)) return;

	/* Chance of breakage (during attacks) - potions always break*/
	if (i_ptr->ident & IDENT_PERFECT_BALANCE) j = 0;
	else j = (hit_body ? breakage_chance(i_ptr) : 0);

	/*hack - throwing weapons have a lesser chance*/
	if (f3 & (TR3_THROWING)) j /= 2;

	/* Drop (or break) near that location */
	drop_near(i_ptr, j, y, x);

}



void textui_cmd_throw(void)
{
	int item, dir;
	cptr q, s;

	/* Get an item */
	q = "Throw which item? ";
	s = "You have nothing to throw.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_QUIVER | USE_FLOOR))) return;

	if (item >= INVEN_WIELD && item < QUIVER_START)
	{
		msg_print("You have cannot throw wielded items.");
		return;
	}

	/* Get a direction (or cancel) */
	if (!get_aim_dir(&dir, FALSE)) return;

	cmd_insert(CMD_THROW, item, dir);
}


