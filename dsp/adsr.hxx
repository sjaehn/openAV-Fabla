
#ifndef OPENAV_DSP_ADSR_H
#define OPENAV_DSP_ADSR_H

// Adsr class, where 0-1 is the output
class ADSR
{
  public:
    /// setup ADSR with samplerate (Hz), attack decay and release in millisecs,
    /// and sustain as a value between [0-1]
    ADSR(int srate, int attackMS, int decayMS, float sustain01, int releaseMS )
    {
      attack  = sr/attackMS;
      decay   = sr/decayMS;
      sustain = sustain01;
      rel     = sr/releaseMS;
      
      finish = true;
      
      w = 10.0f / (srate * 1); // t = 1? // FS = sample rate
      a = 0.07f;               // Controls damping
      b = 1.0f / (1.0f - a);
      g1 = g2 = 0.f;
      
      progress = attack + decay + rel;
      released = true;
    }
    
    void trigger()
    {
      progress = 0;
      released = false;
      finish = false;
    }
    
    void release()
    {
      progress = attack + decay;
      released = true;
    }
    
    bool finished()
    {
      return finish;
    }
    
    float process(int nframes)
    {
      progress += nframes;
      
      float output = 0.f;
      
      if ( progress < attack ) // ATTACK
      {
        output = (progress / attack);
      }
      else if ( progress < attack + decay ) // DECAY
      {
        output = 1 - (1 - sustain) * (progress - attack) / decay;
      }
      else if ( released && progress > attack + decay && progress < attack + decay + rel )
      {
        output = sustain - ((sustain) * (progress - (attack+decay)) / rel);
      }
      else if ( !released )
      {
        output = sustain;
      }
      else
      {
        finish = true; // note end of sound: so can stop voice
      }
      
      
      if ( false )
      {
        g1 += w * (output - g1 - a * g2 - 1e-20f);
        g2 += w * (b * g1 - g2 + 1e-20f);
      }
      else
      {
        return output;
      }
      
      return g2;
    }
  
  private:
    int sr;
    
    enum State {
      STATE_ATTACK,
      STATE_DECAY,
      STATE_SUSTAIN,
      STATE_RELEASE,
    };
    
    // smoothing filter state
    float w, a, b, g1, g2;
    
    int attack, decay, rel;
    bool released;
    float sustain;
    bool finish;
    
    float progress;
};

#endif // OPENAV_DSP_ADSR_H
