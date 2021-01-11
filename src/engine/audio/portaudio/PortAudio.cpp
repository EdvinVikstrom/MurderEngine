#include "PortAudio.hpp"

#include <portaudio.h>

me::PortAudio::PortAudio()
  : AudioSystem("portaudio"), logger("PortAudio")
{
  for (size_t i = 0; i < 4; i++)
    tracks[i] = nullptr;
}

int me::PortAudio::initialize(const ModuleInfo module_info)
{
  logger.info("using version [%s]", Pa_GetVersionText());


  PaError error = Pa_Initialize();
  if (error != paNoError)
    throw exception("failed to initialize [%s]", Pa_GetErrorText(error));


  error = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, pa_stream_callback, this);
  if (error != paNoError)
    throw exception("failed to open default stream [%s]", Pa_GetErrorText(error));


  error = Pa_StartStream(stream);
  if (error != paNoError)
    throw exception("failed to start stream [%s]", Pa_GetErrorText(error));
  return 0;
}

int me::PortAudio::terminate(const ModuleInfo module_info)
{
  PaError error = Pa_AbortStream(stream);
  if (error != paNoError)
    throw exception("failed to abort stream [%s]", Pa_GetErrorText(error));


  error = Pa_CloseStream(stream);
  if (error != paNoError)
    throw exception("failed to close stream [%s]", Pa_GetErrorText(error));


  error = Pa_Terminate();
  if (error != paNoError)
    throw exception("failed to terminate [%s]", Pa_GetErrorText(error));
  return 0;
}

int me::PortAudio::tick(const ModuleInfo module_info)
{
  return 0;
}


int me::PortAudio::push(const AudioTrack* track)
{
  /* find a free strip */
  for (size_t i = 0; i < 4; i++)
  {
    if (tracks[i] == nullptr)
    {
      tracks[i] = track;
      break;
    }
  }

  logger.err("no free space for audio strip");
  return 0;
}


int me::PortAudio::pa_stream_callback(const void* input, void* output, uint64_t frame_count,
    const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void* user_data)
{
  float* audio_output = reinterpret_cast<float*>(output);

  PortAudio* system = reinterpret_cast<PortAudio*>(user_data);

  for (size_t i = 0; i < 4; i++)
  {
    const AudioTrack* track = system->tracks[i];
    if (track == nullptr)
      continue;

    /* check if track ended */
    if (track->position >= track->length)
    {
      system->tracks[i] = nullptr;
      continue;
    }

    for (size_t j = 0; j < 256 && track->position < track->length; j++)
      audio_output[j] = track->data[track->position++];
  }
  return 0;
}
