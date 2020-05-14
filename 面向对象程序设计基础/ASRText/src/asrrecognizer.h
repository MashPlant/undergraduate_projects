#ifndef ASRRECOGNIZER_H
#define ASRRECOGNIZER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <memory>
#include <stdexcept>

#include "./iFlyASR/qisr.h"
#include "./iFlyASR/msp_cmn.h"
#include "./iFlyASR/msp_errors.h"

namespace asr
{

class ASRException : public std::runtime_error
{
  public:
    explicit ASRException(const char *msg) : std::runtime_error(msg) {}
    template<typename Str>
    explicit ASRException(Str &&msg) : std::runtime_error(std::forward<Str>(msg)) {}
};

class ASRRecognizer
{
    const char *const file;
    const static int BufSize = 4096;
    const static int FrameLen = 640;
    const static int HintSize = 100;

    //Helper class for exception
    struct FileCloser
    {
        void operator()(FILE *file) const
        {
            if (file != nullptr)
                fclose(file);
        }
    };
    struct SessionEnder
    {
        std::unique_ptr<char []> hints; //hints为结束本次会话的原因描述，由用户自定义
        SessionEnder():hints(new char[HintSize]{}) {}
        void operator()(const char *session_id) const
        {
            QISRSessionEnd(session_id, hints.get());
        }
    };

  public:
    ASRRecognizer(const ASRRecognizer &) = delete;
    ASRRecognizer &operator=(const ASRRecognizer &) = delete;
    ASRRecognizer(ASRRecognizer &&) = delete;
    ASRRecognizer &operator=(ASRRecognizer &&) = delete;

    ASRRecognizer(const char *file) : file(file)
    {
        const char *login_params = "appid = 5ade6580, work_dir = ."; // 登录参数，appid与msc库绑定
        if (MSPLogin(nullptr, nullptr, login_params) != MSP_SUCCESS) //第一个参数是用户名，第二个参数是密码，均传nullptr即可，第三个参数是登录参数
            throw ASRException("login fail");
    }
    std::unique_ptr<char []> recognize() const
    {
        /*
        * sub:				请求业务类型
        * domain:			领域
        * language:			语言
        * accent:			方言
        * sample_rate:		音频采样率
        * result_type:		识别结果格式
        * result_encoding:	结果编码格式
        */
        const char *session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";
        std::unique_ptr<const char, SessionEnder> session_id;
        std::unique_ptr<char []> rec_result(new char[BufSize]{});
        unsigned total_len = 0;
        int aud_stat = MSP_AUDIO_SAMPLE_CONTINUE; //音频状态
        int ep_stat = MSP_EP_LOOKING_FOR_SPEECH;  //端点检测
        int rec_stat = MSP_REC_STATUS_SUCCESS;	//识别状态
        int errcode = MSP_SUCCESS;

        std::unique_ptr<FILE, FileCloser> f_pcm;
        std::unique_ptr<char []> p_pcm;
        long pcm_count = 0;
        long pcm_size = 0;
        long read_size = 0;

        f_pcm.reset(fopen(file, "rb"));
        if (f_pcm.get() == nullptr)
            throw ASRException("open ["+std::string(file)+"] failed");

        fseek(f_pcm.get(), 0, SEEK_END);
        pcm_size = ftell(f_pcm.get()); //获取音频文件大小
        fseek(f_pcm.get(), 0, SEEK_SET);

        p_pcm.reset(new char[pcm_size]{});

        read_size = fread(p_pcm.get(), 1, pcm_size, f_pcm.get()); //读取音频文件内容
        if (read_size != pcm_size)
               throw ASRException("read ["+std::string(file)+"] error");

        session_id.reset(QISRSessionBegin(nullptr, session_begin_params, &errcode)); //听写不需要语法，第一个参数为nullptr
        if (MSP_SUCCESS != errcode)
            throw ASRException("QISRGetResult failed, error code: "+std::to_string(errcode));

        while (true)
        {
            unsigned int len = 10 * FrameLen; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
            int ret = 0;

            if (pcm_size < 2 * len)
                len = pcm_size;
            if (len <= 0)
                break;

            aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
            if (0 == pcm_count)
                aud_stat = MSP_AUDIO_SAMPLE_FIRST;

            ret = QISRAudioWrite(session_id.get(), &p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
            if (MSP_SUCCESS != ret)
                throw ASRException("QISRGetResult failed, error code: "+std::to_string(errcode));

            pcm_count += len;
            pcm_size -= len;

            if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
            {
                const char *rslt = QISRGetResult(session_id.get(), &rec_stat, 0, &errcode);
                if (MSP_SUCCESS != errcode)
                {
                    printf("\nQISRGetResult failed! error code: %d\n", errcode);
                    //goto iat_exit;
                }
                if (nullptr != rslt)
                {
                    unsigned int rslt_len = strlen(rslt);
                    total_len += rslt_len;
                    if (total_len >= BufSize)
                           throw ASRException("no enough buffer for rec_result");
                    strncat(rec_result.get(), rslt, rslt_len);
                }
            }

            if (MSP_EP_AFTER_SPEECH == ep_stat)
                break;
            usleep(200 * 1000); //模拟人说话时间间隙。200ms对应10帧的音频
        }
        errcode = QISRAudioWrite(session_id.get(), nullptr, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
        if (MSP_SUCCESS != errcode)
               throw ASRException("QISRAudioWrite failed! error code: "+std::to_string(errcode));

        while (MSP_REC_STATUS_COMPLETE != rec_stat)
        {
            const char *rslt = QISRGetResult(session_id.get(), &rec_stat, 0, &errcode);
            if (MSP_SUCCESS != errcode)
                   throw ASRException("QISRGetResult failed, error code: "+std::to_string(errcode));
            if (nullptr != rslt)
            {
                unsigned int rslt_len = strlen(rslt);
                total_len += rslt_len;
                if (total_len >= BufSize)
                       throw ASRException("no enough buffer for rec_result");
                strncat(rec_result.get(), rslt, rslt_len);
            }
            usleep(150 * 1000); //防止频繁占用CPU
        }
        return rec_result;
    }
    ~ASRRecognizer()
    {
        MSPLogout(); //退出登录
    }
};
}

#endif // ASRRECOGNIZER_H
