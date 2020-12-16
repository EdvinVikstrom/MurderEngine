#include "PortAudio.hpp"

#include <portaudio.h>

me::PortAudio::PortAudio(const MurderEngine* engine)
  : AudioSystem(engine, "portaudio")
{
}

int me::PortAudio::initialize()
{
  logger = engine->get_logger().child("PortAudio");

  logger->info("using version [%s]", Pa_GetVersionText());


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

int me::PortAudio::terminate()
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

int me::PortAudio::tick()
{
  return 0;
}


int me::PortAudio::pa_stream_callback(const void* input, void* output, uint64_t frame_count,
    const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void* user_data)
{
  return 0;
}
