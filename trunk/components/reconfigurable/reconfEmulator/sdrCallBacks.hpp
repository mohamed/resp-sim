/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   The following code is derived, directly or indirectly, from the SystemC
 *   source code Copyright (c) 1996-2004 by all Contributors.
 *   All Rights reserved.
 *
 *   The contents of this file are subject to the restrictions and limitations
 *   set forth in the SystemC Open Source License Version 2.4 (the "License");
 *   You may not use this file except in compliance with such restrictions and
 *   limitations. You may obtain instructions on how to receive a copy of the
 *   License at http://www.systemc.org/. Software distributed by Contributors
 *   under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
 *   ANY KIND, either express or implied. See the License for the specific
 *   language governing rights and limitations under the License.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
 *   Component developed by: Fabio Arlati arlati.fabio@gmail.com
 *
\***************************************************************************/

#include "reconfEmulator.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace reconfEmu {

template <typename issueWidth> class openWavToRead16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	openWavToRead16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		ssize_t unusedRV;
		char path[512];
		unsigned int samples;
		int i;
		i = 0;
		do{
			path[i] = processorInstance.readCharMem(callArgs[0]+i);
			i++;
		} while(path[i-1] != '\0' && i < 512);

		printf("\n(SysCall) Opening %s...\n\n", path);
		int wavfile = ::open(path, O_RDONLY);
		if(wavfile == -1) {
			printf("\n(SysCall) ERROR: File %s cannot be opened!\n\n", path);
			processorInstance.setRetVal(-1);
			processorInstance.returnFromCall();
			processorInstance.postCall();
			return true;
		}

		char	myChunkID[5];
		int 	myChunkSize;
		char	myFormat[5];
		char	mySubChunk1ID[5];
		int	mySubChunk1Size;
		short 	myAudioFormat;
		short 	myChannels;
		int   	mySampleRate;
		int   	myByteRate;
		short 	myBlockAlign;
		short 	myBitsPerSample;
		char	mySubChunk2ID[5];
		int	myDataSize;

//		lseek( wavfile, 0, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) myChunkID, 4);
		myChunkID[4]='\0';
//		lseek( wavfile, 4, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myChunkSize, 4);
//		lseek( wavfile, 8, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myFormat, 4);
		myFormat[4]='\0';
//		lseek( wavfile, 12, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &mySubChunk1ID, 4);
		mySubChunk1ID[4]='\0';
//		lseek( wavfile, 16, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &mySubChunk1Size, 4);
//		lseek( wavfile, 20, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myAudioFormat, 2);
//		lseek( wavfile, 22, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myChannels, 2);
//		lseek( wavfile, 24, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &mySampleRate, 4);
//		lseek( wavfile, 28, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myByteRate, 4);
//		lseek( wavfile, 32, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myBlockAlign, 2);
//		lseek( wavfile, 34, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myBitsPerSample, 2);
//		lseek( wavfile, 36, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &mySubChunk2ID, 4);
		mySubChunk2ID[4]='\0';
//		lseek( wavfile, 40, SEEK_SET);
		unusedRV = ::read( wavfile, (char*) &myDataSize, 4);

		unsigned int myNumSamples = myDataSize / ( myChannels * myBitsPerSample/8 );

