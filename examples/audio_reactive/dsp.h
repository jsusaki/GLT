#pragma once
#include "Core/Common.h"

#include <algorithm>

#undef min
#undef max
#undef near
#undef far

// Parameters
constexpr s32 FFT_SIZE     = 4096;
constexpr s32 BINS         = FFT_SIZE / 2; // Nyquist
constexpr s32 SAMPLE_RATE  = 44100;
constexpr f32 FREQ_PER_BIN = SAMPLE_RATE / (f32)FFT_SIZE;

// Band Thresholds
constexpr s32 SUB_BASS_END = (s32)(60   / FREQ_PER_BIN); // ~3   - Sub bass
constexpr s32 BASS_END     = (s32)(250  / FREQ_PER_BIN); // ~12  - Bass
constexpr s32 LOW_MID_END  = (s32)(500  / FREQ_PER_BIN); // ~23  - Low mid
constexpr s32 MID_END      = (s32)(2000 / FREQ_PER_BIN); // ~93  - Mid
constexpr s32 HIGH_MID_END = (s32)(4000 / FREQ_PER_BIN); // ~186 - High mid
constexpr s32 HIGH_END     = (s32)(8000 / FREQ_PER_BIN); // ~372 - High

struct Bands
{
	f32 sub_bass = 0.0f;
	f32 bass     = 0.0f;
	f32 low_mid  = 0.0f;
	f32 mid      = 0.0f;
	f32 high_mid = 0.0f;
	f32 high     = 0.0f;
	f32 low()    const { return (sub_bass + bass) * 2.0f; }
	f32 medium() const { return (low_mid  + mid)  * 2.0f; }
	f32 treble() const { return (high_mid + high) * 2.0f; }
};

// Convert frequency to Mel scale
struct Mel { f32 min, max; };
f32 freq_to_mel(f32 freq) { return 2595.0f * log10(1.0f + freq / 700.0f); }
f32 mel_to_freq(f32 mel)  { return 700.0f * (pow(10.0f, mel / 2595.0f) - 1.0f); }

Bands extract_bands(const f32* fft)
{
	Bands b;
	// Define mel boundaries
	Mel ranges[] = {
		{ freq_to_mel(20.0f),   freq_to_mel(60.0f)    }, // sub bass
		{ freq_to_mel(60.0f),   freq_to_mel(250.0f)   }, // bass
		{ freq_to_mel(250.0f),  freq_to_mel(500.0f)   }, // low mid
		{ freq_to_mel(500.0f),  freq_to_mel(2000.0f)  }, // mid
		{ freq_to_mel(2000.0f), freq_to_mel(4000.0f)  }, // high mid
		{ freq_to_mel(4000.0f), freq_to_mel(10000.0f) }  // high
	};

	f32* bands[] = { &b.sub_bass, &b.bass, &b.low_mid, &b.mid, &b.high_mid, &b.high };

	for (s32 band = 0; band < 6; band++)
	{
		f32 sum = 0.0f;
		s32 count = 0;
		for (s32 i = 1; i < BINS; i++)
		{
			f32 freq = i * FREQ_PER_BIN;
			f32 mel = freq_to_mel(freq);

			if (mel >= ranges[band].min && mel <= ranges[band].max)
			{
				f32 magnitude = fft[i];
				sum += magnitude * magnitude;
				count++;
			}
		}

		*bands[band] = count > 0 ? sqrt(sum / count) : 0.0f;
	}

	// Compensate for low frequency energy
	b.sub_bass *= 3.0f;
	b.bass     *= 2.5f;
	b.low_mid  *= 1.8f;
	b.mid      *= 1.2f;

	return b;
}

// Improved smoothing with adaptive attack/release
f32 smoothing(f32 prev, f32 curr, f32 attack, f32 release)
{
	f32 alpha = curr > prev ? attack : release;
	return prev + alpha * (curr - prev);
}

