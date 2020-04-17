#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <vector>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

static void list_audio_devices(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}

#define TEST_ERROR(_msg)		\
	error = alGetError();		\
	if (error == AL_OUT_OF_MEMORY ) {	\
		printf("AL_OUT_OF_MEMORY");	\
	\
	}				\
	if (error == AL_INVALID_VALUE ) {	\
		printf("AL_INVALID_VALUE");	\
		\
	}				\
	if (error == AL_INVALID_ENUM ) {	\
		printf("AL_INVALID_ENUM");	\
	\
	}				\
	if (error != AL_NO_ERROR) {	\
		fprintf(stderr, _msg "\n");	\
	\
	}

class AudioHandler{

private:
	ALboolean enumeration;
	const ALCchar *devices;
	const ALCchar *defaultDeviceName;
	int ret;
	int sound_bees;
	char *bufferData;
	ALCdevice *device;
	ALvoid *data;
	ALCcontext *context;
	ALsizei size, freq;
	ALenum format;
	ALuint buffer[16];
	ALuint* source;
	ALboolean loop = AL_FALSE;
	ALCenum error;
	ALint source_state;

public:
	AudioHandler(int num_sound_bees){
		sound_bees = num_sound_bees;
		source = (ALuint*)malloc(sizeof(ALuint)*sound_bees);
		ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
		if (enumeration == AL_FALSE)
			fprintf(stderr, "enumeration extension not available\n");

		list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		device = alcOpenDevice(defaultDeviceName);
		if (!device) {
			fprintf(stderr, "unable to open default device\n");
		}

		fprintf(stdout, "Device: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

		alGetError();

		context = alcCreateContext(device, NULL);
		if (!alcMakeContextCurrent(context)) {
			fprintf(stderr, "failed to make default context\n");
		}
		TEST_ERROR("make default context");

		/* set orientation */
		alListener3f(AL_POSITION, 0, 0, 1.0f);
		TEST_ERROR("listener position");
			alListener3f(AL_VELOCITY, 0, 0, 0);
		TEST_ERROR("listener velocity");
		alListenerfv(AL_ORIENTATION, listenerOri);
		TEST_ERROR("listener orientation");

		alGenSources((ALuint)num_sound_bees, source);
		TEST_ERROR("source generation");

		for(int i=0;i<num_sound_bees;i++){
			alSourcef(source[i], AL_PITCH, 1);
			TEST_ERROR("source pitch");
			alSourcef(source[i], AL_GAIN, 1);
			TEST_ERROR("source gain");
			alSource3f(source[i], AL_POSITION, 0, 0, 0);
			TEST_ERROR("source position");
			alSource3f(source[i], AL_VELOCITY, 0, 0, 0);
			TEST_ERROR("source velocity");
			alSourcei(source[i], AL_LOOPING, AL_FALSE);
			TEST_ERROR("source looping");
		}

		alGenBuffers(16, buffer);
		TEST_ERROR("buffer generation");

		for(int i=1;i<=16;i++){
			int length = snprintf( NULL, 0, "%d", i );
			char* str = (char*)malloc( length + 1 );
			snprintf( str, length + 1, "%d", i);
			char flnm[] = ".wav";
			strcat(str,flnm);

			alutLoadWAVFile("1.wav", &format, &data, &size, &freq, (ALboolean*)&loop);
			TEST_ERROR("loading wav file");

			alBufferData(buffer[i-1], format, data, size, freq);
			TEST_ERROR("buffer copy");		
		}
		for(int i=0;i<num_sound_bees;i++){
			alSourcei(source[i], AL_BUFFER, buffer[i%16]);
			TEST_ERROR("buffer binding");
		}
	}

	void play_sound(int i){
		alGetSourcei(source[i], AL_SOURCE_STATE, &source_state);
		TEST_ERROR("source state get");
		if(source_state != AL_PLAYING) {
			//printf("bing\n");
			alSourcePlay(source[i]);
		}

	}

	void delete_sources(){
		alDeleteSources(sound_bees, source);
		alDeleteBuffers(16, buffer);
		device = alcGetContextsDevice(context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}

};