//		printf("\n\t(SysCall)\n ChunkID: %s\n ChunkSize: %d\n Format: %s\n SubChunk1ID: %s\n SubChunk1Size: %d\n AudioFormat: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n SubChunk2ID: %s\n DataSize: %d\n NumSamples: %d\n\n", myChunkID, myChunkSize, myFormat, mySubChunk1ID, mySubChunk1Size, myAudioFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, mySubChunk2ID, myDataSize, myNumSamples);

		samples = myNumSamples;
		processorInstance.writeMem(callArgs[1],samples);

		if (myAudioFormat!=1 || mySampleRate!=44100 || myBitsPerSample!=16 || myChannels!=1) {
			printf("\n(SysCall) ERROR: File %s has an unknown format or a requirement (16-bit, 44100 Hz, mono wav) is missing!\n\n", path);
			processorInstance.setRetVal(-1);
			processorInstance.returnFromCall();
			processorInstance.postCall();
			return true;
		}

		processorInstance.setRetVal(wavfile);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class openWavToWrite16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	openWavToWrite16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		ssize_t unusedRV;
		char path[512];
		unsigned int samples,channels;
		int i;
		i = 0;
		do{
			path[i] = processorInstance.readCharMem(callArgs[0]+i);
			i++;
		} while(path[i-1] != '\0' && i < 512);
		samples = callArgs[1];

		printf("\n(SysCall) Creating new %s...\n\n", path);

		int wavfile = open(path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
		if(wavfile == -1) {
			printf("\n(SysCall) ERROR: File %s cannot be created!\n\n", path);
			processorInstance.setRetVal(-1);
			processorInstance.returnFromCall();
			processorInstance.postCall();
			return true;
		}

		char	myChunkID[5];
		char	myFormat[5];
		char	mySubChunk1ID[5];
		int	mySubChunk1Size = 16;
		short 	myAudioFormat = 1;
		short 	myChannels = 1;
		int   	mySampleRate = 44100;
		short 	myBitsPerSample = 16;
		int   	myByteRate = mySampleRate * myChannels * myBitsPerSample/8;
		short 	myBlockAlign = myChannels * myBitsPerSample/8;
		char	mySubChunk2ID[5];
		int	myDataSize = samples * myChannels * myBitsPerSample/8;
		int 	myChunkSize = myDataSize + 32;

//		lseek( wavfile, 0, SEEK_SET);
		unusedRV = write( wavfile, (char*) "RIFF", 4);
//		lseek( wavfile, 4, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myChunkSize, 4);
//		lseek( wavfile, 8, SEEK_SET);
		unusedRV = write( wavfile, (char*) "WAVE", 4);
//		lseek( wavfile, 12, SEEK_SET);
		unusedRV = write( wavfile, (char*) "fmt ", 4);
//		lseek( wavfile, 16, SEEK_SET);
		unusedRV = write( wavfile, (char*) &mySubChunk1Size, 4);
//		lseek( wavfile, 20, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myAudioFormat, 2);
//		lseek( wavfile, 22, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myChannels, 2);
//		lseek( wavfile, 24, SEEK_SET);
		unusedRV = write( wavfile, (char*) &mySampleRate, 4);
//		lseek( wavfile, 28, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myByteRate, 4);
//		lseek( wavfile, 32, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myBlockAlign, 2);
//		lseek( wavfile, 34, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myBitsPerSample, 2);
//		lseek( wavfile, 36, SEEK_SET);
		unusedRV = write( wavfile, (char*) "data", 4);
//		lseek( wavfile, 40, SEEK_SET);
		unusedRV = write( wavfile, (char*) &myDataSize, 4);

		processorInstance.setRetVal(wavfile);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class readSample16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	readSample16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		ssize_t unusedRV;
		int wavfile = callArgs[0];
		int myIntData;
		int16_t my16Data;

		unusedRV = ::read( wavfile, (int16_t*) &my16Data, 2);
		myIntData=my16Data;

		processorInstance.setRetVal(myIntData);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class writeSample16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	writeSample16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		ssize_t unusedRV;
		int wavfile = callArgs[0];
		int sample = callArgs[1];
		unusedRV = ::write(wavfile, (char*) &sample, 2);

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class seekFirstSample16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	seekFirstSample16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		int wavfile = callArgs[0];
		printf("\n(SysCall) Resetting file %d...\n\n",wavfile);
		::lseek( wavfile, 44, SEEK_SET);

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class closeWav16Call : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	closeWav16Call(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		int wavfile = callArgs[0];
		int sample = callArgs[1];
		printf("\n(SysCall) Closing file %d...\n\n",wavfile);
		::close(wavfile);

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class printoutCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	printoutCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		char text[512];
		int param,i,j;
		i = 0;
		do{
			text[i] = processorInstance.readCharMem(callArgs[0]+i);
			i++;
		} while(text[i-1] != '\0' && i < 512);
		param = callArgs[1];

		for (i=0; i<512; i++) {
			if (text[i] != '\n') break;
		}
		char text2[522];
		strcpy(text2,"\0");

		for (j=0; j<i; j++) {
			strcat(text2,"\n");
		}
		if (i!=0) strcat(text2,"(SysCall) ");
		strcat(text2,&text[i]);

		printf(text2,param);

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class printresultCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	printresultCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		char text[512],res[5];
		int i,j;
		i = 0;
		do{
			text[i] = processorInstance.readCharMem(callArgs[0]+i);
			i++;
		} while(text[i-1] != '\0' && i < 512);
		i = 0;
		do{
			res[i] = processorInstance.readCharMem(callArgs[1]+i);
			i++;
		} while(res[i-1] != '\0' && i < 5);

		for (i=0; i<512; i++) {
			if (text[i] != '\n') break;
		}
		char text2[522];
		strcpy(text2,"\0");

		for (j=0; j<i; j++) {
			strcat(text2,"\n");
		}
		strcat(text2,"(SysCall) ");
		strcat(text2,&text[i]);

		printf(text2,res);

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class printStatusCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	printStatusCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();

		(this->cE)->printSystemStatus();

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class hcMixerCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	hcMixerCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("hcMixer", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("hcMixer");

		float in,osc; 
		float output[2];
		int i;
		get_buffer(processorInstance,0,(unsigned char*)&in,1*sizeof(float));
		get_buffer(processorInstance,1,(unsigned char*)&osc,1*sizeof(float));

		float rTmp = abs(in);

		// Half complex mixer
		float rI44k = in * cos ( osc );
		float rQ44k = in * sin ( osc );

		output[0] = rI44k;
		output[1] = rQ44k;

		set_buffer(processorInstance, 2, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class decimatorFilterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	decimatorFilterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("decimatorFilter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("decimatorFilter");

		int iDecimatorIdxIn = callArgs[0];
		float rDecimatorQueueI[64],rDecimatorQueueQ[64];
		float output[2];
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)rDecimatorQueueI, 64*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)rDecimatorQueueQ, 64*sizeof(float));

		float rDecIntCoeffs[64] = {   5566.813560324988,   23887.044687837064,   56163.025785228267,   107513.703556381680,   168621.188343821940,   223802.327332353220,   246104.113280885560,   207964.212934614710,   90582.578794280125,   -101631.300424710600,   -332583.665147412340,   -534546.264874449470,  -622538.621962122040,   -521199.182517471610,   -198339.454015620660,   306652.941743057050,   872454.899382253880,   1313584.976739374900,   1428128.708473922200,  1066424.293393449600,   201468.132346975360,   -1023471.474557211500,   -2292006.967941345600,   -3167506.806456387500,   -3195752.993445751700,  -2033752.462656715200,   431234.697026152050,   4010325.271783742600,   8216844.745709528200,   12352536.074754233000,   15658827.490046050000,  17495007.205536410000,   17495007.205536410000,   15658827.490046050000,   12352536.074754233000,   8216844.745709528200,   4010325.271783742600,  431234.697026152050,   -2033752.462656715200,   -3195752.993445751700,   -3167506.806456387500,   -2292006.967941345600,   -1023471.474557211500,  201468.132346975360,   1066424.293393449600,   1428128.708473922200,   1313584.976739374900,   872454.899382253880,   306652.941743057050,   -198339.454015620660,  -521199.182517471610,   -622538.621962122040,   -534546.264874449470,   -332583.665147412340,   -101631.300424710600,   90582.578794280125,   207964.212934614710,  246104.113280885560,   223802.327332353220,   168621.188343821940,   107513.703556381680,   56163.025785228267,   23887.044687837064,   5566.813560324988};

		float rI11k = 0.0;
		float rQ11k = 0.0;			// clear MAC accumulators
		float rTmp;

		for (i = 0; i < 64; i++) {
			rTmp = rDecIntCoeffs[i];
			rI11k += rDecimatorQueueI[iDecimatorIdxIn] * rTmp;
			rQ11k += rDecimatorQueueQ[iDecimatorIdxIn] * rTmp;
			if (++iDecimatorIdxIn >= 64) iDecimatorIdxIn = 0;
		}

		rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
		rQ11k *= 1.0e-8;

		output[0] = rI11k;
		output[1] = rQ11k;

		set_buffer(processorInstance, 3, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class CW300FilterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	CW300FilterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("CW300Filter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("CW300Filter");

		int iMainFilterIdxIn = callArgs[0];
		float rMainFilterQueueI[428],rMainFilterQueueQ[428];
		float output[2];
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)rMainFilterQueueI, 428*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)rMainFilterQueueQ, 428*sizeof(float));

		int FIR_LEN_CW300H = 428;
		float rCw300HCoeffs_I[428] = {   -1882.845627579306,   -985.241220746677,   -1094.708621296679,   -970.433547873299,   -554.355932184961,  152.048089185151,   1070.661055049873,   2048.066142774273,   2877.672940059831,   3338.712161537691,   3249.921532839887,   2519.122889984304,   1183.179098849271,   -581.564490588538,   -2479.210413224527,   -4147.745447945033,   -5236.167798113964,   -5486.084286246476,   -4802.772747841721,  -3288.897381685154,   -1236.230019639332,   934.647650908532,   2770.174852818620,   3896.878123090475,   4122.988059767328,   3497.143404598948,   2309.557643131611,   1021.599956557861,   143.807370307565,   82.690908148376,   1001.503299180149,   2732.707805941988,   4773.529155266234,   6380.849461742558,  6746.976431831227,   5227.756725017461,   1559.806782606384,   -3985.333169088263,   -10566.334731954781,   -16880.110619006064,   -21404.583446375604,  -22736.817208804790,   -19947.462265296552,   -12873.986489780697,   -2264.440542744097,   10272.606619198961,   22526.484563956259,   32108.269836106654,  36954.344391931423,   35802.291211959993,   28518.273795816142,   16196.681363957856,   984.794044414206,   -14335.953970962308,   -26932.496757350124,  -34535.495722532607,   -35941.971166746080,   -31285.642967487474,   -21994.506187119925,   -10443.778787110106,   628.606936071253,   8799.207867825462,  12566.454050240476,   11729.961535244525,   7434.220337156994,   1857.308137769335,   -2396.212467506152,   -3043.288873924376,   1172.251619406221,  10015.190811818600,   21671.083695729776,   33086.455956571808,   40680.919611807491,   41266.358569279953,   32956.245243062709,   15819.287460793343,  -7902.906441385021,   -34110.101908982986,   -57645.903571585201,   -73423.089637474419,   -77616.462043051331,   -68627.087104165126,   -47575.743307638979,  -18170.751627308968,   14039.408744615372,   42879.155301767059,   62942.333564790446,   70835.569207916604,   65959.867818574858,   50616.574647183945,  29400.601981404252,   8007.282774171345,   -8250.025115277798,   -15842.759869807816,   -13978.674529533324,   -4835.580947835598,   7008.668286828787,  15839.447714005613,   16448.086961397974,   5741.089578911057,   -16127.510692180862,   -45430.448134915248,   -75493.913060667284,   -98179.600738657886,  -105891.173066217540,   -93627.516934312356,   -60547.762904785210,   -10621.751190601299,   47869.761548688992,   103929.870710933130,   146208.499274218600,  165594.594212664990,   157407.828801789700,   122640.769442784910,   67898.455589903169,   4008.900334985615,   -56407.999963660091,   -101842.219391093210,  -124564.252075649480,   -122396.878817224230,   -99119.389838146555,   -63379.473719240479,   -26349.337075875483,   1290.444501904358,   12277.952618206333,  4830.426916389242,   -16736.687861982893,   -43052.890502719871,   -62198.546024149611,   -63067.796258166702,   -38741.830627019423,   11038.381340541324,  79010.757655278911,   151483.889190466750,   211230.458297487260,   241554.560560942390,   230572.828549811380,   174625.996263846810,   79910.947164016427,  -38193.018666216325,   -158064.462798801050,   -256446.968897921500,   -313673.614052198420,   -318255.265072387180,   -269690.427743282050,   -178738.686959640820,   -65016.371524645350,   47578.138052422641,   136529.112319522860,   186076.351005221280,   190939.550093422440,   157409.365053829530,  101466.670405719660,   44331.603937393113,   6534.701531241162,   2015.478766203268,   33820.412897808208,   92618.372782086313,   158575.585245377120,  206283.023438822830,   211606.302899449480,   158770.599042511810,   45817.952297554330,   -113108.453063516910,   -289488.198748301020,   -446010.930398957160,  -544890.058045211130,   -557005.831560903000,   -469646.446896393730,   -290840.332674446110,   -48992.949213418098,   212356.285639662060,   443651.910164795060,  600295.389185191600,   652894.333416660080,   594264.760863436500,   441339.570519539760,   231320.233026999950,   12798.216865715814,   -166132.032879667090,  -269907.039951691400,   -284636.008126954610,   -221889.999519663690,   -115907.958319286030,   -14588.238948070679,   33752.810441349066,   -5324.600965876536,  -140708.070394304520,   -349941.527006545160,   -581343.336405657120,   -764226.394367903350,   -825397.420728683590,   -708403.101284807550,   -391003.274562636680,   103606.961198460170,   705198.291287792380,   1305905.316675710700,   1779532.443420702800,   2007288.192921554700,   1904709.438240613100,  1443859.287712010300,   665490.482002676000,   -322288.869370070870,   -1359593.057862617800,   -2262283.448770906300,   -2856443.813546707400,  -3013057.531132105700,   -2675754.394863571000,   -1875621.147402196900,   -729508.334460824150,   578456.570974855220,   1829008.188231649100,  2807525.742939385100,   3343770.712421974200,   3343770.712421994200,   2807525.742939443800,   1829008.188231735700,   578456.570974955680,   -729508.334460726240,   -1875621.147402117000,   -2675754.394863521200,   -3013057.531132091800,   -2856443.813546729700,   -2262283.448770958500,  -1359593.057862689700,   -322288.869370149680,   665490.482002603590,   1443859.287711954900,   1904709.438240581200,   2007288.192921548400,   1779532.443420719800,   1305905.316675744300,   705198.291287834640,   103606.961198502530,   -391003.274562601060,   -708403.101284782980,   -825397.420728671480,  -764226.394367902190,   -581343.336405663520,   -349941.527006555170,   -140708.070394313980,   -5324.600965883032,   33752.810441346453,   -14588.238948070222,  -115907.958319284550,   -221889.999519663690,   -284636.008126958270,   -269907.039951699790,   -166132.032879679990,   12798.216865700206,   231320.233026984380,  441339.570519527190,   594264.760863429870,   652894.333416661130,   600295.389185200450,   443651.910164810480,   212356.285639681450,   -48992.949213398075,  -290840.332674428760,   -469646.446896381680,   -557005.831560897760,   -544890.058045212760,   -446010.930398964610,   -289488.198748312130,   -113108.453063529230,   45817.952297543125,   158770.599042503370,   211606.302899444590,   206283.023438821230,   158575.585245377850,   92618.372782088045,  33820.412897809649,   2015.478766203539,   6534.701531240027,   44331.603937391017,   101466.670405717540,   157409.365053828570,   190939.550093423630,  186076.351005225180,   136529.112319529350,   47578.138052430906,   -65016.371524636619,   -178738.686959633230,   -269690.427743277050,   -318255.265072385720,  -313673.614052200860,   -256446.968897927440,   -158064.462798809460,   -38193.018666225667,   79910.947164007724,   174625.996263840120,   230572.828549807570,  241554.560560941610,   211230.458297489240,   151483.889190470630,   79010.757655283625,   11038.381340545837,   -38741.830627015879,   -63067.796258164512,  -62198.546024148687,   -43052.890502719805,   -16736.687861983079,   4830.426916389380,   12277.952618207162,   1290.444501905932,   -26349.337075873442,  -63379.473719238493,   -99119.389838145304,   -122396.878817224230,   -124564.252075651080,   -101842.219391096380,   -56407.999963664464,   4008.900334980725,  67898.455589898585,   122640.769442781410,   157407.828801787950,   165594.594212665250,   146208.499274220750,   103929.870710936750,   47869.761548693357,  -10621.751190596955,   -60547.762904781601,   -93627.516934309941,   -105891.173066216540,   -98179.600738658191,   -75493.913060668550,   -45430.448134916980,  -16127.510692182615,   5741.089578909653,   16448.086961397101,   15839.447714005244,   7008.668286828733,   -4835.580947835620,   -13978.674529533584,  -15842.759869808491,   -8250.025115278906,   8007.282774169954,   29400.601981402855,   50616.574647182882,   65959.867818574450,   70835.569207917040,  62942.333564791763,   42879.155301769097,   14039.408744617811,   -18170.751627306527,   -47575.743307636949,   -68627.087104163846,   -77616.462043050968,  -73423.089637474986,   -57645.903571586532,   -34110.101908984790,   -7902.906441386954,   15819.287460791618,   32956.245243061436,   41266.358569279262,  40680.919611807367,   33086.455956572121,   21671.083695730333,   10015.190811819199,   1172.251619406700,   -3043.288873924098,   -2396.212467506069,  1857.308137769308,   7434.220337156983,   11729.961535244654,   12566.454050240834,   8799.207867826055,   628.606936072020,   -10443.778787109299,  -21994.506187119241,   -31285.642967487071,   -35941.971166746080,   -34535.495722533058,   -26932.496757350964,   -14335.953970963419,   984.794044413005,  16196.681363956763,   28518.273795815334,   35802.291211959593,   36954.344391931481,   32108.269836107131,   22526.484563957041,   10272.606619199898,  -2264.440542743172,   -12873.986489779927,   -19947.462265296042,   -22736.817208804576,   -21404.583446375669,   -16880.110619006347,   -10566.334731955185,  -3985.333169088698,   1559.806782606003,   5227.756725017183,   6746.976431831070,   6380.849461742507,   4773.529155266257,   2732.707805942040,   1001.503299180192,   82.690908148387,   143.807370307540,   1021.599956557812,   2309.557643131563,   3497.143404598927,   4122.988059767353,   3896.878123090556,  2770.174852818751,   934.647650908695,   -1236.230019639166,   -3288.897381685014,   -4802.772747841632,   -5486.084286246451,   -5236.167798114005,  -4147.745447945129,   -2479.210413224659,   -581.564490588680,   1183.179098849142,   2519.122889984207,   3249.921532839833,   3338.712161537681,  2877.672940059857,   2048.066142774326,   1070.661055049937,   152.048089185213,   -554.355932184911,   -970.433547873265,   -1094.708621296663,   -985.241220746676,   -1882.845627579326};

		float rCw300HCoeffs_Q[428] = {   685.299764157141,   -49.171345093264,   -527.183887006259,   -1099.634889522889,   -1649.956590685424,  -2028.940746798781,   -2088.401650257126,   -1718.531545028580,   -887.644545982106,   334.088541337310,   1769.812938348897,   3158.880189738539,  4205.268499371305,   4640.774459506123,   4294.118398358750,   3141.795086367338,   1333.292151300006,   -826.894159011073,   -2934.913331976663,  -4574.577692407867,   -5416.277608559994,   -5300.650230346101,   -4289.069602005046,   -2656.846517985679,   -833.480017678601,   706.962791169006,  1574.629739093548,   1581.746117803201,   815.572124752304,   -362.292540320469,   -1393.006263076245,   -1669.922978483661,   -719.493753724224,   1624.763917808794,   5110.636047317069,   9054.740257340007,   12446.962625173464,   14164.716104068129,   13249.764668339320,   9192.418300688252,  2141.851622884295,   -7013.379285544304,   -16737.908229732853,   -25111.639677033887,   -30216.859090424990,   -30574.859960570244,   -25525.610093664753,  -15462.527806892564,   -1844.314653638033,   13030.968319683629,   26461.091103664319,   35895.533166247566,   39488.994651360052,   36527.414999409513,  27612.525001517988,   14557.932231368744,    0.000000000227,   -13188.004417099026,   -22549.853341099228,   -26610.314367044462,   -25206.342460269323,  -19501.047823235302,   -11659.965391110627,   -4269.356847916582,   371.026512086699,   894.432458454373,   -2715.238810276067,   -9057.889062537768,  -15642.610761244170,   -19535.352407179769,   -18184.198336299087,   -10205.820045386352,   4070.739984697907,   22472.461127588103,   41325.824413085167,  56225.089934459327,   63063.696223838720,   59080.429557711679,   43665.075126571086,   18695.815892894781,   -11698.799318013025,   -41937.139950510536,  -66173.829334660928,   -79611.264575511828,   -79621.443571522832,   -66389.918086699865,   -42913.356405636121,   -14319.719247263834,   13334.074947236884,  34509.796266627272,   45521.035652597959,   45411.557221693802,   36145.721744385395,   22035.937116853867,   8542.190187642664,   728.846556589559,  1784.626233952901,   11997.915403243753,   28488.922304451880,   45812.807210051338,   57320.580420213548,   56967.980083427326,   41111.793854483287,  9824.350831356234,   -32663.101122922162,   -78562.814927624553,   -118102.780869186060,   -141737.419447099620,   -142477.105368919820,   -117766.860129478200,  -70410.302304404351,   -8264.482612127362,   57291.764324319964,   113794.004380626180,   150478.435044594920,   160751.829059067240,   143725.239920047170,  104413.670016657790,   52508.235432257279,    0.000000001209,   -41782.326556755550,   -64979.764721513027,   -67137.015944616549,   -51745.191111430155,  -27210.738145481708,   -4481.981189300603,   6091.656201966415,   -2148.680436418472,   -29953.241091961569,   -71464.500915888435,   -115309.199499897990,  -147296.962431784600,   -154116.184479753110,   -127110.075549023360,   -65155.967393977597,   24171.179450823522,   125563.270089959860,   218974.073239694490,  284020.389804539560,   304773.053420536390,   273775.680438611020,   194251.030326964450,   79871.116398591301,   -47969.262962767716,   -164804.389765459670,  -248610.374616457960,   -284855.335725855430,   -269829.029347984470,   -211388.412844259150,   -126864.708012894830,   -38599.262787192420,   31820.989647793456,  69178.804531159403,   68638.748439480842,   37060.133990753442,   -8830.389440874889,   -47041.329793959776,   -56597.909444350385,   -23901.423740409577,  52526.111977630848,   160285.545735022840,   274998.744289768920,   365618.579281123650,   402011.339739910560,   363006.719451796790,   242884.607301693730,  54524.474070779223,   -171813.544750834150,   -394080.160374473780,   -567305.056865384340,   -653765.473131860490,   -632046.367184453530,   -502718.728438265330,  -289087.023218785300,   -32584.601518750391,   216294.684227604830,   409503.277329596280,   512658.297712884670,   513192.296626528430,   423297.516299428480,  276722.019347053660,   119984.138933788490,    0.000000002981,   -48859.301625355227,   -14956.582611367863,   86000.758434684700,   213509.758974389990,  311841.116960980930,   324698.286206736000,   211591.670340671840,   -38140.953682751831,   -397490.448252118830,   -802718.298152528350,   -1163762.110926471400,  -1382538.816200499200,   -1375540.156558107400,   -1095784.669575069800,   -548913.062986169480,   200859.478736206950,   1037249.476188964700,  1810542.279956729600,   2365293.777855759000,   2571804.121056728500,   2354884.253836064600,   1713613.121277769100,   727339.967715175710,   -454145.350902351430,   -1635119.473456056800,   -2608830.153258861500,   -3196184.847825184900,   -3280590.234408389800,   -2831858.578883906400,  -1914138.640899612100,   -675957.832548291420,   675957.832548189210,   1914138.640899526200,   2831858.578883850000,   3280590.234408372500,   3196184.847825207300,   2608830.153258918800,   1635119.473456138700,   454145.350902443810,   -727339.967715088160,   -1713613.121277699700,   -2354884.253836023200,   -2571804.121056718800,   -2365293.777855779500,   -1810542.279956774100,   -1037249.476189022900,   -200859.478736268440,  548913.062986114990,   1095784.669575029500,   1375540.156558085900,   1382538.816200496200,   1163762.110926483300,   802718.298152550010,   397490.448252144100,  38140.953682775245,   -211591.670340654030,   -324698.286206725290,   -311841.116960976680,   -213509.758974389840,   -86000.758434685718,   14956.582611368307,  48859.301625358748,    0.000000003769,   -119984.138933779820,   -276722.019347045450,   -423297.516299423410,   -513192.296626528900,   -512658.297712891710,  -409503.277329609850,   -216294.684227623050,   32584.601518730382,   289087.023218766900,   502718.728438251830,   632046.367184447120,   653765.473131862000,  567305.056865393300,   394080.160374488100,   171813.544750851230,   -54524.474070762531,   -242884.607301680050,   -363006.719451787940,   -402011.339739907070,  -365618.579281125100,   -274998.744289773750,   -160285.545735029300,   -52526.111977637171,   23901.423740404716,   56597.909444347548,   47041.329793958736,  8830.389440874826,   -37060.133990753646,   -68638.748439482210,   -69178.804531162517,   -31820.989647798269,   38599.262787186584,   126864.708012889120,  211388.412844254960,   269829.029347983010,   284855.335725857410,   248610.374616463440,   164804.389765467930,   47969.262962777466,   -79871.116398581697,  -194251.030326956560,   -273775.680438606180,   -304773.053420535230,   -284020.389804542010,   -218974.073239699880,   -125563.270089966930,   -24171.179450830925,   65155.967393971121,   127110.075549018710,   154116.184479750660,   147296.962431784250,   115309.199499899190,   71464.500915890370,  29953.241091963471,   2148.680436419792,   -6091.656201965902,   4481.981189300454,   27210.738145481333,   51745.191111430111,   67137.015944617349,  64979.764721514955,   41782.326556758569,    0.000000002514,   -52508.235432253481,   -104413.670016654690,   -143725.239920045450,   -160751.829059067390,  -150478.435044597020,   -113794.004380629950,   -57291.764324324788,   8264.482612122289,   70410.302304399869,   117766.860129475010,   142477.105368918330,  141737.419447099940,   118102.780869187920,   78562.814927627434,   32663.101122925404,   -9824.350831353226,   -41111.793854480973,   -56967.980083425937,  -57320.580420213046,   -45812.807210051520,   -28488.922304452386,   -11997.915403244238,   -1784.626233953116,   -728.846556589411,   -8542.190187642234,  -22035.937116853380,   -36145.721744385140,   -45411.557221694049,   -45521.035652598861,   -34509.796266628822,   -13334.074947238902,   14319.719247261670,  42913.356405634193,   66389.918086698541,   79621.443571522395,   79611.264575512381,   66173.829334662383,   41937.139950512639,   11698.799318015404,  -18695.815892892530,   -43665.075126569318,   -59080.429557710631,   -63063.696223838488,   -56225.089934459815,   -41325.824413086171,   -22472.461127589366,  -4070.739984699153,   10205.820045385339,   18184.198336298421,   19535.352407179460,   15642.610761244132,   9057.889062537859,   2715.238810276140,  -894.432458454429,   -371.026512086927,   4269.356847916223,   11659.965391110241,   19501.047823235032,   25206.342460269309,   26610.314367044783,  22549.853341099897,   13188.004417099977,    0.000000000866,   -14557.932231367693,   -27612.525001517170,   -36527.414999409069,   -39488.994651360081,  -35895.533166248060,   -26461.091103665192,   -13030.968319684727,   1844.314653636900,   15462.527806891580,   25525.610093664065,   30574.859960569931,  30216.859090425063,   25111.639677034276,   16737.908229733464,   7013.379285545001,   -2141.851622883639,   -9192.418300687736,   -13249.764668338998,  -14164.716104068008,   -12446.962625173514,   -9054.740257340167,   -5110.636047317275,   -1624.763917808989,   719.493753724078,   1669.922978483578,  1393.006263076214,   362.292540320467,   -815.572124752308,   -1581.746117803232,   -1574.629739093619,   -706.962791169113,   833.480017678475,   2656.846517985559,   4289.069602004960,   5300.650230346073,   5416.277608560032,   4574.577692407968,   2934.913331976810,   826.894159011241,   -1333.292151299846,   -3141.795086367210,   -4294.118398358673,   -4640.774459506105,   -4205.268499371341,   -3158.880189738617,   -1769.812938348996,  -334.088541337413,   887.644545982018,   1718.531545028517,   2088.401650257094,   2028.940746798776,   1649.956590685442,   1099.634889522919,   527.183887006292,  49.171345093294,   -685.299764157084};

		float rI11k = 0.0;
		float rQ11k = 0.0;			// clear MAC accumulators

		for (i = 0; i < FIR_LEN_CW300H; i++) {
			rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rCw300HCoeffs_I[i];
			rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rCw300HCoeffs_Q[i];
			if (++iMainFilterIdxIn >= FIR_LEN_CW300H) iMainFilterIdxIn = 0;
		}

		rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
		rQ11k *= 1.0e-8;

		output[0] = rI11k;
		output[1] = rQ11k;

		set_buffer(processorInstance, 3, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class CW1KFilterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	CW1KFilterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("CW1KFilter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("CW1KFilter");

		int iMainFilterIdxIn = callArgs[0];
		float rMainFilterQueueI[428],rMainFilterQueueQ[428];
		float output[2];
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)rMainFilterQueueI, 428*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)rMainFilterQueueQ, 428*sizeof(float));

		int FIR_LEN_CW1K = 206;
		float rCw1kCoeffs_I[206] = {   3425.916797995238,   4858.354339454209,   6289.653728314946,   5322.812921556410,   452.220733624486,   -8734.760604818192,  -20831.984619100036,   -32583.700188171082,   -39818.163597717896,   -39151.041397179724,   -29793.431330613341,   -14531.592459389160,   833.236951096728,  9672.786019503517,   7577.854152659696,   -5118.372907260542,   -22731.129695456588,   -36199.413958022604,   -37076.239085741821,   -21834.131754635509,  5684.232130007719,   35202.834955947641,   54353.267105797990,   54655.421020078938,   36262.999932528015,   8856.011724039929,   -12285.076521924206,  -13564.990887834509,   9675.051931352842,   49760.897819693913,   89386.789296062401,   109707.790047866160,   100173.918815260370,   65086.098687933372,  23017.056574606137,   -1435.435089704458,   9941.748837217521,   58514.083012982454,   126068.318717698250,   182138.429479329350,   198626.361225714880,  164784.314659346040,   94661.075966249977,   22278.040895938357,   -14214.628976233213,   7834.227804398374,   82145.383908409494,   173996.326576700700,  235761.458983662830,   230771.824292378910,   153699.901033336790,   36338.532681775214,   -65694.585846302536,   -100140.860956495190,   -46123.859443098016,  71735.386820144893,   191906.086605070940,   244870.891755694170,   189027.366306881250,   35274.649379228846,   -153764.711586465860,   -291851.865960340250,  -312301.241536169550,   -204861.582938828190,   -26291.055630596395,   123372.304406807420,   149525.762822583460,   13359.189765235304,   -242495.255070951710,  -508817.995753122900,   -661121.579575707790,   -623858.128893216610,   -414969.899420858130,   -145489.941791921910,   29907.992906959262,   -9794.485996244597,  -284652.151222755610,   -692712.994890546080,   -1050910.014211895900,   -1182796.487034806100,   -1012105.232017683100,   -613906.115789878070,  -192154.070546358150,   12715.081042151656,   -154058.292397758200,   -669569.457572466340,   -1322511.374219769400,   -1798984.812347021000,   -1832754.043932128500,   -1351410.096298399100,   -543710.752776034640,   196483.450797993400,   438885.274271277360,   -68893.964198275149,   -1238381.309264987500,   -2612845.594949014500,   -3506209.593884463000,   -3266325.266497232500,   -1560628.935712501900,   1436999.795200099700,  5046411.554777901600,   8283144.605005032400,   10193142.444525592000,   10193142.444525611000,   8283144.605005079900,   5046411.554777966800,  1436999.795200168400,   -1560628.935712444400,   -3266325.266497194300,   -3506209.593884444800,   -2612845.594949010800,   -1238381.309264990000,  -68893.964198275571,   438885.274271283760,   196483.450798006640,   -543710.752776018460,   -1351410.096298384700,   -1832754.043932119600,   -1798984.812347019100,   -1322511.374219772600,   -669569.457572471120,   -154058.292397760700,   12715.081042153091,   -192154.070546353350,   -613906.115789872130,   -1012105.232017679100,   -1182796.487034805800,   -1050910.014211899100,   -692712.994890551550,   -284652.151222760790,   -9794.485996247822,   29907.992906958636,   -145489.941791920690,   -414969.899420856730,   -623858.128893216610,   -661121.579575710000,   -508817.995753127150,  -242495.255070956800,   13359.189765230905,   149525.762822580730,   123372.304406806460,   -26291.055630596315,   -204861.582938828270,   -312301.241536170830,  -291851.865960343040,   -153764.711586469700,   35274.649379224873,   189027.366306878130,   244870.891755692460,   191906.086605070450,   71735.386820144937,  -46123.859443098227,   -100140.860956496250,   -65694.585846304486,   36338.532681772769,   153699.901033334520,   230771.824292377420,   235761.458983662360,  173996.326576700900,   82145.383908409916,   7834.227804398466,   -14214.628976233740,   22278.040895937291,   94661.075966248754,   164784.314659345080,  198626.361225714590,   182138.429479329670,   126068.318717698970,   58514.083012983225,   9941.748837217998,   -1435.435089704405,   23017.056574605864,  65086.098687933038,   100173.918815260260,   109707.790047866420,   89386.789296062983,   49760.897819694626,   9675.051931353492,   -13564.990887834099,  -12285.076521924077,   8856.011724039889,   36262.999932527979,   54655.421020079091,   54353.267105798383,   35202.834955948223,   5684.232130008360,  -21834.131754634964,   -37076.239085741465,   -36199.413958022458,   -22731.129695456577,   -5118.372907260558,   7577.854152659756,   9672.786019503692,  833.236951097002,   -14531.592459388859,   -29793.431330613086,   -39151.041397179586,   -39818.163597717896,   -32583.700188171191,   -20831.984619100207,  -8734.760604818373,   452.220733624337,   5322.812921556313,   6289.653728314896,   4858.354339454193,   3425.916797995239};

		float rCw1kCoeffs_Q[206] = {   -170.980399102030,   1768.296367079377,   5835.945804681312,   11796.565201846633,   18133.479007744547,   22255.807055389378,   21357.978850183859,   13735.181420231707,   -0.000000000049,   -16503.547886697295,   -30545.696335154938,   -37025.893738326478,  -33411.747046818702,   -21437.095882231028,   -7031.221122970947,   1862.935386117399,   -1134.463519266932,   -17432.718977890690,   -42012.486233933545,  -64985.990947121958,   -75850.806443157853,   -68665.669932586694,   -45607.806381755247,   -16858.964652119030,   3628.660443251266,   4822.726940634635,  -15404.998703644240,   -48212.843626421425,   -77205.081506916060,   -86188.403253953176,   -67707.861757923572,   -27935.035897813086,   15098.788868638427,  39773.286972907052,   32014.776179223369,   -6289.052046459743,   -56382.459435717428,   -90597.521122112812,   -85951.924343938750,   -36820.077871132125,  40153.185181715780,   112348.043431783330,   146564.013100768790,   126345.048309834480,   62278.358731673368,   -10896.747326570579,   -50197.999166518224,  -26225.726516152565,   60032.243992480915,   174802.864045822500,   266216.037708046560,   289974.606590650980,   233492.438048806070,   125572.667818840990,  25117.715115309373,   -7178.208119017167,   59195.188148176414,   205471.074958253970,   368711.518810829730,   470707.483652759340,   457657.408475766720,  330708.367497876690,   150396.351345899510,   10224.216546080263,   -9569.161676973867,   114472.769633641070,   331383.130778279560,   535686.599830899270,  617867.833232840290,   521665.323328070810,   278686.115568886340,    0.000000001455,   -174924.481245685570,   -149163.469372827560,   76204.323125246287,  392746.490814459280,   630853.969739610330,   642743.729782844310,   382499.964065405130,   -59030.918534866134,   -487404.191302017310,   -695640.196377637450,  -571917.529366698470,   -169670.965044850750,   300501.816669035350,   561835.484896631560,   407940.732938562810,   -180015.581687097380,   -998064.656942026220,  -1694614.660675115900,   -1932462.890565141800,   -1567900.714254722000,   -760171.593731659930,   52185.149960383103,   315330.627974595350,   -393823.107362315580,   -2142600.081675804700,   -4543181.792011546000,   -6837562.124754539700,   -8149630.813312892800,   -7813373.360451224300,  -5647352.369420899100,   -2060588.200040315500,   2060588.200040229800,   5647352.369420830200,   7813373.360451181400,   8149630.813312881600,  6837562.124754553700,   4543181.792011573000,   2142600.081675834000,   393823.107362337590,   -315330.627974584930,   -52185.149960382529,   760171.593731656320,  1567900.714254720100,   1932462.890565146500,   1694614.660675127300,   998064.656942041590,   180015.581687112540,   -407940.732938551640,   -561835.484896625860,  -300501.816669034010,   169670.965044850660,   571917.529366700100,   695640.196377642690,   487404.191302025920,   59030.918534876109,   -382499.964065396290,  -642743.729782838490,   -630853.969739607880,   -392746.490814459220,   -76204.323125246534,   149163.469372828750,   174924.481245688980,    0.000000003671,  -278686.115568880920,   -521665.323328066620,   -617867.833232838310,   -535686.599830899390,   -331383.130778280790,   -114472.769633642120,   9569.161676974089,  -10224.216546078533,   -150396.351345896870,   -330708.367497874250,   -457657.408475765380,   -470707.483652759580,   -368711.518810831360,   -205471.074958256010,   -59195.188148178044,   7178.208119016561,   -25117.715115308984,   -125572.667818840140,   -233492.438048805550,   -289974.606590651270,  -266216.037708047840,   -174802.864045824450,   -60032.243992482901,   26225.726516151099,   50197.999166517526,   10896.747326570512,   -62278.358731673252,  -126345.048309834660,   -146564.013100769570,   -112348.043431784710,   -40153.185181717447,   36820.077871130597,   85951.924343937688,   90597.521122112317,  56382.459435717348,   6289.052046459756,   -32014.776179223565,   -39773.286972907597,   -15098.788868639273,   27935.035897812159,   67707.861757922816,  86188.403253952754,   77205.081506915973,   48212.843626421534,   15404.998703644342,   -4822.726940634710,   -3628.660443251573,   16858.964652118571,  45607.806381754795,   68665.669932586403,   75850.806443157810,   64985.990947122147,   42012.486233933858,   17432.718977890996,   1134.463519267124,  -1862.935386117355,   7031.221122970884,   21437.095882230951,   33411.747046818695,   37025.893738326595,   30545.696335155182,   16503.547886697615,   0.000000000376,   -13735.181420231440,   -21357.978850183688,   -22255.807055389301,   -18133.479007744547,   -11796.565201846677,   -5835.945804681365,  -1768.296367079418,   170.980399102002};

		float rI11k = 0.0;
		float rQ11k = 0.0;			// clear MAC accumulators

		for (i = 0; i < FIR_LEN_CW1K; i++) {
			rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rCw1kCoeffs_I[i];
			rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rCw1kCoeffs_Q[i];
			if (++iMainFilterIdxIn >= FIR_LEN_CW1K) iMainFilterIdxIn = 0;
		}

		rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
		rQ11k *= 1.0e-8;

		output[0] = rI11k;
		output[1] = rQ11k;

		set_buffer(processorInstance, 3, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class SSB2K4FilterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	SSB2K4FilterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("SSB2K4Filter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("SSB2K4Filter");

		int iMainFilterIdxIn = callArgs[0];
		float rMainFilterQueueI[428],rMainFilterQueueQ[428];
		float output[2];
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)rMainFilterQueueI, 428*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)rMainFilterQueueQ, 428*sizeof(float));

		int FIR_LEN_SSB2K4 = 139;
		float rSsb2k4Coeffs_I[139] = {   3196.159375674025,   64.377993652844,   -3324.936557688513,   5428.323883923569,   14456.142989659247,   -14436.580570071117,   -74251.842375544933,   -88414.024555140000,   -25845.813380049618,   23437.609290352542,   -11689.675225185665,   -49771.841135470757,  -2775.510782297397,   46734.942586908190,   1136.167674499112,   -42007.064664991376,   29305.158060290327,   96681.954692391751,   30880.209945014820,  -24091.013499376702,   79381.810692355546,   165548.500954047780,   66955.393124596449,   -3266.390774739500,   141658.945741886680,   243179.406944610820,  94474.587743719487,   6210.383228370853,   202840.952478740980,   311536.217809336610,   90901.819167463706,   -15859.264288267068,   245012.549411814660,  347926.895411760260,   29726.285129435706,   -91334.293911220913,   250269.824382618100,   329691.407193108690,   -114640.833095561860,   -238116.137558831720,  206974.562100239190,   240117.811347993790,   -361565.875053454190,   -464234.439760979030,   116557.014738598010,   74006.641202455314,   -720608.771309322910,  -763250.952002380970,   39.491508724979,   -158752.150481568470,   -1192571.881967151500,   -1112487.712972972800,   -94849.290055163103,   -432871.686043560910,  -1780391.409757637200,   -1474982.384528015300,   -78852.783095962644,   -711125.508959321890,   -2526078.330711471400,   -1805135.119477483400,  237172.057095480410,   -951062.239979582260,   -3661817.002441550100,   -2056994.499687228800,   1508438.209926613100,   -1113531.364989988500,  -6910515.848177441400,   -2193331.617350278400,   14211964.108450226000,   23829008.306063984000,   14211964.108450210000,   -2193331.617350292900,  -6910515.848177447000,   -1113531.364989988300,   1508438.209926610300,   -2056994.499687233000,   -3661817.002441551500,   -951062.239979581910,  237172.057095479100,   -1805135.119477485500,   -2526078.330711471800,   -711125.508959321190,   -78852.783095963227,   -1474982.384528016400,  -1780391.409757636700,   -432871.686043560040,   -94849.290055163321,   -1112487.712972973000,   -1192571.881967151100,   -158752.150481567690,   39.491508724979,  -763250.952002380970,   -720608.771309322210,   74006.641202455998,   116557.014738598100,   -464234.439760978920,   -361565.875053453610,   240117.811347994370,  206974.562100239270,   -238116.137558831570,   -114640.833095561360,   329691.407193109040,   250269.824382618130,   -91334.293911220826,   29726.285129436066,  347926.895411760430,   245012.549411814630,   -15859.264288267041,   90901.819167463924,   311536.217809336670,   202840.952478740890,   6210.383228370829,  94474.587743719618,   243179.406944610820,   141658.945741886530,   -3266.390774739554,   66955.393124596492,   165548.500954047750,   79381.810692355430,  -24091.013499376764,   30880.209945014831,   96681.954692391708,   29305.158060290225,   -42007.064664991434,   1136.167674499112,   46734.942586908146,  -2775.510782297474,   -49771.841135470786,   -11689.675225185665,   23437.609290352506,   -25845.813380049687,   -88414.024555140044,   -74251.842375544889,  -14436.580570071068,   14456.142989659265,   5428.323883923570,   -3324.936557688510,   64.377993652850,   3196.159375674028};

		float rSsb2k4Coeffs_Q[139] = {   2720.930146741581,   6024.463329226997,   2955.407094876971,   699.904860207703,   21697.830853469488,   52749.104143180281,   35757.802687759839,   -38013.644786204939,   -80717.542230877822,   -38662.787995035680,   2018.299933673448,   -40569.312192211335,  -86550.646801105613,   -43363.689624795727,   121.871560315160,   -60220.286065308712,   -116827.219126577620,   -49131.854560194806,   12502.011431201243,  -70046.423943851158,   -137493.329315974410,   -32240.567672918944,   52234.516363759765,   -61077.615371554260,   -137189.038559176760,   20839.103919546193,  129450.427657469510,   -27209.466754589863,   -108594.982243871390,   118443.101124407690,   247009.608249117210,   28877.124950093857,   -53174.593397740951,  259664.590554185240,   396669.708072581500,   92315.515650185145,   16067.855835815515,   432047.114782216550,   556875.174487109410,   134104.510281413420,  73667.100138029942,   611810.205419428880,   693054.090427038140,   111190.532497208680,   83170.634310758222,   767156.254201696020,   760176.204610735410,  -32643.331711369337,   49.520785615584,   864278.894051960550,   705629.098426021520,   -369371.369292331040,   -227267.321603050630,   874829.230331203780,  466873.607515387700,   -1005625.962280691500,   -667752.030745583590,   783028.217078446180,   -53993.996949597495,   -2166904.332610104200,   -1466993.034366486100,   590528.087081425940,   -1129520.956100788400,   -4646786.405485468000,   -3219872.198677858300,   317583.719699995770,  -4498247.330495094900,   -15687445.059458060000,   -16337238.187879862000,    0.000000010582,   16337238.187879877000,   15687445.059458058000,  4498247.330495089300,   -317583.719699996700,   3219872.198677859700,   4646786.405485466100,   1129520.956100785200,   -590528.087081426750,   1466993.034366486400,   2166904.332610102400,   53993.996949595254,   -783028.217078446760,   667752.030745583470,   1005625.962280690300,   -466873.607515389220,  -874829.230331204370,   227267.321603050540,   369371.369292330060,   -705629.098426022570,   -864278.894051960670,   -49.520785615584,   32643.331711368661,  -760176.204610735990,   -767156.254201695910,   -83170.634310758134,   -111190.532497209090,   -693054.090427038490,   -611810.205419428650,   -73667.100138029753,  -134104.510281413590,   -556875.174487109530,   -432047.114782216260,   -16067.855835815293,   -92315.515650185233,   -396669.708072581390,   -259664.590554184920,  53174.593397741170,   -28877.124950093876,   -247009.608249117130,   -118443.101124407430,   108594.982243871580,   27209.466754589866,   -129450.427657469450,  -20839.103919545978,   137189.038559176870,   61077.615371554253,   -52234.516363759707,   32240.567672919093,   137493.329315974460,   70046.423943851143,  -12502.011431201216,   49131.854560194894,   116827.219126577630,   60220.286065308683,   -121.871560315159,   43363.689624795770,   86550.646801105613,  40569.312192211284,   -2018.299933673458,   38662.787995035702,   80717.542230877807,   38013.644786204852,   -35757.802687759911,   -52749.104143180295,  -21697.830853469473,   -699.904860207698,   -2955.407094876974,   -6024.463329226997,   -2720.930146741578};

		float rI11k = 0.0;
		float rQ11k =0.0;			// clear MAC accumulators

		for (i = 0; i < FIR_LEN_SSB2K4; i++) {
			rI11k += rMainFilterQueueI[iMainFilterIdxIn] * rSsb2k4Coeffs_I[i];
			rQ11k += rMainFilterQueueQ[iMainFilterIdxIn] * rSsb2k4Coeffs_Q[i];
			if (++iMainFilterIdxIn >= FIR_LEN_SSB2K4) iMainFilterIdxIn = 0;
		}

		rI11k *= 1.0e-8;			// normalize (FIR gain ~10^8 !!!)
		rQ11k *= 1.0e-8;

		output[0] = rI11k;
		output[1] = rQ11k;

		set_buffer(processorInstance, 3, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class interpolatorFilterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	interpolatorFilterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("interpolatorFilter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("interpolatorFilter");

		int iInterpolatorParams[2];
		float rInterpolatorQueueI[16],rInterpolatorQueueQ[16];
		float output[2];
		int i;
		get_buffer(processorInstance, 0, (unsigned char*)iInterpolatorParams, 2*sizeof(int));
		get_buffer(processorInstance, 1, (unsigned char*)rInterpolatorQueueI, 16*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)rInterpolatorQueueQ, 16*sizeof(float));

		float rDecIntCoeffs[64] = {   5566.813560324988,   23887.044687837064,   56163.025785228267,   107513.703556381680,   168621.188343821940,   223802.327332353220,   246104.113280885560,   207964.212934614710,   90582.578794280125,   -101631.300424710600,   -332583.665147412340,   -534546.264874449470,  -622538.621962122040,   -521199.182517471610,   -198339.454015620660,   306652.941743057050,   872454.899382253880,   1313584.976739374900,   1428128.708473922200,  1066424.293393449600,   201468.132346975360,   -1023471.474557211500,   -2292006.967941345600,   -3167506.806456387500,   -3195752.993445751700,  -2033752.462656715200,   431234.697026152050,   4010325.271783742600,   8216844.745709528200,   12352536.074754233000,   15658827.490046050000,  17495007.205536410000,   17495007.205536410000,   15658827.490046050000,   12352536.074754233000,   8216844.745709528200,   4010325.271783742600,  431234.697026152050,   -2033752.462656715200,   -3195752.993445751700,   -3167506.806456387500,   -2292006.967941345600,   -1023471.474557211500,  201468.132346975360,   1066424.293393449600,   1428128.708473922200,   1313584.976739374900,   872454.899382253880,   306652.941743057050,   -198339.454015620660,  -521199.182517471610,   -622538.621962122040,   -534546.264874449470,   -332583.665147412340,   -101631.300424710600,   90582.578794280125,   207964.212934614710,  246104.113280885560,   223802.327332353220,   168621.188343821940,   107513.703556381680,   56163.025785228267,   23887.044687837064,   5566.813560324988};

		float rI44k = 0.0;
		float rQ44k = 0.0;			// clear MAC accumulators
		float rTmp;
		i = iInterpolatorParams[0];		// 0..3, 0 just after a new value has been inserted
		int iInterpolatorIdxIn = iInterpolatorParams[1];

		do {					// loop from newest to oldest sample (they are stored "backwards")
			rTmp = rDecIntCoeffs[i];
			rI44k += rInterpolatorQueueI[iInterpolatorIdxIn] * rTmp;
			rQ44k += rInterpolatorQueueQ[iInterpolatorIdxIn] * rTmp;
			if (++iInterpolatorIdxIn >= 16)
				iInterpolatorIdxIn = 0;
			i += 4;
		} while (i < 64);			// last i == 60, 61, 62 or 63 depending on coeff group used

		rI44k *= 4.0e-8;			// normalize (FIR gain ~10^8 !!!)
		rQ44k *= 4.0e-8;

		output[0] = rI44k;
		output[1] = rQ44k;

		set_buffer(processorInstance, 3, (unsigned char*)output, 2*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class rectifyCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	rectifyCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("rectify", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("rectify");

		int np = callArgs[0];
		float *x = (float*)malloc(np*sizeof(float));
		float *y = (float*)malloc(np*sizeof(float));
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)x, np*sizeof(float));

		for (i=0; i<np; i++) {
			if (x[i] >= 0) y[i] = x[i];
			else y[i] = -x[i];
		}

		set_buffer(processorInstance, 2, (unsigned char*)y, np*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class filterCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	filterCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("filter", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("filter");

		int FILTER_ORDER = 50;
		float *coeff = (float*)malloc(2*FILTER_ORDER*sizeof(float));
		get_buffer(processorInstance, 0, (unsigned char*)coeff, 2*FILTER_ORDER*sizeof(float));
		int np = callArgs[1];
		float *x = (float*)malloc(np*sizeof(float));
		float *y = (float*)malloc(np*sizeof(float));
		get_buffer(processorInstance, 2, (unsigned char*)x, np*sizeof(float));

		int i,j;
		float *a = (float*)malloc(FILTER_ORDER*sizeof(float));
		float *b = (float*)malloc(FILTER_ORDER*sizeof(float));
		for (i=0; i<FILTER_ORDER; i++) {
			a[i] = coeff[i];
			b[i] = coeff[FILTER_ORDER+i];
		}

		y[0]=b[0]*x[0];
		for (i=1;i<FILTER_ORDER;i++) {
			y[i]=0.0;
			for (j=0;j<i+1;j++)
				y[i]=y[i]+b[j]*x[i-j];
			for (j=0;j<i;j++)
				y[i]=y[i]-a[j+1]*y[i-j-1];
		}	// end of initial part

		for (i=FILTER_ORDER;i<np;i++) {
			y[i]=0.0;
			for (j=0;j<FILTER_ORDER;j++)
				y[i]=y[i]+b[j]*x[i-j];
			for (j=0;j<FILTER_ORDER-1;j++)
				y[i]=y[i]-a[j+1]*y[i-j-1];
		}
		set_buffer(processorInstance, 3, (unsigned char*)y, np*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class averageCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	averageCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("average", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("average");

		int np = callArgs[0];
		float *x = (float*)malloc(np*sizeof(float));
		float avg;
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)x, np*sizeof(float));

		float sum=0;
		for (i=0; i<np; i++) {
			sum += x[i];
		}
		avg = sum/(float)np;

		set_buffer(processorInstance, 2, (unsigned char*)&avg, 1*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class decodeCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	decodeCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("decode", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("decode");

		int diffLen = callArgs[0];
		int *tokens = (int*)malloc(diffLen*sizeof(int));
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)tokens, diffLen*sizeof(int));
		int textLen = callArgs[2];
		char *text = (char*)malloc(textLen*sizeof(char));

		int code[40][6] = {	1,2,0,0,0,0,	2,1,1,1,0,0,	2,1,2,1,0,0,	2,1,1,0,0,0,	1,0,0,0,0,0,	1,1,2,1,0,0,	2,2,1,0,0,0,	1,1,1,1,0,0,	1,1,0,0,0,0,
			1,2,2,2,0,0,	2,1,2,0,0,0,	1,2,1,1,0,0,	2,2,0,0,0,0,	2,1,0,0,0,0,	2,2,2,0,0,0,	1,2,2,1,0,0,	1,2,1,2,0,0,	1,2,1,0,0,0,
			1,1,1,0,0,0,	2,0,0,0,0,0,	1,1,2,0,0,0,	1,1,1,2,0,0,	1,2,2,0,0,0,	2,1,1,2,0,0,	2,1,2,2,0,0,	2,2,1,1,0,0,
			1,2,1,2,1,2,	2,2,1,1,2,2,	1,1,2,2,1,1,	2,1,1,2,1,0,
			1,2,2,2,2,0,	1,1,2,2,2,0,	1,1,1,2,2,0,	1,1,1,1,2,0,	1,1,1,1,1,0,	2,1,1,1,1,0,	2,2,1,1,1,0,	2,2,2,1,1,0,	2,2,2,2,1,0,
			2,2,2,2,2,0	};

		char symbols[40] = {	'a',		'b',		'c',		'd',		'e',		'f',		'g',		'h',		'i',
			'j',		'k',		'l',		'm',		'n',		'o',		'p',		'q',		'r',
			's',		't',		'u',		'v',		'w',		'x',		'y',		'z',
			'.',		',',		'?',		'/',
			'1',		'2',		'3',		'4',		'5',		'6',		'7',		'8',		'9',
			'0',	};

		int *tokens2 = (int*)malloc(diffLen*sizeof(int));
		int *buff = (int*)malloc(diffLen*sizeof(int));
		int buffLen = 0;
		int textCnt = 0;

		float cut_t,cut_s;
		int sum=0,cnt=0;
		for (i=0; i<diffLen; i++) {
			if (tokens[i]>0) {sum += tokens[i]; cnt++;}
		}
		cut_t = (float)sum/(float)cnt;
		sum=0; cnt=0;
		for (i=0; i<diffLen; i++) {
			if (tokens[i]<0) {sum += tokens[i]; cnt++;}
		}
		cut_s = (float)sum/(float)cnt;

		for (i=1; i<diffLen; i++) {
			if (tokens[i] > 0 && tokens[i] < cut_t) tokens2[i-1] = 1;
			else if (tokens[i] > 0 && tokens[i] >= cut_t) tokens2[i-1] = 2;
			else if (tokens[i] < 0 && tokens[i] <= cut_s) tokens2[i-1] = -2;
			else if (tokens[i] < 0 && tokens[i] > cut_s) tokens2[i-1] = -1;
		}
		tokens2[diffLen-1]=-2;

		int equal,h,j;
		int *newSeq = (int*)malloc(6*sizeof(int));
		int check;
		for (i=0; i<textLen-1; i++) text[i]='_';
		text[textLen-1]='\0';
		for (i=0; i<diffLen; i++) {
			switch(tokens2[i]) {
			case 1:
			case 2:
				buff[buffLen] = tokens2[i];
				buffLen++;
			case -1:
				break;
			case -2:
				if ( textCnt >= textLen-1) {
					set_buffer(processorInstance, 3, (unsigned char*)text, textLen*sizeof(char));
					processorInstance.setRetVal(2);
					processorInstance.returnFromCall();
					processorInstance.postCall();
					return true;
				}
	
				equal = 0;
	
				for (h=0; h<6; h++) {
					if (h<buffLen)
						newSeq[h]=buff[h];
					else newSeq[h]=0;
				}
	
				for (h=0; (h<40 && equal == 0); h++) {
					equal = 1;
					for (j=0; (j<6 && equal == 1); j++)
						if (newSeq[j] != code[h][j]) equal = 0;
				}
				h--;

				if (equal == 1) {
					text[textCnt]=symbols[h];
					check = 0;
				}
				else {
					text[textCnt]='*';
					check = 1;
				}
	
				if ( check != 0 )  {
					set_buffer(processorInstance, 3, (unsigned char*)text, textLen*sizeof(char));
					processorInstance.setRetVal(1);
					processorInstance.returnFromCall();
					processorInstance.postCall();
					return true;
				}
				buffLen=0;
				textCnt++;
			}
		}

		set_buffer(processorInstance, 3, (unsigned char*)text, textLen*sizeof(char));
		processorInstance.setRetVal(0);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class posavgCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	posavgCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("posavg", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("posavg");

		int np = callArgs[0];
		int *x = (int*)malloc(np*sizeof(int));
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)x, np*sizeof(int));
		float avg;

		int sum=0,cnt=0;
		for (i=0; i<np; i++) {
			if (x[i]>0) {sum += x[i]; cnt++;}
		}
		avg = (float)sum/(float)cnt;
//		printf("SysPosAvg: %f %f %f\n",avg,(float)sum,(float)cnt);

		set_buffer(processorInstance, 2, (unsigned char*)&avg, 1*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class negavgCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	negavgCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("negavg", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("negavg");

		int np = callArgs[0];
		int *x = (int*)malloc(np*sizeof(int));
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)x, np*sizeof(int));
		float avg;

		int sum=0,cnt=0;
		for (i=0; i<np; i++) {
			if (x[i]<0) {sum += x[i]; cnt++;}
		}
		avg = (float)sum/(float)cnt;
//		printf("SysNegAvg: %f %f %f\n",avg,(float)sum,(float)cnt);

		set_buffer(processorInstance, 2, (unsigned char*)&avg, 1*sizeof(float));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class decodeSymbolCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	decodeSymbolCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("decodeSymbol", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("decodeSymbol");

		int seqLen = callArgs[0];
		int *sequence = (int*)malloc(seqLen*sizeof(int));
		int i;
		get_buffer(processorInstance, 1, (unsigned char*)sequence, seqLen*sizeof(float));
		char symbol;

		int code[40][6] = {	1,2,0,0,0,0,	2,1,1,1,0,0,	2,1,2,1,0,0,	2,1,1,0,0,0,	1,0,0,0,0,0,	1,1,2,1,0,0,	2,2,1,0,0,0,	1,1,1,1,0,0,	1,1,0,0,0,0,
			1,2,2,2,0,0,	2,1,2,0,0,0,	1,2,1,1,0,0,	2,2,0,0,0,0,	2,1,0,0,0,0,	2,2,2,0,0,0,	1,2,2,1,0,0,	1,2,1,2,0,0,	1,2,1,0,0,0,
			1,1,1,0,0,0,	2,0,0,0,0,0,	1,1,2,0,0,0,	1,1,1,2,0,0,	1,2,2,0,0,0,	2,1,1,2,0,0,	2,1,2,2,0,0,	2,2,1,1,0,0,
			1,2,1,2,1,2,	2,2,1,1,2,2,	1,1,2,2,1,1,	2,1,1,2,1,0,
			1,2,2,2,2,0,	1,1,2,2,2,0,	1,1,1,2,2,0,	1,1,1,1,2,0,	1,1,1,1,1,0,	2,1,1,1,1,0,	2,2,1,1,1,0,	2,2,2,1,1,0,	2,2,2,2,1,0,
			2,2,2,2,2,0	};

		char symbols[40] = {	'a',		'b',		'c',		'd',		'e',		'f',		'g',		'h',		'i',
			'j',		'k',		'l',		'm',		'n',		'o',		'p',		'q',		'r',
			's',		't',		'u',		'v',		'w',		'x',		'y',		'z',
			'.',		',',		'?',		'/',
			'1',		'2',		'3',		'4',		'5',		'6',		'7',		'8',		'9',
			'0',	};

		int j;
		int equal = 0;
		int *newSeq = (int*)malloc(6*sizeof(int));

		for (i=0; i<6; i++) {
			if (i<seqLen)
				newSeq[i]=sequence[i];
			else newSeq[i]=0;
		}

		for (i=0; (i<40 && equal == 0); i++) {
			equal = 1;
			for (j=0; (j<6 && equal == 1); j++)
				if (newSeq[j] != code[i][j]) equal = 0;
		}
		i--;

		if (equal == 1) {
			symbol=symbols[i];
			processorInstance.setRetVal(0);
		}
		else {
			symbol='*';
			processorInstance.setRetVal(1);
		}

		set_buffer(processorInstance, 2, (unsigned char*)&symbol, 1*sizeof(char));

		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

template <typename issueWidth> class normalizeCall : public reconfCB <issueWidth>{
private:
	configEngine* cE;
public:
	normalizeCall(configEngine* mycE, sc_time latency = SC_ZERO_TIME, unsigned int width = 1, unsigned int height = 1):
		reconfCB<issueWidth>(latency, width, height), cE(mycE)	{}
	bool operator()(ABIIf<issueWidth> &processorInstance){
		processorInstance.preCall();
		std::vector< issueWidth > callArgs = processorInstance.readArgs();

		(this->cE)->executeForce("normalize", this->latency, this->width, this->height,true);
//		(this->cE)->printSystemStatus();
//		(this->cE)->manualRemove("normalize");

		ssize_t unusedRV;
		char inputpath[512];
		char outputpath[512];
		int i;
		i = 0;
		do{
			inputpath[i] = processorInstance.readCharMem(callArgs[0]+i);
			i++;
		} while(inputpath[i-1] != '\0' && i < 512);
		i = 0;
		do{
			outputpath[i] = processorInstance.readCharMem(callArgs[1]+i);
			i++;
		} while(outputpath[i-1] != '\0' && i < 512);

		short 	myChannels;
		short 	myBitsPerSample;
		int	myDataSize;
		int samples;

		printf("\n(SysCall) Opening %s...\n\n",inputpath);
		int wavfd = ::open(inputpath, O_RDONLY);
		if(wavfd == -1) {
			printf("\n(SysCall) ERROR: File %s cannot be opened!\n\n", inputpath);
			processorInstance.setRetVal(1);
			processorInstance.returnFromCall();
			processorInstance.postCall();
			return true;
		}
		::lseek( wavfd, 22, SEEK_SET);
		unusedRV = ::read( wavfd, (char*) &myChannels, 2);
		::lseek( wavfd, 34, SEEK_SET);
		unusedRV = ::read( wavfd, (char*) &myBitsPerSample, 2);
		::lseek( wavfd, 40, SEEK_SET);
		unusedRV = ::read( wavfd, (char*) &myDataSize, 4);
		samples = myDataSize / ( myChannels * myBitsPerSample/8 );

		printf("\n(SysCall) Creating new %s...\n\n", outputpath);
		int wavout = open(outputpath, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
		if(wavout == -1) {
			printf("\n(SysCall) ERROR: File %s cannot be created!\n\n", outputpath);
			printf("\n(SysCall) Closing file %d...\n\n",wavfd);
			::close(wavfd);
			processorInstance.setRetVal(1);
			processorInstance.returnFromCall();
			processorInstance.postCall();
			return true;
		}

		char	myChunkID[5];
		char	myFormat[5];
		char	mySubChunk1ID[5];
		int	mySubChunk1Size = 16;
		short 	myAudioFormat = 1;
		int   	mySampleRate = 44100;
		int   	myByteRate = mySampleRate * myChannels * myBitsPerSample/8;
		short 	myBlockAlign = myChannels * myBitsPerSample/8;
		char	mySubChunk2ID[5];
		myDataSize = samples * myChannels * myBitsPerSample/8;
		int 	myChunkSize = myDataSize + 32;

		unusedRV = write( wavout, (char*) "RIFF", 4);
		unusedRV = write( wavout, (char*) &myChunkSize, 4);
		unusedRV = write( wavout, (char*) "WAVE", 4);
		unusedRV = write( wavout, (char*) "fmt ", 4);
		unusedRV = write( wavout, (char*) &mySubChunk1Size, 4);
		unusedRV = write( wavout, (char*) &myAudioFormat, 2);
		unusedRV = write( wavout, (char*) &myChannels, 2);
		unusedRV = write( wavout, (char*) &mySampleRate, 4);
		unusedRV = write( wavout, (char*) &myByteRate, 4);
		unusedRV = write( wavout, (char*) &myBlockAlign, 2);
		unusedRV = write( wavout, (char*) &myBitsPerSample, 2);
		unusedRV = write( wavout, (char*) "data", 4);
		unusedRV = write( wavout, (char*) &myDataSize, 4);

		printf("\n(SysCall) Normalization: starting peak search...\n\n");
		int myIntData;
		int16_t my16Data;
		unusedRV = ::read( wavfd, (int16_t*) &my16Data, 2);
		myIntData=my16Data;

		int max = myIntData,tmp;
		for (i=0; i<samples; i++) {
			unusedRV = ::read( wavfd, (int16_t*) &my16Data, 2);
			myIntData=my16Data;
			tmp = abs(myIntData);
			if ( tmp > max ) max = tmp;
		}

		printf("\n(SysCall) Normalization: ...peak found, equal to %d; process begins...\n\n",max);
		::lseek( wavfd, 44, SEEK_SET);

		for (i=0; i<samples; i++) {
			unusedRV = ::read( wavfd, (int16_t*) &my16Data, 2);
			myIntData=my16Data;
			tmp = myIntData;
			tmp = 32768 * tmp / max;
			unusedRV = ::write(wavout, (char*) &tmp, 2);
		}
		printf("\n(SysCall) Normalization: ...process terminated!\n\n");

		printf("\n(SysCall) Closing file %d...\n\n",wavfd);
		::close(wavfd);
		printf("\n(SysCall) Closing file %d...\n\n",wavout);
		::close(wavout);

		processorInstance.setRetVal(0);
		processorInstance.returnFromCall();
		processorInstance.postCall();
		return true;
	}
};

};
