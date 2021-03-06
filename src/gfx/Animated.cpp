//  Copyright (c) 2018 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.


#include <gfx/Generated/Types.h>
#include <gfx/Animated.h>

#include <obj/Vector.h>
#include <obj/Reflect/Class.h>
#include <math/Interp.h>
#include <gfx/Node3.h>
#include <gfx/Item.h>
#include <gfx/Mesh.h>
#include <gfx/Skeleton.h>

#include <math/Math.h>
#include <bx/math.h>

namespace mud
{
	Animated::Animated(Node3& node)
		: m_node(node)
	{}

	Animated::~Animated()
	{}

	void Animated::add_item(Item& item)
	{
		m_rig = *item.m_model->m_rig;
		item.m_rig = &m_rig;
	}

	void Animated::play(cstring name, bool loop, float blend, float speed, bool reverse)
	{
		for(Animation& animation : m_rig.m_animations)
			if(animation.m_name == name)
			{
				this->play(animation, loop, blend, speed, reverse);
				return;
			}
	}

	void Animated::play(const Animation& animation, bool loop, float blend, float speed, bool reverse)
	{
		for(AnimationPlay& playing : m_playing)
		{
			if(blend == 0.f)
				blend = m_default_blend_time;
			if(blend > 0.f)
				playing.m_fadeout = blend;
		}

		m_playing.push_back(AnimationPlay{ animation, loop, speed, reverse, &m_rig.m_skeleton });
		m_active = true;
	}

	void Animated::stop()
	{
		m_playing.clear();
	}

	void Animated::pause()
	{
		m_active = false;
	}

	void Animated::next_animation()
	{
		//this->play(*vector_pop(m_queue));
	}

	void Animated::advance(float delta)
	{
		if(m_playing.size() > 2)
			printf("WARNING: Animated playing more than 2 animations at the same time\n");

		for(AnimationPlay& play : m_playing)
			play.step(delta, m_speed_scale);

		vector_remove_if(m_playing, [](AnimationPlay& play) { return play.m_ended; });

		for(Bone& bone : m_rig.m_skeleton.m_bones)
			bone.m_pose_local = bxTRS(bone.m_scale, bone.m_rotation, bone.m_position);

		m_rig.update_rig();
	}

	void Animated::seek(float time)
	{
		for(AnimationPlay& play : m_playing)
		{
			play.m_cursor = time;
			play.update(time, 0.f, 1.f);
		}
	}

	AnimationPlay::AnimationPlay(const Animation& animation, bool loop, float speed, bool reverse, Skeleton* skeleton)
		: m_animation(&animation)
		, m_loop(loop)
		, m_speed(speed)
		, m_reverse(reverse)
	{
		m_tracks.reserve(animation.tracks.size());

		for(const AnimationTrack& track : animation.tracks)
		{
			Ref target = {};
			if(skeleton && skeleton->m_bones.size() > track.m_node)
				target = &skeleton->m_bones[track.m_node];
			if(!target)
			{
				//printf("WARNING: No bone found for animation %s track %s with target %s\n", animation.m_name.c_str(), "", track.m_node_name.c_str());
				continue;
			}

			AnimatedTrack playtrack = { &track, target, {}, track.m_keys[0].m_value };

			m_tracks.push_back(playtrack);
		}
	}

	void AnimationPlay::step(float timestep, float speed)
	{
		float delta = timestep * m_speed * speed;
		float next_pos = m_cursor + delta;
		float blend = 1.f;

		bool looped = false;
		if(m_loop && next_pos >= m_animation->m_length)
		{
			next_pos = fmod(next_pos, m_animation->m_length);
			looped = true;
		}
		else if(m_loop && next_pos < 0.f)
		{
			next_pos = m_animation->m_length - fmod(-next_pos, m_animation->m_length);
			looped = true;
		}
		else if(!m_loop && next_pos >= m_animation->m_length)
		{
			next_pos = m_animation->m_length;
			m_ended = true;
		}

		delta = next_pos - m_cursor;
		m_cursor = next_pos;

		if(m_fadeout)
		{
			blend = m_fadeout_left / m_fadeout;
			m_fadeout_left -= delta;

			if(m_fadeout_left <= 0.f)
				m_ended = true;
		}

		for(AnimatedTrack& track : m_tracks)
		{
			track.m_cursor.m_time = m_cursor;

			if(looped)
			{
				track.m_cursor.m_prev = m_speed > 0.f ? 0 : track.m_track->m_keys.size() - 2;
				track.m_cursor.m_next = m_speed > 0.f ? 1 : track.m_track->m_keys.size() - 1;
			}

			while(m_speed > 0.f && track.m_cursor.m_time >= track.m_track->m_keys[track.m_cursor.m_next].m_time)
			{
				track.m_cursor.m_next++;
				track.m_cursor.m_prev++;
			}
			while(m_speed < 0.f && track.m_cursor.m_time <= track.m_track->m_keys[track.m_cursor.m_prev].m_time)
			{
				track.m_cursor.m_next--;
				track.m_cursor.m_prev--;
			}
		}

		this->update(m_cursor, delta, blend);
	}

	void AnimationPlay::update(float time, float delta, float interp)
	{
		UNUSED(time); UNUSED(interp);
		for(AnimatedTrack& track : m_tracks)
		{
			if(track.m_track->m_interpolation > Interpolation::Nearest)
			{
				track.m_track->sample(track.m_cursor, track.m_value);

				// @todo : add blending of multiple animations
				/*
				if(track.m_value.none())
					track.m_value = value;
				else
					track.m_value = interpolate(track.m_value, value, interp);
				*/

				track.m_track->m_member->set(track.m_target, track.m_value);
			}
			else
			{
				track.m_track->value(track.m_cursor, track.m_value, delta > 0.f);
				track.m_track->m_member->set(track.m_target, track.m_value);
			}
		}
	}
}

