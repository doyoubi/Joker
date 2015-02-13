#ifndef JOKER_METRONOME
#define JOKER_METRONOME

#include <functional>

namespace joker
{

    class Metronome
    {
    public:
        // the first element of rhythmPoints should be zero
        Metronome(const std::vector<float> & rhythmPoints, float hitDeltaTime);

        void reset(); // reset the Metronome to start point
        void start();
        void update(float dt);
        
        void tab();

        typedef std::function<void(int)> MissCallBack;
        typedef std::function<void(int, float)> TabCallBack; // accept index of tag point and delta time of rhythm point
        typedef std::function<void(int)> RhythmCallBack;
        typedef std::function<void(int)> StartHitCallBack;

        void setMissCallBack(MissCallBack missCallBack);
        void setWrongHitCallBack(TabCallBack wrongHitCallBack);
        void setHitCallBack(TabCallBack hitCallBack);
        void setRhythmCallBack(RhythmCallBack rhythmCallBack);
        void setStartHitCallBack(StartHitCallBack startHitCallBack);

        // the time loop is equivalent to
        /*
        passedTime = 0          // total passed time
        timeSinceLastPoint = 0  // delta time since last rhythm point
        rhythmPoints[]          // array of interval time between neighbour rhythm points, the first element should be zero
        n                       // numbers of rhythm point
        i = 1                   // index of next rhythm point, note that 1 is the second index
        while(i < n)
        {
            dt = get_dt() // get delta time since last loop
            passedTime += dt
            timeSinceLastPoint += dt
            if(timeSinceLastPoint >= rhythmPoints[i])
            {
                timeSinceLastPoint -= dt
                i++
            }
            // And the following invariants hold true:
            // passedTime == sum(rhythmPoints[0...i-1]) + timeSinceLastPoint
            // timeSinceLastPoint >= 0
            // sum(rhythmPoints[0...i-1]) <= passedTime < sum(rhythmPoints[0...i])
        }
        */

    private:
        MissCallBack _missCallBack;
        TabCallBack _hitCallBack;
        TabCallBack _wrongHitCallBack;
        RhythmCallBack _rhythmCallBack;
        StartHitCallBack _startHitCallBack;
        float _hitDeltaTime;

        std::vector<float> _rhythmPoints;
        std::vector<int> _hitted;
        bool _rhythmPointChecked;

        bool _startHitTag;

        int _nextRhythmPointsIndex;
        float _passedTime;
        float _timeSinceLastPoint;

        void operator=(const Metronome &) = delete;
        Metronome(const Metronome &) = delete;
    };

}

#endif
