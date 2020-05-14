#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QFile>
#include <QAudioInput>
#include <memory>

namespace asr
{
class AudioRecorder
{
    struct WavHeader
    {
        // RIFF头
        char RiffName[4];
        uint32_t nRiffLength;
        // 数据类型标识符
        char WavName[4];
        // 格式块中的块头
        char FmtName[4];
        uint32_t nFmtLength;
        // 格式块中的块数据
        uint16_t nAudioFormat;
        uint16_t nChannleNumber;
        uint32_t nSampleRate;
        uint32_t nBytesPerSecond;
        uint16_t nBytesPerSample;
        uint16_t nBitsPerSample;
        // 数据块中的块头
        char DATANAME[4];
        uint32_t nDataLength;
    };
    std::unique_ptr<QFile> file;
    std::unique_ptr<QAudioInput> audio;

  public:
    //语音原始文件
    AudioRecorder(const char *file) : file(new QFile(file))
    {
        QAudioFormat audioFormat;
        audioFormat.setByteOrder(QAudioFormat::LittleEndian);
        audioFormat.setChannelCount(1);
        audioFormat.setCodec("audio/pcm");
        audioFormat.setSampleRate(16000);
        audioFormat.setSampleSize(16);
        audioFormat.setSampleType(QAudioFormat::SignedInt);

        QAudioDeviceInfo devInfo = QAudioDeviceInfo::defaultInputDevice();
        if (!devInfo.isFormatSupported(audioFormat))          //当前使用设备是否支持
            audioFormat = devInfo.nearestFormat(audioFormat); //转换为最接近格式

        audio.reset(new QAudioInput(devInfo, audioFormat));
    }
    void startRecord()
    {
        file->open(QIODevice::WriteOnly | QIODevice::Truncate);
        audio->start(file.get());
    }

    void endRecord()
    {
        //添加wav文件头
        WavHeader wavHeader;
        qstrcpy(wavHeader.RiffName, "RIFF");
        qstrcpy(wavHeader.WavName, "WAVE");
        qstrcpy(wavHeader.FmtName, "fmt ");
        qstrcpy(wavHeader.DATANAME, "data");
        wavHeader.nFmtLength = 16;
        int nAudioFormat = 1;
        wavHeader.nAudioFormat = nAudioFormat;
        wavHeader.nBitsPerSample = 16;
        wavHeader.nChannleNumber = 1;
        wavHeader.nSampleRate = 16000;
        wavHeader.nBytesPerSample = wavHeader.nChannleNumber * wavHeader.nBitsPerSample / 8;
        wavHeader.nBytesPerSecond = wavHeader.nSampleRate * wavHeader.nChannleNumber * wavHeader.nBitsPerSample / 8;
        wavHeader.nRiffLength = file->size() - 8 + sizeof(WavHeader);
        wavHeader.nDataLength = file->size();
        //写到IO设备头
        audio->stop();
        file->seek(0);
        file->write(reinterpret_cast<char *>(&wavHeader), sizeof(WavHeader));
        file->close();
    }
};
}

#endif // AUDIORECORDER_H
