#include "MicroBit.h"

MicroBit uBit;

// マイクの音声ストリームをP0に流すためのデータシンククラス
class AudioPassthrough : public DataSink {
public:
    AudioPassthrough() {
        // マイクの出力をこのクラスに接続
        uBit.microphone.output.setSink(*this);
    }

    virtual int pullRequest() {
        // マイクから音声バッファを取得
        ManagedBuffer buf = uBit.microphone.output.pull();
        if (buf.length() == 0) return DEVICE_OK;

        int16_t *samples = (int16_t *)buf.bytes();
        int length = buf.length() / sizeof(int16_t);

        // サンプルごとにP0ピンのPWM値へ変換して出力
        for (int i = 0; i < length; i++) {
            // 16ビット符号付きPCM (-32768〜32767) を PWMの範囲 (0〜1023) にスケーリング
            int val = (samples[i] + 32768) * 1023 / 65535;
            uBit.io.P0.setAnalogValue(val);
        }
        
        return DEVICE_OK;
    }
};

AudioPassthrough *passthrough = NULL;

// MakeCode用ブロック定義（必要に応じて）
//% 
namespace AudioHacks {
    //% block="マイクの音声をP0ピンに出力する"
    void startPassthrough() {
        uBit.init();
        uBit.microphone.enable();
        if (!passthrough) {
            passthrough = new AudioPassthrough();
        }
    }
}