struct BandProcessor
{
	Bands smooth;
	Bands average;
	Bands motion;
	Bands peak;

	// Adaptive thresholds
	f32 noise_floor = 0.0f;
	Bands update(const Bands& raw)
	{
		// Envelope following with different attack/release
		auto env = [](f32 p, f32 c) {
			return smoothing(p, c, 0.25f, 0.08f); // Fast attack, slower release
		};

		smooth.sub_bass = env(smooth.sub_bass, raw.sub_bass);
		smooth.bass     = env(smooth.bass,     raw.bass);
		smooth.low_mid  = env(smooth.low_mid,  raw.low_mid);
		smooth.mid      = env(smooth.mid,      raw.mid);
		smooth.high_mid = env(smooth.high_mid, raw.high_mid);
		smooth.high     = env(smooth.high,     raw.high);

		// Long-term average (for adaptive threshold)
		auto avg = [](f32 p, f32 c) {
			return smoothing(p, c, 0.003f, 0.003f); // Very slow
		};

		average.sub_bass = avg(average.sub_bass, smooth.sub_bass);
		average.bass     = avg(average.bass,     smooth.bass);
		average.low_mid  = avg(average.low_mid,  smooth.low_mid);
		average.mid      = avg(average.mid,      smooth.mid);
		average.high_mid = avg(average.high_mid, smooth.high_mid);
		average.high     = avg(average.high,     smooth.high);

		// Peak tracking
		auto track_peak = [](f32 p, f32 c) {
			if (c > p) return c;
			return smoothing(p, c, 0.0f, 0.001f); // Only decay
		};

		peak.sub_bass = track_peak(peak.sub_bass, smooth.sub_bass);
		peak.bass     = track_peak(peak.bass,     smooth.bass);
		peak.low_mid  = track_peak(peak.low_mid,  smooth.low_mid);
		peak.mid      = track_peak(peak.mid,      smooth.mid);
		peak.high_mid = track_peak(peak.high_mid, smooth.high_mid);
		peak.high     = track_peak(peak.high,     smooth.high);

		// Delta
		Bands delta;
		auto calc_delta = [](f32 s, f32 a, f32 p) {
			f32 threshold = a * 1.2f; // 20% above average
			f32 normalized = (s - threshold) / (p - threshold + 0.001f);
			return std::max(normalized, 0.0f);
		};

		delta.sub_bass = calc_delta(smooth.sub_bass, average.sub_bass, peak.sub_bass);
		delta.bass     = calc_delta(smooth.bass,     average.bass,     peak.bass);
		delta.low_mid  = calc_delta(smooth.low_mid,  average.low_mid,  peak.low_mid);
		delta.mid      = calc_delta(smooth.mid,      average.mid,      peak.mid);
		delta.high_mid = calc_delta(smooth.high_mid, average.high_mid, peak.high_mid);
		delta.high     = calc_delta(smooth.high,     average.high,     peak.high);

		// Motion smoothing
		auto mot = [](f32 p, f32 c) {
			return smoothing(p, c, 0.15f, 0.10f);
		};

		motion.sub_bass = mot(motion.sub_bass, delta.sub_bass);
		motion.bass     = mot(motion.bass,     delta.bass);
		motion.low_mid  = mot(motion.low_mid,  delta.low_mid);
		motion.mid      = mot(motion.mid,      delta.mid);
		motion.high_mid = mot(motion.high_mid, delta.high_mid);
		motion.high     = mot(motion.high,     delta.high);

		return motion;
	}
};

// Soft knee Compression
f32 compress(f32 x, f32 threshold = 0.5f, f32 ratio = 4.0f)
{
	if (x < threshold) return x;

	// Soft knee
	f32 knee = 0.1f;
	if (x < threshold + knee) 
	{
		f32 t = (x - threshold) / knee;
		return threshold + t * t * knee / ratio;
	}

	// Above knee
	return threshold + (x - threshold) / ratio;
}