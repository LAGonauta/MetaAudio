#include "Utilities/ChannelManager.hpp"
#include "Vox/VoxManager.hpp"
#include "SoundSources/BaseSoundSource.hpp"

namespace MetaAudio
{
	ChannelManager::ChannelManager()
	{
	}

	bool ChannelManager::IsPlaying(sfx_t* sfx)
	{
		auto functor = [&](aud_channel_t& channel) { return channel.sfx == sfx && channel.sound_source->IsPlaying(); };

		return std::any_of(channels.dynamic.begin(), channels.dynamic.end(), functor) ||
			std::any_of(channels.static_.begin(), channels.static_.end(), functor);
	}

	void ChannelManager::FreeChannel(aud_channel_t* ch)
	{
		auto it = std::find_if(channels.static_.begin(), channels.static_.end(), [&](aud_channel_t& channel) { return &channel == ch; });
		if (it != channels.static_.end())
		{
			channels.static_.erase(it);
		}
		else
		{
			it = std::find_if(channels.dynamic.begin(), channels.dynamic.end(), [&](aud_channel_t& channel) { return &channel == ch; });
			if (it != channels.dynamic.end())
			{
				channels.dynamic.erase(it);
			}
		}
	}

	aud_channel_t* ChannelManager::SND_PickStaticChannel(int entnum, int entchannel, sfx_t* sfx)
	{
		// We do not want an entity playing the same SFX more than once.
		auto channel = std::find_if(
			channels.static_.begin(),
			channels.static_.end(),
			[&](aud_channel_t& channel)
			{
				return channel.sfx == nullptr ||
					(channel.sfx == sfx && channel.entnum == entnum && (channel.entchannel == entchannel || entchannel == -1)); // actually should compare origin to be sure
			}
		);
		if (channel != channels.static_.end())
		{
			channels.static_.erase(channel);
		}

		return &channels.static_.emplace_back();
	}

	aud_channel_t* ChannelManager::SND_PickDynamicChannel(int entnum, int entchannel, sfx_t* sfx)
	{
		if (entchannel == CHAN_STREAM && IsPlaying(sfx))
		{
			return nullptr;
		}

		// Remove channel if entity is already using for vox. We do not want the entity talking about two things at the same time.
		auto channel = std::find_if(
			channels.dynamic.begin(),
			channels.dynamic.end(),
			[&](aud_channel_t& channel)
			{
				if (channel.sfx && channel.entchannel == CHAN_STREAM)
				{
					return false;
				}

				if (channel.sfx && channel.entnum == *gAudEngine.cl_viewentity && entnum != *gAudEngine.cl_viewentity)
				{
					return false;
				}

				if (entchannel != CHAN_AUTO && channel.entnum == entnum && (channel.entchannel == entchannel || entchannel == -1))
				{
					return true;
				}

				return false;
			});

		if (channel != channels.dynamic.end())
		{
			if (channel->sfx)
			{
				auto sc = reinterpret_cast<aud_sfxcache_t*>(channel->sfx->cache.data);
				if (sc && sc->looping)
				{
					if (channel->sfx == sfx && channel->entnum == entnum && channel->entchannel == entchannel)
					{
						return nullptr;
					}
				}
			}

			channels.dynamic.erase(channel);
		}

		return &channels.dynamic.emplace_back();
	}

	void ChannelManager::ClearAllChannels()
	{
		channels.dynamic.clear();
		channels.static_.clear();
	}

	void ChannelManager::ClearEntityChannels(int entnum, int entchannel)
	{
		auto functor = [&](auto& channel) { return channel.entnum == entnum && channel.entchannel == entchannel; };

		channels.dynamic.erase(std::remove_if(channels.dynamic.begin(), channels.dynamic.end(), functor), channels.dynamic.end());
		channels.static_.erase(std::remove_if(channels.static_.begin(), channels.static_.end(), functor), channels.static_.end());
	}

	void ChannelManager::ClearFinished()
	{
		auto functor = [&](aud_channel_t& channel) { return channel.words.size() == 0 && !channel.sound_source->IsPlaying(); };

		channels.dynamic.erase(std::remove_if(channels.dynamic.begin(), channels.dynamic.end(), functor), channels.dynamic.end());
		channels.static_.erase(std::remove_if(channels.static_.begin(), channels.static_.end(), functor), channels.static_.end());
	}

	void ChannelManager::ClearLoopingRemovedEntities()
	{
		auto functor = [&](aud_channel_t& channel) {
			if (channel.entnum == 0 || !channel.sound_source->IsLooping())
			{
				return false;
			}

			auto entity = gEngfuncs.GetEntityByIndex(channel.entnum);
			return entity == nullptr;
			};

		channels.dynamic.erase(std::remove_if(channels.dynamic.begin(), channels.dynamic.end(), functor), channels.dynamic.end());
		channels.static_.erase(std::remove_if(channels.static_.begin(), channels.static_.end(), functor), channels.static_.end());
	}

	int ChannelManager::S_AlterChannel(int entnum, int entchannel, sfx_t* sfx, float fvol, float pitch, int flags)
	{
		std::function<bool(aud_channel_t& channel)> functor;

		auto internalFunctor = [&](aud_channel_t& channel)
			{
				if (flags & SND_CHANGE_PITCH)
				{
					channel.pitch = pitch;
					channel.sound_source->SetPitch(channel.pitch);
				}

				if (flags & SND_CHANGE_VOL)
				{
					channel.volume = fvol;
					channel.sound_source->SetGain(channel.volume);
				}

				if (flags & SND_STOP)
				{
					channel.sound_source->Stop();
				}
			};

		if (sfx->name[0] == '!')
		{
			// This is a sentence name.
			// For sentences: assume that the entity is only playing one sentence
			// at a time, so we can just shut off
			// any channel that has ch->isentence >= 0 and matches the
			// soundsource.
			functor = [&](aud_channel_t& channel)
				{
					if (channel.entnum == entnum &&
						channel.entchannel == entchannel &&
						channel.sfx != nullptr &&
						channel.words.size() > 0)
					{
						internalFunctor(channel);
						return true;
					}
					else
					{
						return false;
					}
				};
		}
		else
		{
			functor = [&](aud_channel_t& channel)
				{
					if (channel.entnum == entnum &&
						channel.entchannel == entchannel &&
						channel.sfx == sfx)
					{
						internalFunctor(channel);
						return true;
					}
					else
					{
						return false;
					}
				};
		}

		return std::any_of(channels.dynamic.begin(), channels.dynamic.end(), functor) ||
			std::any_of(channels.static_.begin(), channels.static_.end(), functor);
	}
}