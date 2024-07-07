using System.Collections.Generic;
using FMODUnity;

namespace _Project._200_Dev.Audio
{
    [System.Serializable]
    public class TimeStamp
    {
        public float Timestamp;
        public string Message; 
    }
    [System.Serializable]
    public class AudioHelperItem 
    {
        public StudioEventEmitter eventEmitter;
        public string eventName; 
        public List<TimeStamp> timeStamps; 

    }
}
